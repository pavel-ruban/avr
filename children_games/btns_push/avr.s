; Подключаемые файлы ---------------------------------------------------------------------------------------
; Имена регистров и портов ввода\вывода --------------------------------------------------------------------
.include	"include/m88def.inc"
; Настройки портов и архитектуро-независимый интерфейс доступа к ним
#include	"config.h"
; Символы ----------------------------------------------------------------------------------------------------

.equ					XTAL,		F_CPU
.equ					baudrate,	9600
.equ					bauddivider,	XTAL / (16 * baudrate) - 1
.equ					F_JOB,		0			; Флаг занятости процессора, указывает может ли прерывание перейти на обработку
										; данных сразу или запланировать до момента когда процессор освободится
.equ					F_RFID,		1			; Флаг о сигнализации наличия работы связанной с RFID
.equ					F_ETH,		2			; Флаг о сигнализации наличия работы связанной с ethernet
.equ					F_RX,		3			; Флаг о сигнализации наличия работы связанной с UART
;.equ					F_TX,		4			; Похоже этот флаг не нужен; Флаг о сигнализации наличия работы связанной с UART

.equ					rx,		0			; Целевой буфер, используется для макросов работы с пользовательскими кольцевыми буферами
.equ					tx,		1
.equ					read,		0			; Операция над буфером, используется для макросов работы с пользовательскими кольцевыми буферами
.equ					write,		1

.equ					FLAGS,		0x14			; r20 использован для флагов (программного планировщика задач) поскольку
										; флаги используются очень часто (каждую итерацию главного цикла) то чтобы
                                                                                ; избежать потерь на ненужные операции для работы с памятью они всегда хранятся
										; в регистре

; Макросы --------------------------------------------------------------------------------------------------

.macro	pushf									; сохранить флаговый регистр на стек
					push	r16
					in	r16,	SREG
					push	r16
.endm

.macro	outi	r	v							; вывести в порт константу
					ldi	r16,	\v
			.if \r < 0x40
					out	\r,	r16
			.else
					sts	\r,	r16
			.endif
.endm

.macro	popf									; восстановить флаги со стека
					pop	r16
					out	SREG,	r16
					pop	r16
.endm

.macro	invb	r	b							; инвертирует заданный бит регистра
					ldi	r16,	(1 << \b)
			.if \r < 0x40
					in	r17,	\r
					eor	r17,	r16
					out	\r,	r17
			.else
					lds	r17,	\r
					eor	r17,	r16
					sts	\r,	r17
			.endif
.endm

.macro	incm	ctn								; инкремент 4х байтного числа в памяти
					lds	r16,		\ctn
					lds	r17,		\ctn + 1
					;lds	r18,		\ctn + 2
					;lds	r19,		\ctn + 3

					subi	r16,		(-1)		; -1 = 0b11111111, поэтому при вычитании из любого числа регистра кроме 0xFF
					sbci	r17,		(-1)		; всегда будет перенос бита и флаг C в SREG, команда sbci - subtract immediate
					;sbci	r18,		(-1)		; with carry - вычитает константу из буфера и бит C (переноса), поскольку почти
					;sbci	r19,		(-1)		; всегда перенос = 1 то r17 - 1 + 1 = r17

					sts	\ctn,		r16
					sts	\ctn + 1,	r17
					;sts	\ctn + 2,	r18
					;sts	\ctn + 3,	r19
.endm

.macro	lincm	ctn								; инкремент 4х байтного числа в памяти
					lds	r16,		\ctn
					lds	r17,		\ctn + 1
					lds	r18,		\ctn + 2
					;lds	r19,		\ctn + 3

					subi	r16,		(-1)		; -1 = 0b11111111, поэтому при вычитании из любого числа регистра кроме 0xFF
					sbci	r17,		(-1)		; всегда будет перенос бита и флаг C в SREG, команда sbci - subtract immediate
					sbci	r18,		(-1)		; with carry - вычитает константу из буфера и бит C (переноса), поскольку почти
					;sbci	r19,		(-1)		; всегда перенос = 1 то r17 - 1 + 1 = r17

					sts	\ctn,		r16
					sts	\ctn + 1,	r17
					sts	\ctn + 2,	r18
					;sts	\ctn + 3,	r19
.endm

.macro	setbs	p	b
					lds	r16,	\p
					sbr	r16,	1 << \b
					sts	\p,	r16
.endm

.macro	setbi	r	b	tr=r16						; установить бит в регистре I/O используя оптимальные оп коды в зависимости от регистра
			.if	\r <= 0x20
					sbi	\r,	\b
			.else
				.if	\r <= 0x40				; 0x40 = 64 адреса, т.к. опкод содержит 5 бит для адресации IO все что выше
										; обрабатывается опкодами общей адресации памяти lds / sts
					in	\tr,	\r
					sbr	\tr,	1 << \b
					out	\r,	\tr
				.else						; здесь расширенные регистры ввода / вывода которые не уместились в размер операнда
					lds	\tr,	\r			; in / out, поэтому вместо размера 2 байта эти порты обрабатываются опкодами в 4
					sbr	\tr,	1 << \b			; байта из стандартной адресации RAM
					sts	\r,	\tr
				.endif
			.endif
