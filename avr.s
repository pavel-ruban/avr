; ----------------------------------------------------------------------------------------------------------
; 						Includes files
; ----------------------------------------------------------------------------------------------------------
; Имена регистров и портов ввода\вывода
.include	"include/m48PAdef.inc"
; ----------------------------------------------------------------------------------------------------------
; 						Макросы
; ----------------------------------------------------------------------------------------------------------

.macro	pushf									; сохранить флаговый регистр на стек
					push	r16
					in	r16,	SREG
					push	r16
.endm

.macro	outi	r	v							; вывести в порт константу
					ldi	R16,	\v
			.if \r < 0x40
					out	\r,	R16
			.else
					sts	\r,	R16
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
					lds	r18,		\ctn + 2
					lds	r19,		\ctn + 3

					subi	r16,		(-1)		; -1 = 0b11111111, поэтому при вычитании из любого числа регистра кроме 0xFF
					sbci	r17,		(-1)		; всегда будет перенос бита и флаг C в SREG, команда sbci - subtract immediate
					sbci	r18,		(-1)		; with carry - вычитает константу из буфера и бит C (переноса), поскольку почти
					sbci	r19,		(-1)		; всегда перенос = 1 то r17 - 1 + 1 = r17

					sts	\ctn,		r16
					sts	\ctn + 1,	r17
					sts	\ctn + 2,	r18
					sts	\ctn + 3,	r19
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

; ----------------------------------------------------------------------------------------------------------
; 					Данные оперативной памяти
; ----------------------------------------------------------------------------------------------------------
.data
; Используется для счетчика событий переполнения таймера
.org	SRAM_START
TCNT:					.byte	4

.text
; 0x00 Адрес начала работы контроллера
; Таблица векторов прерываний ------------------------------------------------------------------------------
.org	0x00
reset:					rjmp	main			;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog System Reset
					reti				;2 0x001 INT0 External Interrupt Request 0
					reti				;3 0x002 INT1 External Interrupt Request 1
					reti				;4 0x003 PCINT0 Pin Change Interrupt Request 0
					reti				;5 0x004 PCINT1 Pin Change Interrupt Request 1
					reti				;6 0x005 PCINT2 Pin Change Interrupt Request 2
					reti				;7 0x006 WDT Watchdog Time-out Interrupt
					reti				;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
					reti				;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
					reti				;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
					reti				;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
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


; Обработчики прерываний -----------------------------------------------------------------------------------

TIMER0_OVF:				pushf
					push	r17
					push	r18
					push	r19

					incm	TCNT

					pop	r19
					pop	r18
					pop	r17
					popf
					reti

; Начало программы -----------------------------------------------------------------------------------------
; Инициализация локальной перифeрии ------------------------------------------------------------------------

main:					; Initialize ports settings & set their data
					sbi	DDRB,	1
					sbi	DDRB,	2
					cbi	DDRC,	0		; Initialize port's pin as input
					cbi	DDRD,	4

					setbi	TIMSK0, TOIE0		; Включим прерывания переполнения таймера 0

					outi	TCCR0B,	1 << CS00	; Запускаем таймер. Предделитель = 1
									; Т.е. тикаем с тактовой частотой.

					ldi	r16,	lo8(RAMEND)	; инициализация стека
					out	SPL,	r16
					ldi	r16,	hi8(RAMEND)
					out	SPH,	r16

					sei				; Разрешаем глобальные прерывания

; ----------------------------------------------------------------------------------------------------------

; Проверяем прошел ли заданный интервал времени ------------------------------------------------------------
loop:					lds	r16,	TCNT
					cpi	r16,	0x12
					brcs	idle
					lds	r16,	TCNT + 1
					cpi	r16,	0x7a
					brcs	idle

; Если прошел выполняем обработчик события иначе выше переходим на холостую итерацию -----------------------
handle:					invb	PORTC,		1	; Инвертируем светодиод
					cli				; Отключаем прерывания
					clr	r16
					sts	TCNT,		r16	; Обнуляем счетчик в ОЗУ
					sts	TCNT + 1,	r16
					sts	TCNT0,		r16	; Обнуляем счетчик таймера
					sei				; Включаем прерывания
					rjmp	loop

; Холостые итерации - обрабатываем полезную нагрузки пока событие таймера не наступило ---------------------
idle:					sbis	PIND,	4
					cbi	PORTB,	1
					sbic	PIND,	4
					sbi	PORTB,	1
					rjmp	loop

