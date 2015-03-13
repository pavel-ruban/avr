; ----------------------------------------------------------------------------------------------------------
; 						Includes files
; ----------------------------------------------------------------------------------------------------------
; Имена регистров и портов ввода\вывода
.include	"include/m48PAdef.inc"
; ----------------------------------------------------------------------------------------------------------
; 						Макросы
; ----------------------------------------------------------------------------------------------------------

.macro	pushf
					push	r16				; сохранить флаговый регистр на стек
					in	r16,	SREG
					push	r16
.endm

.macro outi	port	const							; вывести в порт константу
					ldi	R16,	\const
			.if \port < 0x40
					out	\port,	R16
			.else
					sts	\port,	R16
			.endif
.endm

.macro	popf									; восстановить флаги со стека
					pop	r16
					out	SREG,	r16
					pop	r16
.endm

.macro	invb	register	shift						; инвертирует заданный бит регистра
					ldi	r16,		(1 << \shift)
			.if \register < 0x40
					in	r17,		\register
					eor	r17,		r16
					out	\register,	r17
			.else
					lds	r17,		\register
					eor	r17,		r16
					sts	\register,	r17
			.endif
.endm

.macro	incm	counter								; инкремент 4х байтного числа в памяти
					lds	r16,	\counter
					lds	r17,	\counter + 1
					lds	r18,	\counter + 2
					lds	r19,	\counter + 3

					subi	r16,	(-1)
					sbci	r17,	(-1)
					sbci	r18,	(-1)
					sbci	r19,	(-1)

					sts	\counter,	r16
					sts	\counter + 1,	r17
					sts	\counter + 2,	r18
					sts	\counter + 3,	r19
.endm

; ----------------------------------------------------------------------------------------------------------
; 					Данные оперативной памяти
; ----------------------------------------------------------------------------------------------------------
.data
; Используется для счетчика событий переполнения таймера
.org SRAM_START
tcounter:				.byte	4

.text
; 0x00 Адрес начала работы контроллера
; Таблица векторов прерываний ------------------------------------------------------------------------------
.org 0x00
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
					rjmp	Timer0_Overflow		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
					reti				;18 0x011 SPI, STC SPI Serial Transfer Complete
					reti				;19 0x012 USART, RX USART Rx Complete
					reti				;20 0x013 USART, UDRE USART, Data Register Empty
					reti				;21 0x014 USART, TX USART, Tx Complete
					reti				;22 0x015 ADC ADC Conversion Complete
					reti				;23 0x016 EE READY EEPROM Ready


; Обработчики прерываний -----------------------------------------------------------------------------------

Timer0_Overflow:			pushf
					push	r17
					push	r18
					push	r19

					incm	tcounter

					pop	r19
					pop	r18
					pop	r17
					popf
					reti

; Начало программы -----------------------------------------------------------------------------------------
; Инициализация локальной перифeрии ------------------------------------------------------------------------

main:					; Initialize ports settings & set their data
					sbi	DDRC,	1		; Initialize port's pin as output
					sbi	DDRB,	1
					sbi	DDRB,	2
					cbi	DDRC,	0		; Initialize port's pin as input
					cbi	DDRD,	4

					lds	r17,	TIMSK0		; Включим прерывания переполнения таймера 0
					ori	r17,	1
					sts	TIMSK0, r17

					outi	TCCR0B,	1 << CS00	; Запускаем таймер. Предделитель = 1
									; Т.е. тикаем с тактовой частотой.

					ldi	r16,	lo8(RAMEND)	; инициализация стека
					out	SPL,	r16
					ldi	r16,	hi8(RAMEND)
					out	SPH,	r16

					sei				; Разрешаем глобальные прерывания

; ----------------------------------------------------------------------------------------------------------

; Проверяем прошел ли заданный интервал времени ------------------------------------------------------------
loop:					lds	r16,	tcounter
					cpi	r16,	0x12
					brcs	idle
					lds	r16,	tcounter + 1
					cpi	r16,	0x7a
					brcs	idle

; Если прошел выполняем обработчик события иначе выше переходим на холостую итерацию -----------------------
handle:					invb	PORTC,		1	; Инвертируем светодиод
					cli				; Отключаем прерывания
					clr	r16
					sts	tcounter,	r16	; Обнуляем счетчик в ОЗУ
					sts	tcounter + 1,	r16
					sts	TCNT0,		r16	; Обнуляем счетчик таймера
					sei				; Включаем прерывания
					rjmp	loop

; Холостые итерации - обрабатываем полезную нагрузки пока событие таймера не наступило ---------------------
idle:					sbis	PIND,	4
					cbi	PORTB,	1
					sbic	PIND,	4
					sbi	PORTB,	1
					rjmp	loop

