# 1 "avr.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "avr.S"
; Подключаемые файлы ---------------------------------------------------------------------------------------
; Имена регистров и портов ввода\вывода --------------------------------------------------------------------
.include "include/m48PAdef.inc"
; Макросы --------------------------------------------------------------------------------------------------

.set FLAGS, r20 ; r20 использован для флагов (программного планировщика задач) поскольку
  ; r20 ; флаги используются очень часто (каждую итерацию главного цикла) то чтобы
          ; избежать потерь на ненужные операции для работы с памятью они всегда хранятся
          ; в регистре

;.endm

.macro pushf ; сохранить флаговый регистр на стек
     push r16
     in r16, SREG
     push r16
.endm

.macro outi r v ; вывести в порт константу
     ldi R16, \v
   .if \r < 0x40
     out \r, R16
   .else
     sts \r, R16
   .endif
.endm

.macro popf ; восстановить флаги со стека
     pop r16
     out SREG, r16
     pop r16
.endm

.macro invb r b ; инвертирует заданный бит регистра
     ldi r16, (1 << \b)
   .if \r < 0x40
     in r17, \r
     eor r17, r16
     out \r, r17
   .else
     lds r17, \r
     eor r17, r16
     sts \r, r17
   .endif
.endm

.macro incm ctn ; инкремент 4х байтного числа в памяти
     lds r16, \ctn
     lds r17, \ctn + 1
     lds r18, \ctn + 2
     lds r19, \ctn + 3

     subi r16, (-1) ; -1 = 0b11111111, поэтому при вычитании из любого числа регистра кроме 0xFF
     sbci r17, (-1) ; всегда будет перенос бита и флаг C в SREG, команда sbci - subtract immediate
     sbci r18, (-1) ; with carry - вычитает константу из буфера и бит C (переноса), поскольку почти
     sbci r19, (-1) ; всегда перенос = 1 то r17 - 1 + 1 = r17

     sts \ctn, r16
     sts \ctn + 1, r17
     sts \ctn + 2, r18
     sts \ctn + 3, r19
.endm

.macro setbs p b
     lds r16, \p
     sbr r16, 1 << \b
     sts \p, r16
.endm

.macro setbi r b tr=r16 ; установить бит в регистре I/O используя оптимальные оп коды в зависимости от регистра
   .if \r <= 0x20
     sbi \r, \b
   .else
    .if \r <= 0x40 ; 0x40 = 64 адреса, т.к. опкод содержит 5 бит для адресации IO все что выше
          ; обрабатывается опкодами общей адресации памяти lds / sts
     in \tr, \r
     sbr \tr, 1 << \b
     out \r, \tr
    .else ; здесь расширенные регистры ввода / вывода которые не уместились в размер операнда
     lds \tr, \r ; in / out, поэтому вместо размера 2 байта эти порты обрабатываются опкодами в 4
     sbr \tr, 1 << \b ; байта из стандартной адресации RAM
     sts \r, \tr
    .endif
   .endif
.endm


.macro schd_job j ; Описание работы планировщика задач -----------------------------------------------
     cpi r20, 1 << F_JOB ;
     brne do_job_\@ ; Планирует работу, если процессор свободен F_JOB == 0, значит выставляет флаг работы,
     ; Запланировать работу ; типа работы и устанавливает адрес возврата на стеке на обработчик данного типа работ.
     ori r20, 1 << \j ;
     rjmp schd_exit_\@ ; В случае если процессор занят то устанавливает бит типа работ (есть работа, надо
     ; Устанавливаем флаг работы ; делать) и выходит из прерывания как обычно. Дальше, когда главный цикл завершит
do_job_\@: ori r20, 1 << F_JOB ; текущую работу он проверит появились ли работы из прерываний и если да начнет делать
     ori r20, 1 << \j ; согласно приоритету.
schd_exit_\@: ; После того как все работы будут завершены снимается флаг типов работ и работы и
          ; процессор засыпает до тех пор пока прерывания не разбудит его для выполнения новых
          ; работ.
          ; ----------------------------------------------------------------------------------
.endm

; Символы ----------------------------------------------------------------------------------------------------
.equ XTAL, 8000000
.equ baudrate, 9600
.equ bauddivider, XTAL / (16 * baudrate) - 1
.equ F_JOB, 0 ; Флаг занятости процессора, указывает может ли прерывание перейти на обработку
          ; данных сразу или запланировать до момента когда процессор освободится
