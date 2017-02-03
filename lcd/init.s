; Подключаемые файлы ---------------------------------------------------------------------------------------
; Имена регистров и портов ввода\вывода --------------------------------------------------------------------
.include	"include/m88def.inc"
; Настройки портов и архитектуро-независимый интерфейс доступа к ним
#include	"config.h"
; Символы ----------------------------------------------------------------------------------------------------

.equ					XTAL,		F_CPU
.equ					baudrate,	9600
.equ					bauddivider,	XTAL / (16 * baudrate) - 1
.text

.global uart_init
uart_init:				ldi 	r16,	lo8(bauddivider)	; Настраиваем бод режим, параметры связи UART'а
					sts 	UBRR0L,	r16
					ldi 	r16, 	hi8(bauddivider)
					sts 	UBRR0H,	r16

					ldi 	r16,	0
					sts 	UCSR0A,	R16

					; Прерывания разрешены, прием-передача разрешен.
					ldi 	r16,	(1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); | (1 << TXCIE0) | (1 << UDRIE0)
					sts 	UCSR0B,	r16

					; Формат кадра - 8 бит, пишем в регистр UCSR0C, за это отвечает бит селектор
					ldi 	r16,	(1 << UCSZ00) | (1 << UCSZ01)
					sts 	UCSR0C,	r16

					ret

.global io_ports_init
io_ports_init:				sbi	LED_DDR,		LED		; Initialize ports settings & set their data

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

start_tx_cnt:				cpi	r19,	0x00		; Если не конец строки отсылаем в uart
					breq	stop_tx_cnt
					subi	r19,	1
					ld	r18,	Z+		; Достанем из флеша константную строку согласно контексту

					rcall	uart_send
					rjmp	start_tx_cnt

stop_tx_cnt:
					ret
					ldi	r18,	0x0A
					rcall	uart_send
					ldi	r18,	0x0D
					rcall	uart_send

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
