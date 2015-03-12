.equ PORTB, 0x05
.equ DDRB, 0x04
.equ PORTC, 0x08
.equ DDRC, 0x07
.equ PINC, 0x06
.equ PIND, 0x09
.equ PORTD, 0x0b
.equ DDRD, 0x0a
.equ RAMEND, 0x02FF
.equ spl, 0x3d
.equ sph, 0x3e

.text			; Кодовый сегмент

reset:
	rjmp main	;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog System Reset
	reti		;2 0x001 INT0 External Interrupt Request 0
	reti		;3 0x002 INT1 External Interrupt Request 1
	reti		;4 0x003 PCINT0 Pin Change Interrupt Request 0
	reti		;5 0x004 PCINT1 Pin Change Interrupt Request 1
	reti		;6 0x005 PCINT2 Pin Change Interrupt Request 2
	reti		;7 0x006 WDT Watchdog Time-out Interrupt
	reti		;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
	reti		;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
	reti		;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
	reti		;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
	reti		;12 0x00B TIMER1 COMPA Timer/Counter1 Compare Match A
	reti		;13 0x00C TIMER1 COMPB Timer/Coutner1 Compare Match B
	reti		;14 0x00D TIMER1 OVF Timer/Counter1 Overflow
	reti		;15 0x00E TIMER0 COMPA Timer/Counter0 Compare Match A
	reti		;16 0x00F TIMER0 COMPB Timer/Counter0 Compare Match B
	reti		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
	reti		;18 0x011 SPI, STC SPI Serial Transfer Complete
	reti		;19 0x012 USART, RX USART Rx Complete
	reti		;20 0x013 USART, UDRE USART, Data Register Empty
	reti		;21 0x014 USART, TX USART, Tx Complete
	reti		;22 0x015 ADC ADC Conversion Complete
	reti		;23 0x016 EE READY EEPROM Ready

main:
	cli			; отключаем прерывания глобально

	ldi r16, lo8(RAMEND)	; инициализация стека
	out spl, r16
	ldi r16, hi8(RAMEND)
	out sph, r16

	; =========================================== ;
	; Initialize ports settings & set their data. ;
	; =========================================== ;
	sbi DDRC, 1		; Initialize port's pin as output
	sbi DDRB, 1;
	cbi DDRC, 0		; Initialize port's pin as output
	cbi DDRD, 4;

led_blink:
	sbi PORTC, 1;

	rcall delay

	cbi PORTC, 1;

	rcall delay

	;rjmp led_blink

poll:
	sbis PINC, 0
	rjmp turn_off_led
	rjmp light_led

poll_second:
	sbis PIND, 4
	rjmp turn_off_led_b
	rjmp light_led_b


end:
	rjmp poll;

light_led_b:
	sbi PORTB, 2		; Light led
	rjmp poll;

turn_off_led_b:
	cbi PORTB, 2		; Light led
	rjmp poll;

light_led:
	sbi PORTB, 1		; Light led
	rjmp poll_second;

turn_off_led:
	cbi PORTB, 1		; Light led
	rjmp poll_second;

delay:
	ldi r17, 0xff
	ldi r18, 0xff
	ldi r19, 0x7a

iterate:
	subi r17, 1
	sbci r18, 0
	sbci r19, 0
	brcc iterate
	ret