.endm

.macro	schd_job	j							; Описание работы планировщика задач -----------------------------------------------
					cpi	FLAGS,		1 << F_JOB	;
					brne	do_job_\@			;   Планирует работу, если процессор свободен F_JOB == 0, значит выставляет флаг работы,
					; Запланировать работу			; типа работы и устанавливает адрес возврата на стеке на обработчик данного типа работ.
					ori	FLAGS,		1 << \j		;
					rjmp	schd_exit_\@ 			;   В случае если процессор занят то устанавливает бит типа работ (есть работа, надо
					; Устанавливаем флаг работы		; делать) и выходит из прерывания как обычно. Дальше, когда главный цикл завершит
do_job_\@:				ori	FLAGS,		1 << F_JOB	; текущую работу он проверит появились ли работы из прерываний и если да начнет делать
					ori	FLAGS,		1 << \j		; согласно приоритету.
schd_exit_\@:									;   После того как все работы будут завершены снимается флаг типов работ и работы и
										; процессор засыпает до тех пор пока прерывания не разбудит его для выполнения новых
										; работ.
										; ----------------------------------------------------------------------------------
.endm

.macro	upd_buffp	bp	bstart	bend	op				; Инкрементированный адрес указателя чтения лежит в Z, проверяем его на конец и если
					push	r16				; не конец обновляем указатель иначе переносим его в начало буфера

					ldi	r16,	lo8(bend)
					cp	ZL,	r16
					brne	1f
					ldi	r16,	hi8(bend)
					cp	ZH,	r16
					brne	1f

					; Если указатель вышел за рамки буфера то вернем его в начало (кольцевой буфер)
					ldi	ZL,	lo8(bstart)
					ldi	ZH,	hi8(bstart)

1:					pop	r16

.endm

