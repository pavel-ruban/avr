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

CTCNT:					.space	2,	0x00

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
					reti 				;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
					rjmp	TIMER1_CPT		;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
					reti				;12 0x00B TIMER1 COMPA Timer/Counter1 Compare Match A
					reti				;13 0x00C TIMER1 COMPB Timer/Counter1 Compare Match B
					reti				;14 0x00D TIMER1 OVF Timer/Counter1 Overflow
					reti				;15 0x00E TIMER0 COMPA Timer/Counter0 Compare Match A
					reti				;16 0x00F TIMER0 COMPB Timer/Counter0 Compare Match B
					rjmp	TIMER0_OVF		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
					reti				;18 0x011 SPI, STC SPI Serial Transfer Complete
					reti				;19 0x012 USART, RX USART Rx Complete
					reti				;20 0x013 USART, UDRE USART, Data Register Empty
					reti				;21 0x014 USART, TX USART, Tx Complete
					reti				;22 0x015 ADC ADC Conversion Complete
					reti				;23 0x016 EE READY EEPROM Ready
.text

; Обработчики прерываний -----------------------------------------------------------------------------------


TIMER1_CPT:				pushf
					push	r17
					push	r18
					push	r19
					push	r1
					eor	r1,	r1

					out	SREG,	r1
					rcall	audio_routine

					pop	r1
					pop r19
					pop r18
					pop r17
					popf
					reti

TIMER0_OVF:				pushf
					push	r17
					push	r18
					push	r19

					incm	CTCNT

					pop r19
					pop r18
					pop r17
					popf
					reti

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

					;setbi	TIMSK2,	TOIE2				; Включим прерывания переполнения таймера 0
					;outi	TCCR2B,	(1 << CS21); | (1 << CS20); | (1 << CS22)	; Запускаем таймер. Предделитель = 1024

		;			setbi	TIMSK1,	ICIE1				; Включим прерывания переполнения таймера 0
		;			outi	TCCR1A,	0x00				; Отключим пвм от IO, выберем WGM на мод CTC
		;			outi	TCCR1B,	(1 << WGM13) | (1 << WGM12) | (1 << CS10); | (1 << CS11)	; Запускаем таймер. Предделитель = 64
					outi	ICR1H,	0x2
					outi	ICR1L,	0x21				; при  делителе 64 совпадение раз в 100 миллисекунд (10 раз в секунду)

					ret

pwm_init:				setbi	TCCR0A,	COM0A0
					setbi	TCCR0A,	COM0A1

					setbi	TCCR0A,	COM0B0
					setbi	TCCR0A,	COM0B1

					setbi	TCCR0A,	WGM00
					setbi	TCCR0A,	WGM01
				;	setbi	TIMSK0,	OCIE0A
					;setbi	TIMSK0,	OCIE0B
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

					sbi	SR_SCREEN_CLK_DDR,	SR_SCREEN_CLK
					sbi	SR_SCREEN_STROBE_DDR,	SR_SCREEN_STROBE
					sbi	SR_SCREEN_DATA_DDR,	SR_SCREEN_DATA

					sbi	AUDIO_PWM_A_DDR,	AUDIO_PWM_A
					sbi	AUDIO_PWM_B_DDR,	AUDIO_PWM_B
					cbi	AUDIO_PWM_A_PORT,	AUDIO_PWM_A
					cbi	AUDIO_PWM_B_PORT,	AUDIO_PWM_B

					sbi	TX_PORT,		TX
					cbi	RX_PORT,		RX

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
.global uart_str_send_cnt
uart_str_send_cnt: 			; Отправляем в UART строку
					movw	r30,	r24
					mov	r19,	r22
					mov	r17,	r23

start_tx_cnt:				cpi	r19,	0x00		; Если не конец строки отсылаем в uart
					breq	stop_tx_cnt
process_tx_cnt:				subi	r19,	1
					sbci	r17,	0
					ld	r18,	Z+		; Достанем из флеша константную строку согласно контексту

					rcall	uart_send
					rjmp	start_tx_cnt

stop_tx_cnt:
					cpi	r17,	0x00
					brne	process_tx_cnt
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
					rcall	uart_init
					rcall	pwm_init

					; GCC uses that registers as zero register - очищаем, иначе если при подаче питания там не будет 0 весь GCC код сломается
					eor	r1,	r1
					out	SREG, r1

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
					rcall	script
					;rcall   adc_update
					;rcall	audio_routine
					rjmp	loop


idle:
					rjmp	loop



handle:
					invb	LED_PORT,	LED	; Инвертируем светодиод
					;rcall	script

					cli

					clr	r16
					sts	CTCNT,		r16	; Обнуляем счетчик в ОЗУ
					sts	CTCNT + 1,	r16
					sts	TCNT0,		r16	; Обнуляем счетчик таймера
					sei				; Включаем прерывания

					ret
.global audio_stream
audio_stream:
					movw	ZL,	r24

					lpm	r18,	Z+
					out	OCR0A,	r18
					ret