.equ F_RFID, 1 ; Флаг о сигнализации наличия работы связанной с RFID
.equ F_ETH, 2 ; Флаг о сигнализации наличия работы связанной с ethernet
.equ F_RX, 3 ; Флаг о сигнализации наличия работы связанной с UART
.equ F_TX, 4 ; Флаг о сигнализации наличия работы связанной с UART

.data ; Сегмент оперативной памяти
; Данные оперативной памяти --------------------------------------------------------------------------------
; Используется для счетчика событий переполнения таймера
.org SRAM_START

TCNT: .space 4, 0x00
RX_BUFF_IN: .word 0x00 ; Кольцевые буферы на прием и передачу для UART с использованием
RX_BUFF_OUT: .word 0x00 ; двух указателей для записи и чтения из буфера на каждый
RX_BUFF: .space 48, 0x00
RX_BUFF_END:
TX_BUFF_IN: .word 0x00
TX_BUFF_OUT: .word 0x00
TX_BUFF: .space 48, 0x00
TX_BUFF_END:

.text ; 0x00 Адрес начала работы контроллера ---------------------------------------------------------------
; Таблица векторов прерываний ------------------------------------------------------------------------------
.org 0x00
reset: rjmp main ;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog System Reset
     reti ;2 0x001 INT0 External Interrupt Request 0
     reti ;3 0x002 INT1 External Interrupt Request 1
     reti ;4 0x003 PCINT0 Pin Change Interrupt Request 0
     reti ;5 0x004 PCINT1 Pin Change Interrupt Request 1
     reti ;6 0x005 PCINT2 Pin Change Interrupt Request 2
     reti ;7 0x006 WDT Watchdog Time-out Interrupt
     reti ;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
     reti ;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
     reti ;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
     reti ;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
     reti ;12 0x00B TIMER1 COMPA Timer/Counter1 Compare Match A
     reti ;13 0x00C TIMER1 COMPB Timer/Counter1 Compare Match B
     reti ;14 0x00D TIMER1 OVF Timer/Counter1 Overflow
     reti ;15 0x00E TIMER0 COMPA Timer/Counter0 Compare Match A
     reti ;16 0x00F TIMER0 COMPB Timer/Counter0 Compare Match B
     rjmp TIMER0_OVF ;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
     reti ;18 0x011 SPI, STC SPI Serial Transfer Complete
     rjmp RX_OK ;19 0x012 USART, RX USART Rx Complete
     rjmp DRE_OK ;20 0x013 USART, UDRE USART, Data Register Empty
     rjmp TX_OK ;21 0x014 USART, TX USART, Tx Complete
     reti ;22 0x015 ADC ADC Conversion Complete
     reti ;23 0x016 EE READY EEPROM Ready


; Обработчики прерываний -----------------------------------------------------------------------------------

TIMER0_OVF: pushf
     push r17
     push r18
     push r19

     incm TCNT

     pop r19
     pop r18
     pop r17
     popf

     reti

RX_OK:
     pushf
     push r16

     lds r16, UDR0 ; Считываем данные с Rx буфера, флаг прерывания снимается

         ; Тратить время внутри прерывания на обработку плохая манера, поэтому
         ; кладем данные в буфер ОЗУ и переходим к обработке данных вне прерывания
         ; если ядро свободно иначе ставим флаг - нужна обработка и после того как
         ; ядро закончит другую работу она доделает что установлено во флагах и уснет
     pop r16
     popf

     schd_job F_RX

     reti

TX_OK:
DRE_OK: reti

; Подпрограммы инициализации -------------------------------------------------------------------------------

uart_init: ldi r16, lo8(bauddivider) ; Настраиваем бод режим, параметры связи UART'а
     sts UBRR0L, r16
     ldi r16, hi8(bauddivider)
     sts UBRR0H, r16

     ldi r16, lo8(RX_BUFF) ; Инициализируем указатели кольцевых буферов UART'a, чтобы избежать
     sts RX_BUFF_IN, r16 ; чтения \ записи по прерыванию в неверном месте
     sts RX_BUFF_OUT, r16
     ldi r16, hi8(RX_BUFF)
     sts RX_BUFF_IN + 1, r16 ; чтения \ записи по прерыванию в неверном месте
     sts RX_BUFF_OUT + 1, r16

     ldi r16, lo8(TX_BUFF) ; Инициализируем указатели кольцевых буферов UART'a, чтобы избежать
     sts TX_BUFF_IN, r16 ; чтения \ записи по прерыванию в неверном месте
     sts TX_BUFF_OUT, r16
     ldi r16, hi8(TX_BUFF)
     sts TX_BUFF_IN + 1, r16 ; чтения \ записи по прерыванию в неверном месте
     sts TX_BUFF_OUT + 1, r16

     ldi r16, 0
     sts UCSR0A, R16

     ; Прерывания разрешены, прием-передача разрешен.
     ldi R16, (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0) | (1 << UDRIE0)
     sts UCSR0B, r16

     ; Формат кадра - 8 бит, пишем в регистр UCSRC, за это отвечает бит селектор
     ldi r16, (1 << UCSZ00) | (1 << UCSZ01)
     sts UCSR0C, r16

     ret