.macro	buff_in	READ_BUFF	r							; Считываем данные в кольцевые (пользовательские) буфера UART
			.equ WRITE_BUFF,	0
			.equ BUFF_START,	0
			.equ BUFF_END,		0

			; Макрос реализован в общей манере, чтобы его можно использовать с разными типами кольцевых буферов
			.if \READ_BUFF == RX_BUFF_IN
				.set WRITE_BUFF,	RX_BUFF_OUT
				.set BUFF_START,	RX_BUFF
				.set BUFF_END,		RX_BUFF_END
			.endif

					lds	ZL,	\READ_BUFF		; С помощью указателя чтения из буфера определим доступны ли данные, если указатель не
					lds	ZH,	\READ_BUFF + 1		; равен указателю OUT значит данные имеются, считаем байт в регистр и инкрементируем
										; указатель на 1 (проверим что если следующий адрес конец буфера, то перенесем указатель
					lds	\r,	WRITE_BUFF		; в начало.
					cp	ZL,	\r
					brne	1f				; Если данных нету (IN == OUT) то снимем флаг соответствующего типа работы и выйдем

					lds	\r,	WRITE_BUFF + 1
					cp	ZH,	\r
					brne	1f

					; IN == OUT, снимаем флаг, завершаем работу
					cbr	FLAGS,	1 << F_RX
					rjmp	2f

					; Грузим данные в регистр из кольцевого буфера
1:					lds	\r,	Z+

					; Инкрементируем указатель чтения буфера с проверкой на конец
					upd_buffp	READ_BUFF,	BUFF_START,	BUFF_END

					; Сохраняем обновленный указатель буфера в память
					sts	READ_BUFF,	ZL
					sts	READ_BUFF + 1,	ZH
2:

.endm

.macro	clr_tcnt	tcnt
					ldi	r24,	lo8(\tcnt)
					ldi	r25,	hi8(\tcnt)

					rcall	clear_tcn
.endm
.macro	clr_ltcnt	tcnt
					ldi	r24,	lo8(\tcnt)
					ldi	r25,	hi8(\tcnt)

					rcall	clear_ltcn
.endm

.macro	buff_out	WRITE_BUFF	r					; Записываем данные в кольцевые (пользовательские) буфера UART
			.equ READ_BUFF,		0
			.equ BUFF_START,	0
			.equ BUFF_END,		0

			; Макрос реализован в общей манере, чтобы его можно использовать с разными типами кольцевых буферов
			.if \WRITE_BUFF == TX_BUFF_OUT
				.set READ_BUFF,		TX_BUFF_IN
				.set BUFF_START,	TX_BUFF
				.set BUFF_END,		TX_BUFF_END
			.endif

					lds	ZL,	\WRITE_BUFF		; С помощью указателя чтения из буфера определим доступны ли данные, если указатель не
					lds	ZH,	\WRITE_BUFF + 1		; равен указателю OUT значит данные имеются, считаем байт в регистр и инкрементируем
					sts	Z+,	\r			; указатель на 1 (проверим что если следующий адрес конец буфера, то перенесем указатель
					; Изменим указатель на следующий адрес (учитывая кольцевую структуру)
					upd_buffp	WRITE_BUFF,	BUFF_START,	BUFF_END

					; Проверим не равен ли новый адрес указателю чтения, если да это означает что обработчик не успел считать данные и мы не можем
					; их повредить, также это означает что записываемый байт будет перетерт при следующей записи т.к. буфер уперся в барьер.
					lds	\r,	READ_BUFF
					cp	ZL,	\r
					brne	1f				; Если OUT != IN то записываем инкрементированный указатель в память

					lds	\r,	READ_BUFF + 1
					cp	ZH,	\r
					brne	1f

					; Иначе завершаем работу.
					rjmp	2f

					; Обновляем указатель на запись
1:					sts	WRITE_BUFF,	ZL
					sts	WRITE_BUFF + 1,	ZH
2:

.endm

.macro	game_over_complete_action
					rcall	game_over_complete_script
.endm

.macro	game_over_action
					rcall	game_over_blink
.endm

.macro	left_game_over_complete_action
					rcall	left_game_over_complete_script
.endm

.macro	left_game_over_action
					rcall	left_game_over_blink
.endm

.macro	right_game_over_complete_action
					rcall	right_game_over_complete_script
.endm

.macro	right_game_over_action
					rcall	right_game_over_blink
.endm

.macro	game_start_complete_action
					rcall	game_start_complete_script
.endm

.macro	game_start_action
					rcall	game_start_blink
.endm

.macro	game_select_complete_action
					rcall	game_select_complete_script
.endm

.macro	game_select_action
					rcall	game_select_blink
.endm

.macro	game_idle_action
					rcall	game_idle_action_script
.endm

.macro	game_action
					ldi	r18,		MOD_IDLE
					sts	context,	r18
.endm


.macro	net_complete_action
					cbi	LED_WHITE_PORT,	LED_WHITE
.endm

.macro	rfid_action
					rcall	rfid
.endm

.macro	lan_action
 					rcall	lan_poll
.endm

.macro	converter_action
	 				rcall	open_door_script
.endm

.macro	open_signal_start_action
					sbi	LED_RFID_PORT,	LED_RFID
.endm

.macro	open_signal_complete_action
					cbi	LED_RFID_PORT,	LED_RFID
.endm

.macro	rfid_start_action
					sbi	LED_RFID_PORT,	LED_RFID
.endm

.macro	rfid_complete_action
					cbi	LED_RFID_PORT,	LED_RFID
.endm

.macro	denied_signal_start_action
					sbi	LED_WHITE_PORT,	LED_WHITE
.endm

.macro	denied_signal_complete_action
					cbi	LED_WHITE_PORT,	LED_WHITE
.endm

.macro	open_door_start_action
					sbi	LED_RFID_PORT,	LED_RFID
					sbi	LED_GREEN_PORT,	LED_GREEN

					cbi	EMLOCK_MOSFET_PORT,	EMLOCK_MOSFET
.endm

.macro	open_door_complete_action
					cbi	LED_RFID_PORT,	LED_RFID
					cbi	LED_GREEN_PORT,	LED_GREEN

					sbi	EMLOCK_MOSFET_PORT,	EMLOCK_MOSFET
.endm

.macro	button_action
	 				rcall	open_door_script
.endm

.macro	button_trigger	button_port,	button,	action
					sbic	\button_port,	\button
					rjmp	0f
 					\action
0:
.endm

.macro	positive_button_trigger		button_port,	button,	action
					sbis	\button_port,	\button
					rjmp	0f
 					\action
0:
.endm

.macro	flag_timer_scheduler	flag	timer	lbyte_threshold	hbyte_threshold	start_action	complete_action
					lds	r18,	\flag
					cpi	r18,	1
					brne	0f

					\start_action

					lds	r16,	\timer
					cpi	r16,	\lbyte_threshold
					brcs	0f
					lds	r16,	\timer + 1
					cpi	r16,	\hbyte_threshold
					brcs	0f

					clr	r18
					sts	\flag,	r18

					;clr_tcnt	\timer

					\complete_action

					0:

.endm

.macro	timer_scheduler	timer	lbyte_threshold	hbyte_threshold	action
					lds	r16,	\timer
					cpi	r16,	\lbyte_threshold
					brcs	5f
					lds	r16,	\timer + 1
					cpi	r16,	\hbyte_threshold
					brcs	5f
					\action
					clr_tcnt	\timer
5:
.endm

.macro	ltimer_scheduler	timer	lbyte_threshold	mbyte_threshold	hbyte_threshold	action
					lds	r16,	\timer
					cpi	r16,	\lbyte_threshold
					brcs	5f
					lds	r16,	\timer + 1
					cpi	r16,	\mbyte_threshold
					brcs	5f
					lds	r16,	\timer + 2
					cpi	r16,	\hbyte_threshold
					brcs	5f
					\action
					clr_ltcnt	\timer
5:
.endm

.macro	save_context
					pushf
					push	r17
					push	r18
					push	r19

					push	r20
					push	r21
					push	r22
					push	r23
					push	r24
					push	r25
					push	r26
					push	r27
					push	r28
					push	r29
					push	r30
					push	r31
.endm

.macro	restore_context
					pop	r31
					pop	r30
					pop	r29
					pop	r28
					pop	r27
					pop	r26
					pop	r25
					pop	r24
					pop	r23
					pop	r22
					pop	r21
					pop	r20

					pop	r19
					pop	r18
					pop	r17
					popf
.endm

.section .bss ; Сегмент оперативной памяти
; Данные оперативной памяти --------------------------------------------------------------------------------
; Используется для счетчика событий переполнения таймера
.org	SRAM_START
buffer:				.space	50,	0x00
.global buffer
.global	context
.global	sub_context
.global	current_game

context:				.space  1,	0x00
sub_context:				.space  1,	0x00
current_game:				.space  1,	0x00
CTCNT:					.space	2,	0x00
CTCNT2:					.space	2,	0x00
CTCNT3:					.space	3,	0x00
CTCNT4:					.space	2,	0x00
CTCNT5:					.space	3,	0x00
CTCNT6:					.space	3,	0x00
CTCNT7:					.space	3,	0x00
CTCNT8:					.space	3,	0x00
ccc:					.space	2,	0x00
.global  ccc

.global CTCNT6
.global CTCNT3
.global CTCNT4

; 0x00 Адрес начала работы контроллера ---------------------------------------------------------------
.section .vectors
; Таблица векторов прерываний ------------------------------------------------------------------------------
.org	0x00
reset:					rjmp	asm_main		;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog System Reset
					reti				;2 0x001 INT0 External Interrupt Request 0
					reti				;3 0x002 INT1 External Interrupt Request 1
					reti				;4 0x003 PCINT0 Pin Change Interrupt Request 0
					reti				;5 0x004 PCINT1 Pin Change Interrupt Request 1
					reti				;6 0x005 PCINT2 Pin Change Interrupt Request 2
					reti				;7 0x006 WDT Watchdog Time-out Interrupt
					reti				;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
					reti				;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
					rjmp	TIMER2_OVF		;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
					rjmp	TIMER1_ICR		;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
					reti				;12 0x00B TIMER1 COMPA Timer/Counter1 Compare Match A
					reti				;13 0x00C TIMER1 COMPB Timer/Counter1 Compare Match B
					reti				;14 0x00D TIMER1 OVF Timer/Counter1 Overflow
					rjmp	TIMER0_COMP		;15 0x00E TIMER0 COMPA Timer/Counter0 Compare Match A
					rjmp	TIMER0_COMP		;16 0x00F TIMER0 COMPB Timer/Counter0 Compare Match B
					rjmp	TIMER0_OVF		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
					rjmp	SPI_OK			;18 0x011 SPI, STC SPI Serial Transfer Complete
					rjmp	RX_OK			;19 0x012 USART, RX USART Rx Complete
					rjmp	DRE_OK			;20 0x013 USART, UDRE USART, Data Register Empty
					rjmp	TX_OK			;21 0x014 USART, TX USART, Tx Complete
					reti				;22 0x015 ADC ADC Conversion Complete
					reti				;23 0x016 EE READY EEPROM Ready
.text

; Обработчики прерываний -----------------------------------------------------------------------------------

TIMER1_ICR:
					pushf

					push	r31
					push	r30

					push	r29
					push	r28
					push	r27
					push	r26
					push	r25
					push	r24
					push	r23
					push	r22
					push	r21
					push	r20
					push	r19
					push	r18
					push	r17
					push	r16

					push	r15
					push	r14
					push	r13
					push	r12
					push	r11
					push	r10
					push	r9
					push	r8
					push	r7
					push	r6
					push	r5
					push	r4
					push	r3
					push	r2
					push	r1
					push	r0

					rcall	script

					pop	r0
					pop	r1
					pop	r2
					pop	r3
					pop	r4
					pop	r5
					pop	r6
					pop	r7
					pop	r8
					pop	r9
					pop	r10
					pop	r11
					pop	r12
					pop	r13
					pop	r14
					pop	r15

					pop	r16
					pop	r17
					pop	r18
					pop	r19

					pop	r20
					pop	r21
					pop	r22
					pop	r23
					pop	r24
					pop	r25
					pop	r26
					pop	r27
					pop	r28
					pop	r29

					pop	r30
					pop	r31
					popf
					reti
TIMER0_COMP:
					pushf
					push	r31
					push	r30

					push	r29
					push	r28
					push	r27
					push	r26
					push	r25
					push	r24
					push	r23
					push	r22
					push	r21
					push	r20
					push	r19
					push	r18
					push	r17
					push	r16

					push	r15
					push	r14
					push	r13
					push	r12
					push	r11
					push	r10
					push	r9
					push	r8
					push	r7
					push	r6
					push	r5
					push	r4
					push	r3
					push	r2
					push	r1
					push	r0

					rcall	audio_routine

					pop	r0
					pop	r1
					pop	r2
					pop	r3
					pop	r4
					pop	r5
					pop	r6
					pop	r7
					pop	r8
					pop	r9
					pop	r10
					pop	r11
					pop	r12
					pop	r13
					pop	r14
					pop	r15

					pop	r16
					pop	r17
					pop	r18
					pop	r19

					pop	r20
					pop	r21
					pop	r22
					pop	r23
					pop	r24
					pop	r25
					pop	r26
					pop	r27
					pop	r28
					pop	r29

					pop	r30
					pop	r31
					popf

					reti
TIMER2_OVF:				pushf
					push	r31
					push	r30

					push	r29
					push	r28
					push	r27
					push	r26
					push	r25
					push	r24
					push	r23
					push	r22
					push	r21
					push	r20
					push	r19
					push	r18
					push	r17
					push	r16

					push	r15
					push	r14
					push	r13
					push	r12
					push	r11
					push	r10
					push	r9
					push	r8
					push	r7
					push	r6
					push	r5
					push	r4
					push	r3
					push	r2
					push	r1
					push	r0

					rcall	screen_render

					pop	r0
					pop	r1
					pop	r2
					pop	r3
					pop	r4
					pop	r5
					pop	r6
					pop	r7
					pop	r8
					pop	r9
					pop	r10
					pop	r11
					pop	r12
					pop	r13
					pop	r14
					pop	r15

					pop	r16
					pop	r17
					pop	r18
					pop	r19

					pop	r20
					pop	r21
					pop	r22
					pop	r23
					pop	r24
					pop	r25
					pop	r26
					pop	r27
					pop	r28
					pop	r29

					pop	r30
					pop	r31
					popf

					reti

TIMER0_OVF:				pushf
					push	r17
					push	r18
					push	r19

					incm	CTCNT
					incm	CTCNT2

					lds	r18,	sub_context
					cpi	r18,	SUB_MOD_LEFT_GAME_OVER
					brne	0f
					lincm	CTCNT3
					rjmp	sub_context_end
0:
					lds	r18,	sub_context
					cpi	r18,	SUB_MOD_RIGHT_GAME_OVER
					brne	0f
					lincm	CTCNT3
0:

sub_context_end:
					lds	r18,	context
					cpi	r18,	MOD_GAME
					brne	0f
					incm	CTCNT4
					rjmp	context_end
0:
					lds	r18,	context
					cpi	r18,	MOD_GAME_START
					brne	0f
					lincm	CTCNT8
					incm	CTCNT4
					rjmp	context_end
0:
					lds	r18,	context
					cpi	r18,	MOD_GAME_OVER
					brne	0f
					lincm	CTCNT7
					incm	CTCNT4
					rjmp	context_end
0:
					lds	r18,	context
					cpi	r18,	MOD_IDLE
					brne	0f
					lincm	CTCNT5
					rjmp	context_end
0:

					lds	r18,	context
					cpi	r18,	MOD_GAME_SELECT
					brne	0f
					incm	CTCNT4
					rjmp	context_end
0:

context_end:
					lincm	CTCNT6

				;	incm	CTCNT7
				;	incm	CTCNT8

					pop r19
					pop r18
					pop r17
					popf

					reti

RX_OK:
					lds	r18,	UDR0
					sts	ccc,	r18
					reti

					; Складываем в ОЗУ и с помощью планировщика устанавливаем работу в очередь
					;buff_out	RX_BUFF_OUT,	r16				; Тратить время внутри прерывания на обработку плохая манера, поэтому
													; кладем данные в буфер ОЗУ и переходим к обработке данных вне прерывания
													; если ядро свободно иначе ставим флаг - нужна обработка и после того как
													; ядро закончит другую работу она доделает что установлено во флагах и уснет

					;schd_job	F_RX

					reti

SPI_OK:					;ldi	ZL,	lo8(spi_ok_string)
					;ldi	ZH,	hi8(spi_ok_string)
					reti

;spi_start_tx:				lpm	r16,	Z+		; Достанем из флеша константную строку согласно контексту
;					cpi	r16,	0x00		; Если не конец строки отсылаем в uart
;					breq	spi_stop_tx		; Иначе завершаем передачу
;					rcall	uart_
;					rjmp	spi_start_tx
;
;spi_stop_tx:				reti

TX_OK:
DRE_OK:					reti

; Подпрограммы инициализации -------------------------------------------------------------------------------

uart_init:				ldi 	r16,	lo8(bauddivider)	; Настраиваем бод режим, параметры связи UART'а
					sts 	UBRR0L,	r16
					ldi 	r16, 	hi8(bauddivider)
					sts 	UBRR0H,	r16

			;		ldi	r16,		lo8(RX_BUFF)	; Инициализируем указатели кольцевых буферов UART'a, чтобы избежать
			;		sts	RX_BUFF_IN,	r16		; чтения \ записи по прерыванию в неверном месте
			;		sts	RX_BUFF_OUT,	r16
			;		ldi	r16,		hi8(RX_BUFF)
			;		sts	RX_BUFF_IN + 1,		r16	; чтения \ записи по прерыванию в неверном месте
			;		sts	RX_BUFF_OUT + 1,	r16

			;		ldi	r16,		lo8(TX_BUFF)	; Инициализируем указатели кольцевых буферов UART'a, чтобы избежать
			;		sts	TX_BUFF_IN,	r16		; чтения \ записи по прерыванию в неверном месте
			;		sts	TX_BUFF_OUT,	r16
			;		ldi	r16,		hi8(TX_BUFF)
			;		sts	TX_BUFF_IN + 1,		r16	; чтения \ записи по прерыванию в неверном месте
			;		sts	TX_BUFF_OUT + 1,	r16

					ldi 	r16,	0
					sts 	UCSR0A,	R16

					; Прерывания разрешены, прием-передача разрешен.
					ldi 	r16,	(1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); | (1 << TXCIE0) | (1 << UDRIE0)
					sts 	UCSR0B,	r16

					; Формат кадра - 8 бит, пишем в регистр UCSR0C, за это отвечает бит селектор
					ldi 	r16,	(1 << UCSZ00) | (1 << UCSZ01)
					sts 	UCSR0C,	r16

					ret

timer0_init:				setbi	TIMSK0,	TOIE0				; Включим прерывания переполнения таймера 0
					outi	TCCR0B,	(1 << CS00) | (1 << CS02)	; Запускаем таймер. Предделитель = 1024

					setbi	TIMSK2,	TOIE2				; Включим прерывания переполнения таймера 0
					outi	TCCR2B,	(1 << CS22); | (1 << CS20); | (1 << CS21)	; Запускаем таймер. Предделитель = 1024

					setbi	TIMSK1,	ICIE1				; Включим прерывания переполнения таймера 0
					outi	TCCR1A,	0x00				; Отключим пвм от IO, выберем WGM на мод CTC
					outi	TCCR1B,	(1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10)	; Запускаем таймер. Предделитель = 64
					outi	ICR1H,	0x49
					outi	ICR1L,	0x3E				; при  делителе 64 совпадение раз в 100 миллисекунд (10 раз в секунду)
					;outi	ICR1H,	0x0
					;outi	ICR1L,	0x3E				; при  делителе 64 совпадение раз в 100 миллисекунд (10 раз в секунду)

					ret

pwm_init:				setbi	TCCR0A,	COM0A0
					setbi	TCCR0A,	COM0A1

					setbi	TCCR0A,	COM0B0
					setbi	TCCR0A,	COM0B1

					setbi	TCCR0A,	WGM00
					setbi	TCCR0A,	WGM01
					setbi	TIMSK0,	OCIE0A
				;	setbi	TIMSK0,	OCIE0B
					ret

memory_init:				clr	r16			; Очищаем память в 0x0
					pop	r17			; Сохраним адрес возврата
					pop	r18
					ldi	ZL,	lo8(SRAM_START)
					ldi	ZH,	hi8(SRAM_START)
memory_clr:				st	Z+,	r16
					cpi	ZL,	lo8(RAMEND + 1)
					brne	memory_clr
					cpi	ZH,	hi8(RAMEND + 1)
					brne	memory_clr
					push	r18			; Восстановим адрес перед ret
					push	r17

					ret

io_ports_init:				sbi	LED_DDR,		LED		; Initialize ports settings & set their data
					sbi	SR_OUT_CLK_DDR,		SR_OUT_CLK
					sbi	SR_OUT_STROBE_DDR,	SR_OUT_STROBE
					sbi	SR_OUT_DATA_DDR,	SR_OUT_DATA

					sbi	SR_IN_CLK_DDR,		SR_IN_CLK
					sbi	SR_IN_PL_DDR,		SR_IN_PL
					cbi	SR_IN_DATA_DDR,		SR_IN_DATA

					cbi	SR_IN_FIRST_DDR,	SR_IN_FIRST
					cbi	SR_IN_SECOND_DDR,	SR_IN_SECOND
					cbi	SR_IN_THIRD_DDR,	SR_IN_THIRD
					cbi	SR_IN_FIRST_PORT,	SR_IN_FIRST
					cbi	SR_IN_SECOND_PORT,	SR_IN_SECOND
					cbi	SR_IN_THIRD_PORT,	SR_IN_THIRD

					sbi	SR_SCREEN_CLK_DDR,	SR_SCREEN_CLK
					sbi	SR_SCREEN_STROBE_DDR,	SR_SCREEN_STROBE
					sbi	SR_SCREEN_DATA_DDR,	SR_SCREEN_DATA

					;sbi	AUDIO_PWM_A_DDR,	AUDIO_PWM_A
					;sbi	AUDIO_PWM_B_DDR,	AUDIO_PWM_B
					;cbi	AUDIO_PWM_A_PORT,	AUDIO_PWM_A
					;cbi	AUDIO_PWM_B_PORT,	AUDIO_PWM_B

					;sbi	TX_PORT,		TX
					;cbi	RX_PORT,		RX

					ret

; Подпрограммы коммуникации --------------------------------------------------------------------------------

; Процедура отправки байта
uart_send:				lds 	r17,	UCSR0A		; Пропуск если нет флага готовности
					andi	r17,	1 << UDRE0
					cpi	r17,	1 << UDRE0
					brne	uart_send		; ждем готовности - флага UDRE

					sts	UDR0, r18		; шлем байт

					ret				; Возврат

.global uart_str_send_P
uart_str_send_P: 				; Отправляем в UART строку
					movw	r30,	r24

start_tx_P:				lpm	r18,	Z+		; Достанем из флеша константную строку согласно контексту
					cpi	r18,	0x00		; Если не конец строки отсылаем в uart
					breq	stop_tx_P

					rcall	uart_send
					rjmp	start_tx_P

stop_tx_P:
					ret
.global uart_str_send
uart_str_send: 				; Отправляем в UART строку
					movw	r30,	r24

start_tx:				ld	r18,	Z+		; Достанем из флеша константную строку согласно контексту
					cpi	r18,	0x00		; Если не конец строки отсылаем в uart
					breq	stop_tx

					rcall	uart_send
					rjmp	start_tx

stop_tx:
					ret

asm_main:
; Начало программы -----------------------------------------------------------------------------------------
; Инициализация локальной перифeрии ------------------------------------------------------------------------
					cli

					;rcall	initial_delay
					ldi	r16,	lo8(RAMEND)	; инициализация стека
					out	SPL,	r16
					ldi	r16,	hi8(RAMEND)
                                        out	SPH,	r16

					rcall	memory_init
					rcall	timer0_init
					rcall	io_ports_init
					;rcall	uart_init
					;rcall	pwm_init

					; GCC uses that registers as zero register - очищаем, иначе если при подаче питания там не будет 0 весь GCC код сломается
					clr	r0
					clr	r1

					rcall	main_init

					sei				; Разрешаем глобальные прерывания

; Проверяем прошел ли заданный интервал времени ------------------------------------------------------------
loop:					lds	r16,	CTCNT
					cpi	r16,	0x2d
					brcs	0f
					lds	r16,	CTCNT + 1
					cpi	r16,	0x00
					brcs	0f

; Если прошел выполняем обработчик события иначе выше переходим на холостую итерацию -----------------------
					rcall	handle

; Холостые итерации - обрабатываем полезную нагрузки пока событие таймера не наступило ---------------------
0:
					;rcall	script
					;rjmp	loop

					lds	r16,	CTCNT2
					cpi	r16,	0x01
					brcs	idle
					lds	r16,	CTCNT2 + 1
					cpi	r16,	0x00
					brcs	idle

					rcall	event_listener

					rcall	mod_game_over_handler
					rcall	mod_idle_handler
					rcall	mod_game_select_handler
					rcall	mod_game_start_handler
					; end of Blink leds on MOD_IDLE
					rcall	mod_game_handler
					lds	r18,	context
					cpi	r18,	MOD_GAME_OVER
					breq	idle
					rcall	mod_left_game_over_handler
					rcall	mod_right_game_over_handler


idle:
					rjmp	loop

mod_game_select_handler:
					; Blink leds on MOD_GAME_SELECT
					lds	r18,	context
					cpi	r18,	MOD_GAME_SELECT
					brne	game_select_mod_end

					timer_scheduler	CTCNT4,	0x0b,	0x00,	game_select_action
					ltimer_scheduler	CTCNT6,	0xba,	0xa,	0x00,	game_action

game_select_mod_end:
					ret
mod_game_over_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	context
					cpi	r18,	MOD_GAME_OVER
					brne	game_over_mod_end

					timer_scheduler	CTCNT4,	0x0b,	0x00,	game_over_action

					lds	r16,	CTCNT7
					cpi	r16,	0x5c
					brcs	game_over_mod_end
					lds	r16,	CTCNT7 + 1
					cpi	r16,	0x0
					brcs	game_over_mod_end
					lds	r16,	CTCNT7 + 2
					cpi	r16,	0x0
					brcs	game_over_mod_end

					ldi	r18,		MOD_GAME_SELECT
					sts	context,	r18

					clr_ltcnt	CTCNT7
					clr_ltcnt	CTCNT6

					game_over_complete_action

game_over_mod_end:
					ret

mod_right_game_over_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	sub_context
					cpi	r18,	SUB_MOD_RIGHT_GAME_OVER
					brne	right_game_over_mod_end

					timer_scheduler	CTCNT4,	0xb,	0x0,	right_game_over_action

					lds	r16,	CTCNT3
					cpi	r16,	0x5c
					brcs	right_game_over_mod_end
					lds	r16,	CTCNT3 + 1
					cpi	r16,	0x0
					brcs	right_game_over_mod_end
					lds	r16,	CTCNT3 + 2
					cpi	r16,	0x0
					brcs	right_game_over_mod_end

					ldi	r18,		SUB_MOD_COMMON
					sts	sub_context,	r18

					clr_ltcnt	CTCNT3
					clr_ltcnt	CTCNT6

					right_game_over_complete_action

right_game_over_mod_end:
mod_left_game_over_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	sub_context
					cpi	r18,	SUB_MOD_LEFT_GAME_OVER
					brne	left_game_over_mod_end

					timer_scheduler	CTCNT4,	0xb,	0x0,	left_game_over_action

					lds	r16,	CTCNT3
					cpi	r16,	0x5c
					brcs	left_game_over_mod_end
					lds	r16,	CTCNT3 + 1
					cpi	r16,	0x0
					brcs	left_game_over_mod_end
					lds	r16,	CTCNT3 + 2
					cpi	r16,	0x0
					brcs	left_game_over_mod_end

					ldi	r18,		SUB_MOD_COMMON
					sts	sub_context,	r18

					clr_ltcnt	CTCNT3
					clr_ltcnt	CTCNT6

					left_game_over_complete_action

left_game_over_mod_end:
					ret
mod_game_start_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	context
					cpi	r18,	MOD_GAME_START
					brne	mod_end

					timer_scheduler	CTCNT4,	0xb,	0x0,	game_start_action

					lds	r16,	CTCNT8
					cpi	r16,	0x5c
					brcs	mod_end
					lds	r16,	CTCNT8 + 1
					cpi	r16,	0x0
					brcs	mod_end
					lds	r16,	CTCNT8 + 2
					cpi	r16,	0x0
					brcs	mod_end

					ldi	r18,		MOD_GAME
					sts	context,	r18

					clr_ltcnt	CTCNT8
					clr_ltcnt	CTCNT6

					game_start_complete_action

mod_end:
					ret
mod_idle_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	context
					cpi	r18,	MOD_IDLE
					brne	7f

					ltimer_scheduler	CTCNT5,	0x36,	0x6e,	0x01,	game_idle_action
7:
					ret

mod_game_handler:
					; Blink leds on MOD_GAME_START
					lds	r18,	context
					cpi	r18,	MOD_GAME
					brne	7f

					ltimer_scheduler	CTCNT6,	0xba,	0xa,	0x0,	game_action
7:
					ret
event_listener:
					rcall	events_poll

					sts	CTCNT2,		r1	; Обнуляем счетчик в ОЗУ
					sts	CTCNT2 + 1,	r1
					ret

handle:
					invb	LED_PORT,	LED	; Инвертируем светодиод

					;rcall	sr_out_upd
		;			rcall	script
	;				rcall	sr_in_upd
					;sbi	LED_PORT,	LED
	;				rcall	screen_render

					cli

					clr	r16
					sts	CTCNT,		r16	; Обнуляем счетчик в ОЗУ
					sts	CTCNT + 1,	r16
					sts	TCNT0,		r16	; Обнуляем счетчик таймера
					sei				; Включаем прерывания

					ret
;.global fill_net_buff
;
;fill_net_buff:
;					;movw	X,	r24
;	;				cpi	r22,	1
;					;brne	retval_ignored
;
;retval_ok:			;	ldi	ZL,	lo8(retval_ok_str)
; 				;	ldi	ZH,	hi8(retval_ok_str)
;				;	rjmp	fill_net_buff_next_char
;
;retval_ignored:				;ldi	ZL,	lo8(retval_ignored_str)
; 					;ldi	ZH,	hi8(retval_ignored_str)
;
;fill_net_buff_next_char:	;	lpm	r18,	Z+
;				;	cpi	r18,	0
;				;	breq	fill_net_buff_end
;
;					;ldi	r18,	'o
;					;st	X+,	r18
;					;ldi	r18,	'k
;					;st	X+,	r18
;					;rjmp	fill_net_buff_next_char
;
;fill_net_buff_end:
					ret

.global clear_tcn
clear_tcn:				cli
					movw	ZL,	r24
					clr	r16
					st	Z+,	r16	        ; Обнуляем счетчик в ОЗУ
					st	Z,	r16
					sei				; Включаем прерывания
					ret

clear_ltcn:				cli
					movw	ZL,	r24
					clr	r16
					st	Z+,	r16	        ; Обнуляем счетчик в ОЗУ
					st	Z+,	r16	        ; Обнуляем счетчик в ОЗУ
					st	Z,	r16
					sei				; Включаем прерывания
					ret

.global audio_stream
audio_stream:
		;			lds	ZL,	ccc
		;			lds	ZH,	ccc + 1

		;			lpm	r18,	Z+
					lds	r18,	ccc
					sts	OCR0B,	r18
					ret