timer0_init: setbi TIMSK0, TOIE0 ; Включим прерывания переполнения таймера 0
     outi TCCR0B, 1 << CS00 ; Запускаем таймер. Предделитель = 1
         ; Т.е. тикаем с тактовой частотой.
     ret

memory_init: clr r16 ; Очищаем память в 0x0
     pop r17 ; Сохраним адрес возврата
     pop r18
     ldi ZL, lo8(SRAM_START)
     ldi ZH, hi8(SRAM_START)
memory_clr: st Z+, r16
     cpi ZL, lo8(RAMEND + 1)
     brne memory_clr
     cpi ZH, hi8(RAMEND + 1)
     brne memory_clr
     push r18 ; Восстановим адрес перед ret
     push r17

     ret

io_ports_init: sbi DDRB, 1 ; Initialize ports settings & set their data
     sbi DDRB, 2
     cbi DDRC, 0 ; Initialize port's pin as input
     cbi DDRD, 4

     ret

; Подпрограммы коммуникации --------------------------------------------------------------------------------

; Процедура отправки байта
uart_snt: lds r17, UCSR0A ; Пропуск если нет флага готовности
     andi r17, 1 << UDRE0
     cpi r17, 1 << UDRE0
     brne uart_snt ; ждем готовности - флага UDRE

     sts UDR0, r16 ; шлем байт

     ret ; Возврат

main:
; Начало программы -----------------------------------------------------------------------------------------
; Инициализация локальной перифeрии ------------------------------------------------------------------------

     ldi r16, lo8(RAMEND) ; инициализация стека
     out SPL, r16
     ldi r16, hi8(RAMEND)
                                        out SPH, r16

     rcall memory_init
     rcall io_ports_init
     rcall timer0_init
     rcall uart_init

     sei ; Разрешаем глобальные прерывания

; Проверяем прошел ли заданный интервал времени ------------------------------------------------------------
loop: lds r16, TCNT
     cpi r16, 0x12
     brcs idle
     lds r16, TCNT + 1
     cpi r16, 0x7a
     brcs idle

; Если прошел выполняем обработчик события иначе выше переходим на холостую итерацию -----------------------
     rjmp handle

; Холостые итерации - обрабатываем полезную нагрузки пока событие таймера не наступило ---------------------
idle: sbis PIND, 4
     cbi PORTB, 1
     sbic PIND, 4
     sbi PORTB, 1

     rjmp loop

handle:
     invb PORTC, 1 ; Инвертируем светодиод

     ; Отправляем в UART строку
     lds r16, r20 ; Проверяем посылалось ли приветствие
     andi r16, 0x01

     breq term_init ; Если нет выводим приветствие иначе рабочую строку
     ldi ZL, lo8(term_var_info)
     ldi ZH, hi8(term_var_info)
     rjmp start_tx

term_init: setbs r20, 0 ; Пометим что приветствие было отослано
     ldi ZL, lo8(term_var_init)
     ldi ZH, hi8(term_var_init)

start_tx: lpm r16, Z+ ; Достанем из флеша константную строку согласно контексту
     cpi r16, 0x00 ; Если не конец строки отсылаем в uart
     breq stop_tx ; Иначе завершаем передачу
     rcall uart_snt
     rjmp start_tx

stop_tx: cli
     clr r16
     sts TCNT, r16 ; Обнуляем счетчик в ОЗУ
     sts TCNT + 1, r16
     sts TCNT0, r16 ; Обнуляем счетчик таймера
     sei ; Включаем прерывания
     rjmp loop


; Константы записанные в памяти программы ------------------------------------------------------------------
data:
term_var_init: .string "Serial terminal initialization...\n\r"
term_var_info: .ascii "Terminal version:	Osci 0.01 alpha\n\r"
     .ascii "Creator:		Pavel Ruban 2015/03\n\r"
     .asciz "Description:		Used to control, debug & measure embedded microchips\n\n\r"
     .align 2
