GAS LISTING avr.s 			page 1


   1               	; Подключаемые файлы -------------------------------------------------------------
   2               	; Имена регистров и портов ввода\вывода ---------------------------
   3               	.include	"include/m8Adef.inc"
   1               	;***** THIS IS A MACHINE GENERATED FILE - DO NOT EDIT ********************
   2               	;***** Created: 2011-02-09 12:03 ******* Source: ATmega8A.xml ************
   3               	;*************************************************************************
   4               	;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
   5               	;*
   6               	;* Number            : AVR000
   7               	;* File Name         : "m8Adef.inc"
   8               	;* Title             : Register/Bit Definitions for the ATmega8A
   9               	;* Date              : 2011-02-09
  10               	;* Version           : 2.35
  11               	;* Support E-mail    : avr@atmel.com
  12               	;* Target MCU        : ATmega8A
  13               	;*
  14               	;* DESCRIPTION
  15               	;* When including this file in the assembly program file, all I/O register
  16               	;* names and I/O register bit names appearing in the data book can be used.
  17               	;* In addition, the six registers forming the three data pointers X, Y and
  18               	;* Z have been assigned names XL - ZH. Highest RAM address for Internal
  19               	;* SRAM is also defined
  20               	;*
  21               	;* The Register names are represented by their hexadecimal address.
  22               	;*
  23               	;* The Register Bit names are represented by their bit number (0-7).
  24               	;*
  25               	;* Please observe the difference in using the bit names with instructions
  26               	;* such as "sbr"/"cbr" (set/clear bit in register) and "sbrs"/"sbrc"
  27               	;* (skip if bit in register set/cleared). The following example illustrates
  28               	;* this:
  29               	;*
  30               	;* in    r16,PORTB             ;read PORTB latch
  31               	;* sbr   r16,(1<<PB6)+(1<<PB5) ;set PB6 and PB5 (use masks, not bit#)
  32               	;* out   PORTB,r16             ;output to PORTB
  33               	;*
  34               	;* in    r16,TIFR              ;read the Timer Interrupt Flag Register
  35               	;* sbrc  r16,TOV0              ;test the overflow flag (use bit#)
  36               	;* rjmp  TOV0_is_set           ;jump if set
  37               	;* ...                         ;otherwise do something else
  38               	;*************************************************************************
  39               	
  40               	#ifndef _M8ADEF_INC_
  41               	#define _M8ADEF_INC_
  42               	
  43               	
  44               	#pragma partinc 0
  45               	
  46               	; ***** SPECIFY DEVICE ***************************************************
  47               	;.device ATmega8A
  48               	#pragma AVRPART ADMIN PART_NAME ATmega8A
  49               	.equ	SIGNATURE_000	,	 0x1e
  50               	.equ	SIGNATURE_001	,	 0x93
  51               	.equ	SIGNATURE_002	,	 0x07
  52               	
  53               	#pragma AVRPART CORE CORE_VERSION V2E
  54               	
GAS LISTING avr.s 			page 2


  55               	
  56               	; ***** I/O REGISTER DEFINITIONS *****************************************
  57               	; NOTE:
  58               	; Definitions marked "MEMORY MAPPED"are extended I/O ports
  59               	; and cannot be used with IN/OUT instructions
  60               	.equ	SREG	,	 0x3f
  61               	.equ	SPL	,	 0x3d
  62               	.equ	SPH	,	 0x3e
  63               	.equ	GICR	,	 0x3b
  64               	.equ	GIFR	,	 0x3a
  65               	.equ	TIMSK	,	 0x39
  66               	.equ	TIFR	,	 0x38
  67               	.equ	SPMCR	,	 0x37
  68               	.equ	TWCR	,	 0x36
  69               	.equ	MCUCR	,	 0x35
  70               	.equ	MCUCSR	,	 0x34
  71               	.equ	TCCR0	,	 0x33
  72               	.equ	TCNT0	,	 0x32
  73               	.equ	OSCCAL	,	 0x31
  74               	.equ	SFIOR	,	 0x30
  75               	.equ	TCCR1A	,	 0x2f
  76               	.equ	TCCR1B	,	 0x2e
  77               	.equ	TCNT1L	,	 0x2c
  78               	.equ	TCNT1H	,	 0x2d
  79               	.equ	OCR1AL	,	 0x2a
  80               	.equ	OCR1AH	,	 0x2b
  81               	.equ	OCR1BL	,	 0x28
  82               	.equ	OCR1BH	,	 0x29
  83               	.equ	ICR1L	,	 0x26
  84               	.equ	ICR1H	,	 0x27
  85               	.equ	TCCR2	,	 0x25
  86               	.equ	TCNT2	,	 0x24
  87               	.equ	OCR2	,	 0x23
  88               	.equ	ASSR	,	 0x22
  89               	.equ	WDTCR	,	 0x21
  90               	.equ	UBRRH	,	 0x20
  91               	.equ	UCSRC	,	 0x20
  92               	.equ	EEARL	,	 0x1e
  93               	.equ	EEARH	,	 0x1f
  94               	.equ	EEDR	,	 0x1d
  95               	.equ	EECR	,	 0x1c
  96               	.equ	PORTB	,	 0x18
  97               	.equ	DDRB	,	 0x17
  98               	.equ	PINB	,	 0x16
  99               	.equ	PORTC	,	 0x15
 100               	.equ	DDRC	,	 0x14
 101               	.equ	PINC	,	 0x13
 102               	.equ	PORTD	,	 0x12
 103               	.equ	DDRD	,	 0x11
 104               	.equ	PIND	,	 0x10
 105               	.equ	SPDR	,	 0x0f
 106               	.equ	SPSR	,	 0x0e
 107               	.equ	SPCR	,	 0x0d
 108               	.equ	UDR	,	 0x0c
 109               	.equ	UCSRA	,	 0x0b
 110               	.equ	UCSRB	,	 0x0a
 111               	.equ	UBRRL	,	 0x09
GAS LISTING avr.s 			page 3


 112               	.equ	ACSR	,	 0x08
 113               	.equ	ADMUX	,	 0x07
 114               	.equ	ADCSRA	,	 0x06
 115               	.equ	ADCL	,	 0x04
 116               	.equ	ADCH	,	 0x05
 117               	.equ	TWDR	,	 0x03
 118               	.equ	TWAR	,	 0x02
 119               	.equ	TWSR	,	 0x01
 120               	.equ	TWBR	,	 0x00
 121               	
 122               	
 123               	; ***** BIT DEFINITIONS **************************************************
 124               	
 125               	; ***** ANALOG_COMPARATOR ************
 126               	; SFIOR - Special Function IO Register
 127               	.equ	ACME	,	 3	; Analog Comparator Multiplexer Enable
 128               	
 129               	; ACSR - Analog Comparator Control And Status Register
 130               	.equ	ACIS0	,	 0	; Analog Comparator Interrupt Mode Select bit 0
 131               	.equ	ACIS1	,	 1	; Analog Comparator Interrupt Mode Select bit 1
 132               	.equ	ACIC	,	 2	; Analog Comparator Input Capture Enable
 133               	.equ	ACIE	,	 3	; Analog Comparator Interrupt Enable
 134               	.equ	ACI	,	 4	; Analog Comparator Interrupt Flag
 135               	.equ	ACO	,	 5	; Analog Compare Output
 136               	.equ	ACBG	,	 6	; Analog Comparator Bandgap Select
 137               	.equ	ACD	,	 7	; Analog Comparator Disable
 138               	
 139               	
 140               	; ***** SPI **************************
 141               	; SPDR - SPI Data Register
 142               	.equ	SPDR0	,	 0	; SPI Data Register bit 0
 143               	.equ	SPDR1	,	 1	; SPI Data Register bit 1
 144               	.equ	SPDR2	,	 2	; SPI Data Register bit 2
 145               	.equ	SPDR3	,	 3	; SPI Data Register bit 3
 146               	.equ	SPDR4	,	 4	; SPI Data Register bit 4
 147               	.equ	SPDR5	,	 5	; SPI Data Register bit 5
 148               	.equ	SPDR6	,	 6	; SPI Data Register bit 6
 149               	.equ	SPDR7	,	 7	; SPI Data Register bit 7
 150               	
 151               	; SPSR - SPI Status Register
 152               	.equ	SPI2X	,	 0	; Double SPI Speed Bit
 153               	.equ	WCOL	,	 6	; Write Collision Flag
 154               	.equ	SPIF	,	 7	; SPI Interrupt Flag
 155               	
 156               	; SPCR - SPI Control Register
 157               	.equ	SPR0	,	 0	; SPI Clock Rate Select 0
 158               	.equ	SPR1	,	 1	; SPI Clock Rate Select 1
 159               	.equ	CPHA	,	 2	; Clock Phase
 160               	.equ	CPOL	,	 3	; Clock polarity
 161               	.equ	MSTR	,	 4	; Master/Slave Select
 162               	.equ	DORD	,	 5	; Data Order
 163               	.equ	SPE	,	 6	; SPI Enable
 164               	.equ	SPIE	,	 7	; SPI Interrupt Enable
 165               	
 166               	
 167               	; ***** EXTERNAL_INTERRUPT ***********
 168               	; GICR - General Interrupt Control Register
GAS LISTING avr.s 			page 4


 169               	.equ	GIMSK	,	 GICR	; For compatibility
 170               	.equ	IVCE	,	 0	; Interrupt Vector Change Enable
 171               	.equ	IVSEL	,	 1	; Interrupt Vector Select
 172               	.equ	INT0	,	 6	; External Interrupt Request 0 Enable
 173               	.equ	INT1	,	 7	; External Interrupt Request 1 Enable
 174               	
 175               	; GIFR - General Interrupt Flag Register
 176               	.equ	INTF0	,	 6	; External Interrupt Flag 0
 177               	.equ	INTF1	,	 7	; External Interrupt Flag 1
 178               	
 179               	; MCUCR - MCU Control Register
 180               	.equ	ISC00	,	 0	; Interrupt Sense Control 0 Bit 0
 181               	.equ	ISC01	,	 1	; Interrupt Sense Control 0 Bit 1
 182               	.equ	ISC10	,	 2	; Interrupt Sense Control 1 Bit 0
 183               	.equ	ISC11	,	 3	; Interrupt Sense Control 1 Bit 1
 184               	
 185               	
 186               	; ***** TIMER_COUNTER_0 **************
 187               	; TIMSK - Timer/Counter Interrupt Mask Register
 188               	.equ	TOIE0	,	 0	; Timer/Counter0 Overflow Interrupt Enable
 189               	
 190               	; TIFR - Timer/Counter Interrupt Flag register
 191               	.equ	TOV0	,	 0	; Timer/Counter0 Overflow Flag
 192               	
 193               	; TCCR0 - Timer/Counter0 Control Register
 194               	.equ	CS00	,	 0	; Clock Select0 bit 0
 195               	.equ	CS01	,	 1	; Clock Select0 bit 1
 196               	.equ	CS02	,	 2	; Clock Select0 bit 2
 197               	
 198               	; TCNT0 - Timer Counter 0
 199               	.equ	TCNT00	,	 0	; Timer Counter 0 bit 0
 200               	.equ	TCNT01	,	 1	; Timer Counter 0 bit 1
 201               	.equ	TCNT02	,	 2	; Timer Counter 0 bit 2
 202               	.equ	TCNT03	,	 3	; Timer Counter 0 bit 3
 203               	.equ	TCNT04	,	 4	; Timer Counter 0 bit 4
 204               	.equ	TCNT05	,	 5	; Timer Counter 0 bit 5
 205               	.equ	TCNT06	,	 6	; Timer Counter 0 bit 6
 206               	.equ	TCNT07	,	 7	; Timer Counter 0 bit 7
 207               	
 208               	
 209               	; ***** TIMER_COUNTER_1 **************
 210               	; TIMSK - Timer/Counter Interrupt Mask Register
 211               	.equ	TOIE1	,	 2	; Timer/Counter1 Overflow Interrupt Enable
 212               	.equ	OCIE1B	,	 3	; Timer/Counter1 Output CompareB Match Interrupt Enable
 213               	.equ	OCIE1A	,	 4	; Timer/Counter1 Output CompareA Match Interrupt Enable
 214               	.equ	TICIE1	,	 5	; Timer/Counter1 Input Capture Interrupt Enable
 215               	
 216               	; TIFR - Timer/Counter Interrupt Flag register
 217               	.equ	TOV1	,	 2	; Timer/Counter1 Overflow Flag
 218               	.equ	OCF1B	,	 3	; Output Compare Flag 1B
 219               	.equ	OCF1A	,	 4	; Output Compare Flag 1A
 220               	.equ	ICF1	,	 5	; Input Capture Flag 1
 221               	
 222               	; TCCR1A - Timer/Counter1 Control Register A
 223               	.equ	WGM10	,	 0	; Waveform Generation Mode
 224               	.equ	PWM10	,	 WGM10	; For compatibility
 225               	.equ	WGM11	,	 1	; Waveform Generation Mode
GAS LISTING avr.s 			page 5


 226               	.equ	PWM11	,	 WGM11	; For compatibility
 227               	.equ	FOC1B	,	 2	; Force Output Compare 1B
 228               	.equ	FOC1A	,	 3	; Force Output Compare 1A
 229               	.equ	COM1B0	,	 4	; Compare Output Mode 1B, bit 0
 230               	.equ	COM1B1	,	 5	; Compare Output Mode 1B, bit 1
 231               	.equ	COM1A0	,	 6	; Compare Ouput Mode 1A, bit 0
 232               	.equ	COM1A1	,	 7	; Compare Output Mode 1A, bit 1
 233               	
 234               	; TCCR1B - Timer/Counter1 Control Register B
 235               	.equ	CS10	,	 0	; Prescaler source of Timer/Counter 1
 236               	.equ	CS11	,	 1	; Prescaler source of Timer/Counter 1
 237               	.equ	CS12	,	 2	; Prescaler source of Timer/Counter 1
 238               	.equ	WGM12	,	 3	; Waveform Generation Mode
 239               	.equ	CTC10	,	 WGM12	; For compatibility
 240               	.equ	CTC1	,	 WGM12	; For compatibility
 241               	.equ	WGM13	,	 4	; Waveform Generation Mode
 242               	.equ	CTC11	,	 WGM13	; For compatibility
 243               	.equ	ICES1	,	 6	; Input Capture 1 Edge Select
 244               	.equ	ICNC1	,	 7	; Input Capture 1 Noise Canceler
 245               	
 246               	
 247               	; ***** TIMER_COUNTER_2 **************
 248               	; TIMSK - Timer/Counter Interrupt Mask register
 249               	.equ	TOIE2	,	 6	; Timer/Counter2 Overflow Interrupt Enable
 250               	.equ	OCIE2	,	 7	; Timer/Counter2 Output Compare Match Interrupt Enable
 251               	
 252               	; TIFR - Timer/Counter Interrupt Flag Register
 253               	.equ	TOV2	,	 6	; Timer/Counter2 Overflow Flag
 254               	.equ	OCF2	,	 7	; Output Compare Flag 2
 255               	
 256               	; TCCR2 - Timer/Counter2 Control Register
 257               	.equ	CS20	,	 0	; Clock Select bit 0
 258               	.equ	CS21	,	 1	; Clock Select bit 1
 259               	.equ	CS22	,	 2	; Clock Select bit 2
 260               	.equ	WGM21	,	 3	; Waveform Generation Mode
 261               	.equ	CTC2	,	 WGM21	; For compatibility
 262               	.equ	COM20	,	 4	; Compare Output Mode bit 0
 263               	.equ	COM21	,	 5	; Compare Output Mode bit 1
 264               	.equ	WGM20	,	 6	; Waveform Genration Mode
 265               	.equ	PWM2	,	 WGM20	; For compatibility
 266               	.equ	FOC2	,	 7	; Force Output Compare
 267               	
 268               	; TCNT2 - Timer/Counter2
 269               	.equ	TCNT2_0	,	 0	; Timer/Counter 2 bit 0
 270               	.equ	TCNT2_1	,	 1	; Timer/Counter 2 bit 1
 271               	.equ	TCNT2_2	,	 2	; Timer/Counter 2 bit 2
 272               	.equ	TCNT2_3	,	 3	; Timer/Counter 2 bit 3
 273               	.equ	TCNT2_4	,	 4	; Timer/Counter 2 bit 4
 274               	.equ	TCNT2_5	,	 5	; Timer/Counter 2 bit 5
 275               	.equ	TCNT2_6	,	 6	; Timer/Counter 2 bit 6
 276               	.equ	TCNT2_7	,	 7	; Timer/Counter 2 bit 7
 277               	
 278               	; OCR2 - Timer/Counter2 Output Compare Register
 279               	.equ	OCR2_0	,	 0	; Timer/Counter2 Output Compare Register Bit 0
 280               	.equ	OCR2_1	,	 1	; Timer/Counter2 Output Compare Register Bit 1
 281               	.equ	OCR2_2	,	 2	; Timer/Counter2 Output Compare Register Bit 2
 282               	.equ	OCR2_3	,	 3	; Timer/Counter2 Output Compare Register Bit 3
GAS LISTING avr.s 			page 6


 283               	.equ	OCR2_4	,	 4	; Timer/Counter2 Output Compare Register Bit 4
 284               	.equ	OCR2_5	,	 5	; Timer/Counter2 Output Compare Register Bit 5
 285               	.equ	OCR2_6	,	 6	; Timer/Counter2 Output Compare Register Bit 6
 286               	.equ	OCR2_7	,	 7	; Timer/Counter2 Output Compare Register Bit 7
 287               	
 288               	; ASSR - Asynchronous Status Register
 289               	.equ	TCR2UB	,	 0	; Timer/counter Control Register2 Update Busy
 290               	.equ	OCR2UB	,	 1	; Output Compare Register2 Update Busy
 291               	.equ	TCN2UB	,	 2	; Timer/Counter2 Update Busy
 292               	.equ	AS2	,	 3	; Asynchronous Timer/counter2
 293               	
 294               	; SFIOR - Special Function IO Register
 295               	.equ	PSR2	,	 1	; Prescaler Reset Timer/Counter2
 296               	
 297               	
 298               	; ***** USART ************************
 299               	; UDR - USART I/O Data Register
 300               	.equ	UDR0	,	 0	; USART I/O Data Register bit 0
 301               	.equ	UDR1	,	 1	; USART I/O Data Register bit 1
 302               	.equ	UDR2	,	 2	; USART I/O Data Register bit 2
 303               	.equ	UDR3	,	 3	; USART I/O Data Register bit 3
 304               	.equ	UDR4	,	 4	; USART I/O Data Register bit 4
 305               	.equ	UDR5	,	 5	; USART I/O Data Register bit 5
 306               	.equ	UDR6	,	 6	; USART I/O Data Register bit 6
 307               	.equ	UDR7	,	 7	; USART I/O Data Register bit 7
 308               	
 309               	; UCSRA - USART Control and Status Register A
 310               	.equ	USR	,	 UCSRA	; For compatibility
 311               	.equ	MPCM	,	 0	; Multi-processor Communication Mode
 312               	.equ	U2X	,	 1	; Double the USART transmission speed
 313               	.equ	UPE	,	 2	; Parity Error
 314               	.equ	PE	,	 UPE	; For compatibility
 315               	.equ	DOR	,	 3	; Data overRun
 316               	.equ	FE	,	 4	; Framing Error
 317               	.equ	UDRE	,	 5	; USART Data Register Empty
 318               	.equ	TXC	,	 6	; USART Transmitt Complete
 319               	.equ	RXC	,	 7	; USART Receive Complete
 320               	
 321               	; UCSRB - USART Control and Status Register B
 322               	.equ	UCR	,	 UCSRB	; For compatibility
 323               	.equ	TXB8	,	 0	; Transmit Data Bit 8
 324               	.equ	RXB8	,	 1	; Receive Data Bit 8
 325               	.equ	UCSZ2	,	 2	; Character Size
 326               	.equ	CHR9	,	 UCSZ2	; For compatibility
 327               	.equ	TXEN	,	 3	; Transmitter Enable
 328               	.equ	RXEN	,	 4	; Receiver Enable
 329               	.equ	UDRIE	,	 5	; USART Data register Empty Interrupt Enable
 330               	.equ	TXCIE	,	 6	; TX Complete Interrupt Enable
 331               	.equ	RXCIE	,	 7	; RX Complete Interrupt Enable
 332               	
 333               	; UCSRC - USART Control and Status Register C
 334               	.equ	UCPOL	,	 0	; Clock Polarity
 335               	.equ	UCSZ0	,	 1	; Character Size
 336               	.equ	UCSZ1	,	 2	; Character Size
 337               	.equ	USBS	,	 3	; Stop Bit Select
 338               	.equ	UPM0	,	 4	; Parity Mode Bit 0
 339               	.equ	UPM1	,	 5	; Parity Mode Bit 1
GAS LISTING avr.s 			page 7


 340               	.equ	UMSEL	,	 6	; USART Mode Select
 341               	.equ	URSEL	,	 7	; Register Select
 342               	
 343               	.equ	UBRRHI	,	 UBRRH	; For compatibility
 344               	
 345               	; ***** TWI **************************
 346               	; TWBR - TWI Bit Rate register
 347               	.equ	I2BR	,	 TWBR	; For compatibility
 348               	.equ	TWBR0	,	 0	;
 349               	.equ	TWBR1	,	 1	;
 350               	.equ	TWBR2	,	 2	;
 351               	.equ	TWBR3	,	 3	;
 352               	.equ	TWBR4	,	 4	;
 353               	.equ	TWBR5	,	 5	;
 354               	.equ	TWBR6	,	 6	;
 355               	.equ	TWBR7	,	 7	;
 356               	
 357               	; TWCR - TWI Control Register
 358               	.equ	I2CR	,	 TWCR	; For compatibility
 359               	.equ	TWIE	,	 0	; TWI Interrupt Enable
 360               	.equ	I2IE	,	 TWIE	; For compatibility
 361               	.equ	TWEN	,	 2	; TWI Enable Bit
 362               	.equ	I2EN	,	 TWEN	; For compatibility
 363               	.equ	ENI2C	,	 TWEN	; For compatibility
 364               	.equ	TWWC	,	 3	; TWI Write Collition Flag
 365               	.equ	I2WC	,	 TWWC	; For compatibility
 366               	.equ	TWSTO	,	 4	; TWI Stop Condition Bit
 367               	.equ	I2STO	,	 TWSTO	; For compatibility
 368               	.equ	TWSTA	,	 5	; TWI Start Condition Bit
 369               	.equ	I2STA	,	 TWSTA	; For compatibility
 370               	.equ	TWEA	,	 6	; TWI Enable Acknowledge Bit
 371               	.equ	I2EA	,	 TWEA	; For compatibility
 372               	.equ	TWINT	,	 7	; TWI Interrupt Flag
 373               	.equ	I2INT	,	 TWINT	; For compatibility
 374               	
 375               	; TWSR - TWI Status Register
 376               	.equ	I2SR	,	 TWSR	; For compatibility
 377               	.equ	TWPS0	,	 0	; TWI Prescaler
 378               	.equ	TWS0	,	 TWPS0	; For compatibility
 379               	.equ	I2GCE	,	 TWPS0	; For compatibility
 380               	.equ	TWPS1	,	 1	; TWI Prescaler
 381               	.equ	TWS1	,	 TWPS1	; For compatibility
 382               	.equ	TWS3	,	 3	; TWI Status
 383               	.equ	I2S3	,	 TWS3	; For compatibility
 384               	.equ	TWS4	,	 4	; TWI Status
 385               	.equ	I2S4	,	 TWS4	; For compatibility
 386               	.equ	TWS5	,	 5	; TWI Status
 387               	.equ	I2S5	,	 TWS5	; For compatibility
 388               	.equ	TWS6	,	 6	; TWI Status
 389               	.equ	I2S6	,	 TWS6	; For compatibility
 390               	.equ	TWS7	,	 7	; TWI Status
 391               	.equ	I2S7	,	 TWS7	; For compatibility
 392               	
 393               	; TWDR - TWI Data register
 394               	.equ	I2DR	,	 TWDR	; For compatibility
 395               	.equ	TWD0	,	 0	; TWI Data Register Bit 0
 396               	.equ	TWD1	,	 1	; TWI Data Register Bit 1
GAS LISTING avr.s 			page 8


 397               	.equ	TWD2	,	 2	; TWI Data Register Bit 2
 398               	.equ	TWD3	,	 3	; TWI Data Register Bit 3
 399               	.equ	TWD4	,	 4	; TWI Data Register Bit 4
 400               	.equ	TWD5	,	 5	; TWI Data Register Bit 5
 401               	.equ	TWD6	,	 6	; TWI Data Register Bit 6
 402               	.equ	TWD7	,	 7	; TWI Data Register Bit 7
 403               	
 404               	; TWAR - TWI (Slave) Address register
 405               	.equ	I2AR	,	 TWAR	; For compatibility
 406               	.equ	TWGCE	,	 0	; TWI General Call Recognition Enable Bit
 407               	.equ	TWA0	,	 1	; TWI (Slave) Address register Bit 0
 408               	.equ	TWA1	,	 2	; TWI (Slave) Address register Bit 1
 409               	.equ	TWA2	,	 3	; TWI (Slave) Address register Bit 2
 410               	.equ	TWA3	,	 4	; TWI (Slave) Address register Bit 3
 411               	.equ	TWA4	,	 5	; TWI (Slave) Address register Bit 4
 412               	.equ	TWA5	,	 6	; TWI (Slave) Address register Bit 5
 413               	.equ	TWA6	,	 7	; TWI (Slave) Address register Bit 6
 414               	
 415               	
 416               	; ***** WATCHDOG *********************
 417               	; WDTCR - Watchdog Timer Control Register
 418               	.equ	WDTCSR	,	 WDTCR	; For compatibility
 419               	.equ	WDP0	,	 0	; Watch Dog Timer Prescaler bit 0
 420               	.equ	WDP1	,	 1	; Watch Dog Timer Prescaler bit 1
 421               	.equ	WDP2	,	 2	; Watch Dog Timer Prescaler bit 2
 422               	.equ	WDE	,	 3	; Watch Dog Enable
 423               	.equ	WDCE	,	 4	; Watchdog Change Enable
 424               	.equ	WDTOE	,	 WDCE	; For compatibility
 425               	
 426               	
 427               	; ***** PORTB ************************
 428               	; PORTB - Port B Data Register
 429               	.equ	PORTB0	,	 0	; Port B Data Register bit 0
 430               	.equ	PB0	,	 0	; For compatibility
 431               	.equ	PORTB1	,	 1	; Port B Data Register bit 1
 432               	.equ	PB1	,	 1	; For compatibility
 433               	.equ	PORTB2	,	 2	; Port B Data Register bit 2
 434               	.equ	PB2	,	 2	; For compatibility
 435               	.equ	PORTB3	,	 3	; Port B Data Register bit 3
 436               	.equ	PB3	,	 3	; For compatibility
 437               	.equ	PORTB4	,	 4	; Port B Data Register bit 4
 438               	.equ	PB4	,	 4	; For compatibility
 439               	.equ	PORTB5	,	 5	; Port B Data Register bit 5
 440               	.equ	PB5	,	 5	; For compatibility
 441               	.equ	PORTB6	,	 6	; Port B Data Register bit 6
 442               	.equ	PB6	,	 6	; For compatibility
 443               	.equ	PORTB7	,	 7	; Port B Data Register bit 7
 444               	.equ	PB7	,	 7	; For compatibility
 445               	
 446               	; DDRB - Port B Data Direction Register
 447               	.equ	DDB0	,	 0	; Port B Data Direction Register bit 0
 448               	.equ	DDB1	,	 1	; Port B Data Direction Register bit 1
 449               	.equ	DDB2	,	 2	; Port B Data Direction Register bit 2
 450               	.equ	DDB3	,	 3	; Port B Data Direction Register bit 3
 451               	.equ	DDB4	,	 4	; Port B Data Direction Register bit 4
 452               	.equ	DDB5	,	 5	; Port B Data Direction Register bit 5
 453               	.equ	DDB6	,	 6	; Port B Data Direction Register bit 6
GAS LISTING avr.s 			page 9


 454               	.equ	DDB7	,	 7	; Port B Data Direction Register bit 7
 455               	
 456               	; PINB - Port B Input Pins
 457               	.equ	PINB0	,	 0	; Port B Input Pins bit 0
 458               	.equ	PINB1	,	 1	; Port B Input Pins bit 1
 459               	.equ	PINB2	,	 2	; Port B Input Pins bit 2
 460               	.equ	PINB3	,	 3	; Port B Input Pins bit 3
 461               	.equ	PINB4	,	 4	; Port B Input Pins bit 4
 462               	.equ	PINB5	,	 5	; Port B Input Pins bit 5
 463               	.equ	PINB6	,	 6	; Port B Input Pins bit 6
 464               	.equ	PINB7	,	 7	; Port B Input Pins bit 7
 465               	
 466               	
 467               	; ***** PORTC ************************
 468               	; PORTC - Port C Data Register
 469               	.equ	PORTC0	,	 0	; Port C Data Register bit 0
 470               	.equ	PC0	,	 0	; For compatibility
 471               	.equ	PORTC1	,	 1	; Port C Data Register bit 1
 472               	.equ	PC1	,	 1	; For compatibility
 473               	.equ	PORTC2	,	 2	; Port C Data Register bit 2
 474               	.equ	PC2	,	 2	; For compatibility
 475               	.equ	PORTC3	,	 3	; Port C Data Register bit 3
 476               	.equ	PC3	,	 3	; For compatibility
 477               	.equ	PORTC4	,	 4	; Port C Data Register bit 4
 478               	.equ	PC4	,	 4	; For compatibility
 479               	.equ	PORTC5	,	 5	; Port C Data Register bit 5
 480               	.equ	PC5	,	 5	; For compatibility
 481               	.equ	PORTC6	,	 6	; Port C Data Register bit 6
 482               	.equ	PC6	,	 6	; For compatibility
 483               	
 484               	; DDRC - Port C Data Direction Register
 485               	.equ	DDC0	,	 0	; Port C Data Direction Register bit 0
 486               	.equ	DDC1	,	 1	; Port C Data Direction Register bit 1
 487               	.equ	DDC2	,	 2	; Port C Data Direction Register bit 2
 488               	.equ	DDC3	,	 3	; Port C Data Direction Register bit 3
 489               	.equ	DDC4	,	 4	; Port C Data Direction Register bit 4
 490               	.equ	DDC5	,	 5	; Port C Data Direction Register bit 5
 491               	.equ	DDC6	,	 6	; Port C Data Direction Register bit 6
 492               	
 493               	; PINC - Port C Input Pins
 494               	.equ	PINC0	,	 0	; Port C Input Pins bit 0
 495               	.equ	PINC1	,	 1	; Port C Input Pins bit 1
 496               	.equ	PINC2	,	 2	; Port C Input Pins bit 2
 497               	.equ	PINC3	,	 3	; Port C Input Pins bit 3
 498               	.equ	PINC4	,	 4	; Port C Input Pins bit 4
 499               	.equ	PINC5	,	 5	; Port C Input Pins bit 5
 500               	.equ	PINC6	,	 6	; Port C Input Pins bit 6
 501               	
 502               	
 503               	; ***** PORTD ************************
 504               	; PORTD - Port D Data Register
 505               	.equ	PORTD0	,	 0	; Port D Data Register bit 0
 506               	.equ	PD0	,	 0	; For compatibility
 507               	.equ	PORTD1	,	 1	; Port D Data Register bit 1
 508               	.equ	PD1	,	 1	; For compatibility
 509               	.equ	PORTD2	,	 2	; Port D Data Register bit 2
 510               	.equ	PD2	,	 2	; For compatibility
GAS LISTING avr.s 			page 10


 511               	.equ	PORTD3	,	 3	; Port D Data Register bit 3
 512               	.equ	PD3	,	 3	; For compatibility
 513               	.equ	PORTD4	,	 4	; Port D Data Register bit 4
 514               	.equ	PD4	,	 4	; For compatibility
 515               	.equ	PORTD5	,	 5	; Port D Data Register bit 5
 516               	.equ	PD5	,	 5	; For compatibility
 517               	.equ	PORTD6	,	 6	; Port D Data Register bit 6
 518               	.equ	PD6	,	 6	; For compatibility
 519               	.equ	PORTD7	,	 7	; Port D Data Register bit 7
 520               	.equ	PD7	,	 7	; For compatibility
 521               	
 522               	; DDRD - Port D Data Direction Register
 523               	.equ	DDD0	,	 0	; Port D Data Direction Register bit 0
 524               	.equ	DDD1	,	 1	; Port D Data Direction Register bit 1
 525               	.equ	DDD2	,	 2	; Port D Data Direction Register bit 2
 526               	.equ	DDD3	,	 3	; Port D Data Direction Register bit 3
 527               	.equ	DDD4	,	 4	; Port D Data Direction Register bit 4
 528               	.equ	DDD5	,	 5	; Port D Data Direction Register bit 5
 529               	.equ	DDD6	,	 6	; Port D Data Direction Register bit 6
 530               	.equ	DDD7	,	 7	; Port D Data Direction Register bit 7
 531               	
 532               	; PIND - Port D Input Pins
 533               	.equ	PIND0	,	 0	; Port D Input Pins bit 0
 534               	.equ	PIND1	,	 1	; Port D Input Pins bit 1
 535               	.equ	PIND2	,	 2	; Port D Input Pins bit 2
 536               	.equ	PIND3	,	 3	; Port D Input Pins bit 3
 537               	.equ	PIND4	,	 4	; Port D Input Pins bit 4
 538               	.equ	PIND5	,	 5	; Port D Input Pins bit 5
 539               	.equ	PIND6	,	 6	; Port D Input Pins bit 6
 540               	.equ	PIND7	,	 7	; Port D Input Pins bit 7
 541               	
 542               	
 543               	; ***** EEPROM ***********************
 544               	; EEDR - EEPROM Data Register
 545               	.equ	EEDR0	,	 0	; EEPROM Data Register bit 0
 546               	.equ	EEDR1	,	 1	; EEPROM Data Register bit 1
 547               	.equ	EEDR2	,	 2	; EEPROM Data Register bit 2
 548               	.equ	EEDR3	,	 3	; EEPROM Data Register bit 3
 549               	.equ	EEDR4	,	 4	; EEPROM Data Register bit 4
 550               	.equ	EEDR5	,	 5	; EEPROM Data Register bit 5
 551               	.equ	EEDR6	,	 6	; EEPROM Data Register bit 6
 552               	.equ	EEDR7	,	 7	; EEPROM Data Register bit 7
 553               	
 554               	; EECR - EEPROM Control Register
 555               	.equ	EERE	,	 0	; EEPROM Read Enable
 556               	.equ	EEWE	,	 1	; EEPROM Write Enable
 557               	.equ	EEMWE	,	 2	; EEPROM Master Write Enable
 558               	.equ	EEWEE	,	 EEMWE	; For compatibility
 559               	.equ	EERIE	,	 3	; EEPROM Ready Interrupt Enable
 560               	
 561               	
 562               	; ***** CPU **************************
 563               	; SREG - Status Register
 564               	.equ	SREG_C	,	 0	; Carry Flag
 565               	.equ	SREG_Z	,	 1	; Zero Flag
 566               	.equ	SREG_N	,	 2	; Negative Flag
 567               	.equ	SREG_V	,	 3	; Two's Complement Overflow Flag
GAS LISTING avr.s 			page 11


 568               	.equ	SREG_S	,	 4	; Sign Bit
 569               	.equ	SREG_H	,	 5	; Half Carry Flag
 570               	.equ	SREG_T	,	 6	; Bit Copy Storage
 571               	.equ	SREG_I	,	 7	; Global Interrupt Enable
 572               	
 573               	; MCUCR - MCU Control Register
 574               	;.equ	ISC00	,	 0	; Interrupt Sense Control 0 Bit 0
 575               	;.equ	ISC01	,	 1	; Interrupt Sense Control 0 Bit 1
 576               	;.equ	ISC10	,	 2	; Interrupt Sense Control 1 Bit 0
 577               	;.equ	ISC11	,	 3	; Interrupt Sense Control 1 Bit 1
 578               	.equ	SM0	,	 4	; Sleep Mode Select
 579               	.equ	SM1	,	 5	; Sleep Mode Select
 580               	.equ	SM2	,	 6	; Sleep Mode Select
 581               	.equ	SE	,	 7	; Sleep Enable
 582               	
 583               	; MCUCSR - MCU Control And Status Register
 584               	.equ	MCUSR	,	 MCUCSR	; For compatibility
 585               	.equ	PORF	,	 0	; Power-on reset flag
 586               	.equ	EXTRF	,	 1	; External Reset Flag
 587               	.equ	BORF	,	 2	; Brown-out Reset Flag
 588               	.equ	WDRF	,	 3	; Watchdog Reset Flag
 589               	
 590               	; OSCCAL - Oscillator Calibration Value
 591               	.equ	CAL0	,	 0	; Oscillator Calibration Value Bit0
 592               	.equ	CAL1	,	 1	; Oscillator Calibration Value Bit1
 593               	.equ	CAL2	,	 2	; Oscillator Calibration Value Bit2
 594               	.equ	CAL3	,	 3	; Oscillator Calibration Value Bit3
 595               	.equ	CAL4	,	 4	; Oscillator Calibration Value Bit4
 596               	.equ	CAL5	,	 5	; Oscillator Calibration Value Bit5
 597               	.equ	CAL6	,	 6	; Oscillator Calibration Value Bit6
 598               	.equ	CAL7	,	 7	; Oscillator Calibration Value Bit7
 599               	
 600               	; SPMCR - Store Program Memory Control Register
 601               	.equ	SPMEN	,	 0	; Store Program Memory Enable
 602               	.equ	PGERS	,	 1	; Page Erase
 603               	.equ	PGWRT	,	 2	; Page Write
 604               	.equ	BLBSET	,	 3	; Boot Lock Bit Set
 605               	.equ	RWWSRE	,	 4	; Read-While-Write Section Read Enable
 606               	.equ	RWWSB	,	 6	; Read-While-Write Section Busy
 607               	.equ	SPMIE	,	 7	; SPM Interrupt Enable
 608               	
 609               	; SFIOR - Special Function IO Register
 610               	.equ	PSR10	,	 0	; Prescaler Reset Timer/Counter1 and Timer/Counter0
 611               	.equ	PUD	,	 2	; Pull-up Disable
 612               	.equ	ADHSM	,	 4	; ADC High Speed Mode
 613               	
 614               	
 615               	; ***** AD_CONVERTER *****************
 616               	; ADMUX - The ADC multiplexer Selection Register
 617               	.equ	MUX0	,	 0	; Analog Channel and Gain Selection Bits
 618               	.equ	MUX1	,	 1	; Analog Channel and Gain Selection Bits
 619               	.equ	MUX2	,	 2	; Analog Channel and Gain Selection Bits
 620               	.equ	MUX3	,	 3	; Analog Channel and Gain Selection Bits
 621               	.equ	ADLAR	,	 5	; Left Adjust Result
 622               	.equ	REFS0	,	 6	; Reference Selection Bit 0
 623               	.equ	REFS1	,	 7	; Reference Selection Bit 1
 624               	
GAS LISTING avr.s 			page 12


 625               	; ADCSRA - The ADC Control and Status register
 626               	.equ	ADCSR	,	 ADCSRA	; For compatibility
 627               	.equ	ADPS0	,	 0	; ADC  Prescaler Select Bits
 628               	.equ	ADPS1	,	 1	; ADC  Prescaler Select Bits
 629               	.equ	ADPS2	,	 2	; ADC  Prescaler Select Bits
 630               	.equ	ADIE	,	 3	; ADC Interrupt Enable
 631               	.equ	ADIF	,	 4	; ADC Interrupt Flag
 632               	.equ	ADFR	,	 5	; ADC  Free Running Select
 633               	.equ	ADSC	,	 6	; ADC Start Conversion
 634               	.equ	ADEN	,	 7	; ADC Enable
 635               	
 636               	
 637               	
 638               	; ***** LOCKSBITS ********************************************************
 639               	.equ	LB1	,	 0	; Lock bit
 640               	.equ	LB2	,	 1	; Lock bit
 641               	.equ	BLB01	,	 2	; Boot Lock bit
 642               	.equ	BLB02	,	 3	; Boot Lock bit
 643               	.equ	BLB11	,	 4	; Boot lock bit
 644               	.equ	BLB12	,	 5	; Boot lock bit
 645               	
 646               	
 647               	; ***** FUSES ************************************************************
 648               	; LOW fuse bits
 649               	.equ	CKSEL0	,	 0	; Select Clock Source
 650               	.equ	CKSEL1	,	 1	; Select Clock Source
 651               	.equ	CKSEL2	,	 2	; Select Clock Source
 652               	.equ	CKSEL3	,	 3	; Select Clock Source
 653               	.equ	SUT0	,	 4	; Select start-up time
 654               	.equ	SUT1	,	 5	; Select start-up time
 655               	.equ	BODEN	,	 6	; Brown out detector enable
 656               	.equ	BODLEVEL	,	 7	; Brown out detector trigger level
 657               	
 658               	; HIGH fuse bits
 659               	.equ	BOOTRST	,	 0	; Select Reset Vector
 660               	.equ	BOOTSZ0	,	 1	; Select Boot Size
 661               	.equ	BOOTSZ1	,	 2	; Select Boot Size
 662               	.equ	EESAVE	,	 3	; EEPROM memory is preserved through chip erase
 663               	.equ	CKOPT	,	 4	; Oscillator Options
 664               	.equ	SPIEN	,	 5	; Enable Serial programming and Data Downloading
 665               	.equ	WTDON	,	 6	; Enable watchdog
 666               	.equ	RSTDISBL	,	 7	; Disable reset
 667               	
 668               	
 669               	
 670               	; ***** CPU REGISTER DEFINITIONS *****************************************
 671               	;.def	XH	= r27
 672               	;.def	XL	= r26
 673               	;.def	YH	= r29
 674               	;.def	YL	= r28
 675               	;.def	ZH	= r31
 676               	;.def	ZL	= r30
 677               	.equ	XH,	0x1B
 678               	.equ	XL,	0x1A
 679               	.equ	YH,	0x1D
 680               	.equ	YL,	0x1C
 681               	.equ	ZH,	0x1F
GAS LISTING avr.s 			page 13


 682               	.equ	ZL,	0x1E
 683               	
 684               	
 685               	
 686               	; ***** DATA MEMORY DECLARATIONS *****************************************
 687               	.equ	FLASHEND	,	 0x0fff	; Note: Word address
 688               	.equ	IOEND	,	 0x003f
 689               	.equ	SRAM_START	,	 0x0060
 690               	.equ	SRAM_SIZE	,	 1024
 691               	.equ	RAMEND	,	 0x045f
 692               	.equ	XRAMEND	,	 0x0000
 693               	.equ	E2END	,	 0x01ff
 694               	.equ	EEPROMEND	,	 0x01ff
 695               	.equ	EEADRBITS	,	 9
 696               	#pragma AVRPART MEMORY PROG_FLASH 8192
 697               	#pragma AVRPART MEMORY EEPROM 512
 698               	#pragma AVRPART MEMORY INT_SRAM SIZE 1024
 699               	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x60
 700               	
 701               	
 702               	
 703               	; ***** BOOTLOADER DECLARATIONS ******************************************
 704               	.equ	NRWW_START_ADDR	,	 0xc00
 705               	.equ	NRWW_STOP_ADDR	,	 0xfff
 706               	.equ	RWW_START_ADDR	,	 0x0
 707               	.equ	RWW_STOP_ADDR	,	 0xbff
 708               	.equ	PAGESIZE	,	 32
 709               	.equ	FIRSTBOOTSTART	,	 0xf80
 710               	.equ	SECONDBOOTSTART	,	 0xf00
 711               	.equ	THIRDBOOTSTART	,	 0xe00
 712               	.equ	FOURTHBOOTSTART	,	 0xc00
 713               	.equ	SMALLBOOTSTART	,	 FIRSTBOOTSTART
 714               	.equ	LARGEBOOTSTART	,	 FOURTHBOOTSTART
 715               	
 716               	
 717               	
 718               	; ***** INTERRUPT VECTORS ************************************************
 719               	.equ	INT0addr	,	 0x0001	; External Interrupt Request 0
 720               	.equ	INT1addr	,	 0x0002	; External Interrupt Request 1
 721               	.equ	OC2addr	,	 0x0003	; Timer/Counter2 Compare Match
 722               	.equ	OVF2addr	,	 0x0004	; Timer/Counter2 Overflow
 723               	.equ	ICP1addr	,	 0x0005	; Timer/Counter1 Capture Event
 724               	.equ	OC1Aaddr	,	 0x0006	; Timer/Counter1 Compare Match A
 725               	.equ	OC1Baddr	,	 0x0007	; Timer/Counter1 Compare Match B
 726               	.equ	OVF1addr	,	 0x0008	; Timer/Counter1 Overflow
 727               	.equ	OVF0addr	,	 0x0009	; Timer/Counter0 Overflow
 728               	.equ	SPIaddr	,	 0x000a	; Serial Transfer Complete
 729               	.equ	URXCaddr	,	 0x000b	; USART, Rx Complete
 730               	.equ	UDREaddr	,	 0x000c	; USART Data Register Empty
 731               	.equ	UTXCaddr	,	 0x000d	; USART, Tx Complete
 732               	.equ	ADCCaddr	,	 0x000e	; ADC Conversion Complete
 733               	.equ	ERDYaddr	,	 0x000f	; EEPROM Ready
 734               	.equ	ACIaddr	,	 0x0010	; Analog Comparator
 735               	.equ	TWIaddr	,	 0x0011	; 2-wire Serial Interface
 736               	.equ	SPMRaddr	,	 0x0012	; Store Program Memory Ready
 737               	
 738               	.equ	INT_VECTORS_SIZE	,	 19	; size in words
GAS LISTING avr.s 			page 14


 739               	
 740               	#pragma AVRPART CORE INSTRUCTIONS_NOT_SUPPORTED break
 741               	
 742               	#endif  /* _M8ADEF_INC_ */
 743               	
 744               	; ***** END OF FILE ******************************************************
   4               	; Символы ----------------------------------------------------------------------------------
   5               	
   6               	.equ					XTAL,		8000000
   7               	.equ					baudrate,	9600
   8               	.equ					bauddivider,	XTAL / (16 * baudrate) - 1
   9               	.equ					F_JOB,		0			; Флаг занятости процессора, указывает мож�
  10               											; данных сразу или запланировать до момента когд�
  11               	.equ					F_RFID,		1			; Флаг о сигнализации наличия работы связа
  12               	.equ					F_ETH,		2			; Флаг о сигнализации наличия работы связа�
  13               	.equ					F_RX,		3			; Флаг о сигнализации наличия работы связан
  14               	;.equ					F_TX,		4			; Похоже этот флаг не нужен; Флаг о сигнализ
  15               	
  16               	.equ					rx,		0			; Целевой буфер, используется для макросов р�
  17               	.equ					tx,		1
  18               	.equ					read,		0			; Операция над буфером, используется для ма�
  19               	.equ					write,		1
  20               	
  21               	.equ					FLAGS,		0x14			; r20 использован для флагов (программного 
  22               											; флаги используются очень часто (каждую итераци�
  23               	                                                                                ; избежать 
  24               											; в регистре
  25               	
  26               	; Макросы ----------------------------------------------------------------------------------
  27               	
  28               	.macro	pushf									; сохранить флаговый регистр на стек
  29               						push	r16
  30               						in	r16,	SREG
  31               						push	r16
  32               	.endm
  33               	
  34               	.macro	outi	r	v							; вывести в порт константу
  35               						ldi	R16,	\v
  36               				.if \r < 0x40
  37               						out	\r,	R16
  38               				.else
  39               						sts	\r,	R16
  40               				.endif
  41               	.endm
  42               	
  43               	.macro	popf									; восстановить флаги со стека
  44               						pop	r16
  45               						out	SREG,	r16
  46               						pop	r16
  47               	.endm
  48               	
  49               	.macro	invb	r	b							; инвертирует заданный бит регистра
  50               						ldi	r16,	(1 << \b)
  51               				.if \r < 0x40
  52               						in	r17,	\r
  53               						eor	r17,	r16
  54               						out	\r,	r17
GAS LISTING avr.s 			page 15


  55               				.else
  56               						lds	r17,	\r
  57               						eor	r17,	r16
  58               						sts	\r,	r17
  59               				.endif
  60               	.endm
  61               	
  62               	.macro	incm	ctn								; инкремент 4х байтного числа в памяти
  63               						lds	r16,		\ctn
  64               						lds	r17,		\ctn + 1
  65               						;lds	r18,		\ctn + 2
  66               						;lds	r19,		\ctn + 3
  67               	
  68               						subi	r16,		(-1)		; -1 = 0b11111111, поэтому при вычитании из любого
  69               						sbci	r17,		(-1)		; всегда будет перенос бита и флаг C в SREG, ко
  70               						;sbci	r18,		(-1)		; with carry - вычитает константу из буфера и би
  71               						;sbci	r19,		(-1)		; всегда перенос = 1 то r17 - 1 + 1 = r17
  72               	
  73               						sts	\ctn,		r16
  74               						sts	\ctn + 1,	r17
  75               						;sts	\ctn + 2,	r18
  76               						;sts	\ctn + 3,	r19
  77               	.endm
  78               	
  79               	.macro	setbs	p	b
  80               						lds	r16,	\p
  81               						sbr	r16,	1 << \b
  82               						sts	\p,	r16
  83               	.endm
  84               	
  85               	.macro	setbi	r	b	tr=r16						; установить бит в регистре I/O использу
  86               				.if	\r <= 0x20
  87               						sbi	\r,	\b
  88               				.else
  89               					.if	\r <= 0x40				; 0x40 = 64 адреса, т.к. опкод содержит 5 бит для 
  90               											; обрабатывается опкодами общей адресации памят�
  91               						in	\tr,	\r
  92               						sbr	\tr,	1 << \b
  93               						out	\r,	\tr
  94               					.else						; здесь расширенные регистры ввода / вывода ко�
  95               						lds	\tr,	\r			; in / out, поэтому вместо размера 2 байта эти по�
  96               						sbr	\tr,	1 << \b			; байта из стандартной адресации RAM
  97               						sts	\r,	\tr
  98               					.endif
  99               				.endif
 100               	.endm
 101               	
 102               	.macro	schd_job	j							; Описание работы планировщика задач -------
 103               						cpi	FLAGS,		1 << F_JOB	;
 104               						brne	do_job_\@			;   Планирует работу, если процессор свобод
 105               						; Запланировать работу			; типа работы и устанавлива�
 106               						ori	FLAGS,		1 << \j		;
 107               						rjmp	schd_exit_\@ 			;   В случае если процессор занят то уста�
 108               						; Устанавливаем флаг работы		; делать) и выходит из п�
 109               	do_job_\@:				ori	FLAGS,		1 << F_JOB	; текущую работу он проверит появи
 110               						ori	FLAGS,		1 << \j		; согласно приоритету.
 111               	schd_exit_\@:									;   После того как все работы будут заверше�
GAS LISTING avr.s 			page 16


 112               											; процессор засыпает до тех пор пока прерывания н
 113               											; работ.
 114               											; ----------------------------------------------------------------------------------
 115               	.endm
 116               	
 117               	.macro	upd_buffp	bp	bstart	bend	op				; Инкрементированный адрес указа�
 118               						push	r16				; не конец обновляем указатель иначе перенос�
 119               	
 120               						ldi	r16,	lo8(bend)
 121               						cp	ZL,	r16
 122               						brne	1f
 123               						ldi	r16,	hi8(bend)
 124               						cp	ZH,	r16
 125               						brne	1f
 126               	
 127               						; Если указатель вышел за рамки буфера то вернем его
 128               						ldi	ZL,	lo8(bstart)
 129               						ldi	ZH,	hi8(bstart)
 130               	
 131               	1:					pop	r16
 132               	
 133               	.endm
 134               	
 135               	.macro	buff_in	READ_BUFF	r							; Считываем данные в кольцевые (поль�
 136               				.equ WRITE_BUFF,	0
 137               				.equ BUFF_START,	0
 138               				.equ BUFF_END,		0
 139               	
 140               				; Макрос реализован в общей манере, чтобы его можно и�
 141               				.if \READ_BUFF == RX_BUFF_IN
 142               					.set WRITE_BUFF,	RX_BUFF_OUT
 143               					.set BUFF_START,	RX_BUFF
 144               					.set BUFF_END,		RX_BUFF_END
 145               				.endif
 146               	
 147               						lds	ZL,	\READ_BUFF		; С помощью указателя чтения из буфера оп
 148               						lds	ZH,	\READ_BUFF + 1		; равен указателю OUT значит данные имею
 149               											; указатель на 1 (проверим что если следующий адре
 150               						lds	\r,	WRITE_BUFF		; в начало.
 151               						cp	ZL,	\r
 152               						brne	1f				; Если данных нету (IN == OUT) то снимем флаг соотв�
 153               	
 154               						lds	\r,	WRITE_BUFF + 1
 155               						cp	ZH,	\r
 156               						brne	1f
 157               	
 158               						; IN == OUT, снимаем флаг, завершаем работу
 159               						cbr	FLAGS,	1 << F_RX
 160               						rjmp	2f
 161               	
 162               						; Грузим данные в регистр из кольцевого буфера
 163               	1:					lds	\r,	Z+
 164               	
 165               						; Инкрементируем указатель чтения буфера с проверк�
 166               						upd_buffp	READ_BUFF,	BUFF_START,	BUFF_END
 167               	
 168               						; Сохраняем обновленный указатель буфера в память
GAS LISTING avr.s 			page 17


 169               						sts	READ_BUFF,	ZL
 170               						sts	READ_BUFF + 1,	ZH
 171               	2:
 172               	
 173               	.endm
 174               	
 175               	.macro	clr_tcnt	tcnt
 176               						ldi	r24,	lo8(\tcnt)
 177               						ldi	r25,	hi8(\tcnt)
 178               	
 179               						rcall	clear_tcn
 180               	.endm
 181               	
 182               	.macro	buff_out	WRITE_BUFF	r					; Записываем данные в кольцевые (пол�
 183               				.equ READ_BUFF,		0
 184               				.equ BUFF_START,	0
 185               				.equ BUFF_END,		0
 186               	
 187               				; Макрос реализован в общей манере, чтобы его можно и�
 188               				.if \WRITE_BUFF == TX_BUFF_OUT
 189               					.set READ_BUFF,		TX_BUFF_IN
 190               					.set BUFF_START,	TX_BUFF
 191               					.set BUFF_END,		TX_BUFF_END
 192               				.endif
 193               	
 194               						lds	ZL,	\WRITE_BUFF		; С помощью указателя чтения из буфера о�
 195               						lds	ZH,	\WRITE_BUFF + 1		; равен указателю OUT значит данные име�
 196               						sts	Z+,	\r			; указатель на 1 (проверим что если следующий 
 197               						; Изменим указатель на следующий адрес (учитывая ко�
 198               						upd_buffp	WRITE_BUFF,	BUFF_START,	BUFF_END
 199               	
 200               						; Проверим не равен ли новый адрес указателю чтения,
 201               						; их повредить, также это означает что записываемый 
 202               						lds	\r,	READ_BUFF
 203               						cp	ZL,	\r
 204               						brne	1f				; Если OUT != IN то записываем инкрементированный
 205               	
 206               						lds	\r,	READ_BUFF + 1
 207               						cp	ZH,	\r
 208               						brne	1f
 209               	
 210               						; Иначе завершаем работу.
 211               						rjmp	2f
 212               	
 213               						; Обновляем указатель на запись
 214               	1:					sts	WRITE_BUFF,	ZL
 215               						sts	WRITE_BUFF + 1,	ZH
 216               	2:
 217               	
 218               	.endm
 219               	
 220               	.data ; Сегмент оперативной памяти
 221               	; Данные оперативной памяти ------------------------------------------------
 222               	; Используется для счетчика событий переполнения тайм
 223 0000 0000 0000 	.org	SRAM_START
 223      0000 0000 
 223      0000 0000 
GAS LISTING avr.s 			page 18


 223      0000 0000 
 223      0000 0000 
 224               	
 225 0060 0000      	TCNT:					.space	2,	0x00
 226 0062 0000      	CTCNT2:					.space	2,	0x00
 227 0064 0000      	CTCNT3:					.space	2,	0x00
 228 0066 0000      	CTCNT4:					.space	2,	0x00
 229 0068 0000      	CTCNT5:					.space	2,	0x00
 230               	
 231               	;RX_BUFF_IN:				.word	0x00			; Кольцевые буферы на прием и передачу 
 232               	;RX_BUFF_OUT:				.word	0x00			; двух указателей для записи и чтения �
 233               	;;//RX_BUFF:				.space	48,	0x00
 234               	;RX_BUFF_END:
 235               	;TX_BUFF_IN:				.word	0x00
 236               	;TX_BUFF_OUT:				.word	0x00
 237               	;TX_BUFF:				.space	48,	0x00
 238               	;TX_BUFF_END:
 239               	
 240               	.global CTCNT2
 241               	.global CTCNT3
 242               	.global CTCNT4
 243               	.global CTCNT5
 244               	.global DOOR_IS_OPENED
 245               	.global OPEN_DOOR
 246               	.global OPEN_SIGNAL
 247               	.global DENIED_SIGNAL
 248               	.global NET_ACTIVITY
 249               	.global RFID_ACTIVITY
 250               	.global SIGNAL_DELTA_THRESHOLD
 251               	
 252 006a 0100      	DOOR_IS_OPENED:				.byte	1,	0x00
 253 006c 0100      	OPEN_DOOR:				.byte	1,	0x00
 254 006e 0100      	OPEN_SIGNAL:				.byte	1,	0x00
 255 0070 0100      	DENIED_SIGNAL:				.byte	1,	0x00
 256 0072 0100      	NET_ACTIVITY:				.byte	1,	0x00
 257 0074 0100      	RFID_ACTIVITY:				.byte	1,	0x00
 258 0076 0100      	SIGNAL_DELTA:				.byte	1,	0x00
 259               	
 260               	
 261               	.text ; 0x00 Адрес начала работы контроллера --------------------------
 262               	; Таблица векторов прерываний --------------------------------------------
 263               	.org	0x00
 264               	;reset:					rjmp	asm_main		;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdo
 265               	;					reti				;2 0x001 INT0 External Interrupt Request 0
 266               	;					reti				;3 0x002 INT1 External Interrupt Request 1
 267               	;					reti				;4 0x003 PCINT0 Pin Change Interrupt Request 0
 268               	;					reti				;5 0x004 PCINT1 Pin Change Interrupt Request 1
 269               	;					reti				;6 0x005 PCINT2 Pin Change Interrupt Request 2
 270               	;					reti				;7 0x006 WDT Watchdog Time-out Interrupt
 271               	;					reti				;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
 272               	;					reti				;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
 273               	;					reti				;10 0x009 TIMER2 OVF Timer/Counter2 Overflow
 274               	;					reti				;11 0x00A TIMER1 CAPT Timer/Counter1 Capture Event
 275               	;					reti				;12 0x00B TIMER1 COMPA Timer/Counter1 Compare Match A
 276               	;					reti				;13 0x00C TIMER1 COMPB Timer/Counter1 Compare Match B
 277               	;					reti				;14 0x00D TIMER1 OVF Timer/Counter1 Overflow
 278               	;					reti				;15 0x00E TIMER0 COMPA Timer/Counter0 Compare Match A
GAS LISTING avr.s 			page 19


 279               	;					reti				;16 0x00F TIMER0 COMPB Timer/Counter0 Compare Match B
 280               	;					rjmp	TIMER0_OVF		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
 281               	;					rjmp	SPI_OK			;18 0x011 SPI, STC SPI Serial Transfer Complete
 282               	;					rjmp	RX_OK			;19 0x012 USART, RX USART Rx Complete
 283               	;					rjmp	DRE_OK			;20 0x013 USART, UDRE USART, Data Register Empty
 284               	;					rjmp	TX_OK			;21 0x014 USART, TX USART, Tx Complete
 285               	;					reti				;22 0x015 ADC ADC Conversion Complete
 286               	;					reti				;23 0x016 EE READY EEPROM Ready
 287               	; 8a mega
 288 0000 00C0      	reset:					rjmp	asm_main		;1 0x000 RESET External Pin, Power-on Reset, Brown-out Reset and Watchdog
 289 0002 1895      						reti				;2 0x001 INT0 External Interrupt Request 0
 290 0004 1895      						reti				;3 0x002 INT1 External Interrupt Request 1
 291 0006 1895      						reti				;4 0x003 PCINT0 Pin Change Interrupt Request 0
 292 0008 1895      						reti				;5 0x004 PCINT1 Pin Change Interrupt Request 1
 293 000a 1895      						reti				;6 0x005 PCINT2 Pin Change Interrupt Request 2
 294 000c 1895      						reti				;7 0x006 WDT Watchdog Time-out Interrupt
 295 000e 1895      						reti				;8 0x007 TIMER2 COMPA Timer/Counter2 Compare Match A
 296 0010 1895      						reti				;9 0x008 TIMER2 COMPB Timer/Counter2 Compare Match B
 297 0012 00C0      						rjmp	TIMER0_OVF		;17 0x010 TIMER0 OVF Timer/Counter0 Overflow
 298 0014 00C0      						rjmp	SPI_OK			;18 0x011 SPI, STC SPI Serial Transfer Complete
 299 0016 00C0      						rjmp	RX_OK			;19 0x012 USART, RX USART Rx Complete
 300 0018 00C0      						rjmp	DRE_OK			;20 0x013 USART, UDRE USART, Data Register Empty
 301 001a 00C0      						rjmp	TX_OK			;21 0x014 USART, TX USART, Tx Complete
 302 001c 1895      						reti				;22 0x015 ADC ADC Conversion Complete
 303 001e 1895      						reti				;23 0x016 EE READY EEPROM Ready
 304               	
 305               	
 306               	; Обработчики прерываний -----------------------------------------------------
 307               	
 308               	TIMER0_OVF:				pushf
 308 0020 0F93      	>  push r16
 308 0022 0FB7      	>  in r16,SREG
 308 0024 0F93      	>  push r16
 309 0026 1F93      						push	r17
 310 0028 2F93      						push	r18
 311 002a 3F93      						push	r19
 312               	
 313               						incm	TCNT
 313 002c 0091 0000 	>  lds r16,TCNT
 313 0030 1091 0000 	>  lds r17,TCNT+1
 313               	>  
 313               	>  
 313               	> 
 313 0034 0F5F      	>  subi r16,(-1)
 313 0036 1F4F      	>  sbci r17,(-1)
 313               	>  
 313               	>  
 313               	> 
 313 0038 0093 0000 	>  sts TCNT,r16
 313 003c 1093 0000 	>  sts TCNT+1,r17
 313               	>  
 313               	>  
 314               						incm	CTCNT2
 314 0040 0091 0000 	>  lds r16,CTCNT2
 314 0044 1091 0000 	>  lds r17,CTCNT2+1
 314               	>  
 314               	>  
GAS LISTING avr.s 			page 20


 314               	> 
 314 0048 0F5F      	>  subi r16,(-1)
 314 004a 1F4F      	>  sbci r17,(-1)
 314               	>  
 314               	>  
 314               	> 
 314 004c 0093 0000 	>  sts CTCNT2,r16
 314 0050 1093 0000 	>  sts CTCNT2+1,r17
 314               	>  
 314               	>  
 315               						incm	CTCNT3
 315 0054 0091 0000 	>  lds r16,CTCNT3
 315 0058 1091 0000 	>  lds r17,CTCNT3+1
 315               	>  
 315               	>  
 315               	> 
 315 005c 0F5F      	>  subi r16,(-1)
 315 005e 1F4F      	>  sbci r17,(-1)
 315               	>  
 315               	>  
 315               	> 
 315 0060 0093 0000 	>  sts CTCNT3,r16
 315 0064 1093 0000 	>  sts CTCNT3+1,r17
 315               	>  
 315               	>  
 316               						incm	CTCNT4
 316 0068 0091 0000 	>  lds r16,CTCNT4
 316 006c 1091 0000 	>  lds r17,CTCNT4+1
 316               	>  
 316               	>  
 316               	> 
 316 0070 0F5F      	>  subi r16,(-1)
 316 0072 1F4F      	>  sbci r17,(-1)
 316               	>  
 316               	>  
 316               	> 
 316 0074 0093 0000 	>  sts CTCNT4,r16
 316 0078 1093 0000 	>  sts CTCNT4+1,r17
 316               	>  
 316               	>  
 317               						incm	CTCNT5
 317 007c 0091 0000 	>  lds r16,CTCNT5
 317 0080 1091 0000 	>  lds r17,CTCNT5+1
 317               	>  
 317               	>  
 317               	> 
 317 0084 0F5F      	>  subi r16,(-1)
 317 0086 1F4F      	>  sbci r17,(-1)
 317               	>  
 317               	>  
 317               	> 
 317 0088 0093 0000 	>  sts CTCNT5,r16
 317 008c 1093 0000 	>  sts CTCNT5+1,r17
 317               	>  
 317               	>  
 318               	
 319 0090 3F91      						pop	r19
GAS LISTING avr.s 			page 21


 320 0092 2F91      						pop	r18
 321 0094 1F91      						pop	r17
 322               						popf
 322 0096 0F91      	>  pop r16
 322 0098 0FBF      	>  out SREG,r16
 322 009a 0F91      	>  pop r16
 323               	
 324 009c 1895      						reti
 325               	
 326               	RX_OK:
 327               	;					pushf
 328               	;					push	r16
 329               	;
 330               	;					lds	r16,	UDR0						; Считываем данные с Rx буфера, флаг преры�
 331               	;
 332               	;					cpi	r16,	101
 333               	;					breq	RX_OK_LIGHT
 334               	;
 335               	;					cpi	r16,	100
 336               	;					breq	RX_OK_DARK
 337               	;
 338               	;					pop	r16
 339               	;					popf
 340 009e 1895      						reti
 341               	
 342               	RX_OK_LIGHT:			;	sbi	PORTC,	2
 343               						;sbi	PORTD,	4
 344 00a0 0F91      						pop	r16
 345               						popf
 345 00a2 0F91      	>  pop r16
 345 00a4 0FBF      	>  out SREG,r16
 345 00a6 0F91      	>  pop r16
 346 00a8 1895      						reti
 347               	
 348               	RX_OK_DARK:				;cbi	PORTC,	2
 349               						;cbi	PORTD,	4
 350 00aa 0F91      						pop	r16
 351               						popf
 351 00ac 0F91      	>  pop r16
 351 00ae 0FBF      	>  out SREG,r16
 351 00b0 0F91      	>  pop r16
 352 00b2 1895      						reti
 353               	
 354               						; Складываем в ОЗУ и с помощью планировщика устанав�
 355               						;buff_out	RX_BUFF_OUT,	r16				; Тратить время внутри прерывания н
 356               														; кладем данные в буфер ОЗУ и переходим к обрабо
 357               														; если ядро свободно иначе ставим флаг - нужна о�
 358               														; ядро закончит другую работу она доделает что �
 359               	
 360               						;schd_job	F_RX
 361               	
 362 00b4 1895      						reti
 363               	
 364               	SPI_OK:					;ldi	ZL,	lo8(spi_ok_string)
 365               						;ldi	ZH,	hi8(spi_ok_string)
 366 00b6 1895      						reti
 367               	
GAS LISTING avr.s 			page 22


 368               	;spi_start_tx:				lpm	r16,	Z+		; Достанем из флеша константную строк
 369               	;					cpi	r16,	0x00		; Если не конец строки отсылаем в uart
 370               	;					breq	spi_stop_tx		; Иначе завершаем передачу
 371               	;					rcall	uart_snt
 372               	;					rjmp	spi_start_tx
 373               	;
 374               	;spi_stop_tx:				reti
 375               	
 376               	TX_OK:
 377 00b8 1895      	DRE_OK:					reti
 378               	
 379               	; Подпрограммы инициализации ---------------------------------------------
 380               	
 381 00ba 03E3      	uart_init:				ldi 	r16,	lo8(bauddivider)	; Настраиваем бод режим, парамет
 382 00bc 0093 0900 						sts 	UBRRL,	r16
 383 00c0 00E0      						ldi 	r16, 	hi8(bauddivider)
 384 00c2 0093 2000 						sts 	UBRRH,	r16
 385               	
 386               				;		ldi	r16,		lo8(RX_BUFF)	; Инициализируем указатели кольцевых �
 387               				;		sts	RX_BUFF_IN,	r16		; чтения \ записи по прерыванию в неверн�
 388               				;		sts	RX_BUFF_OUT,	r16
 389               				;		ldi	r16,		hi8(RX_BUFF)
 390               				;		sts	RX_BUFF_IN + 1,		r16	; чтения \ записи по прерыванию в неве�
 391               				;		sts	RX_BUFF_OUT + 1,	r16
 392               	
 393               				;		ldi	r16,		lo8(TX_BUFF)	; Инициализируем указатели кольцевых �
 394               				;		sts	TX_BUFF_IN,	r16		; чтения \ записи по прерыванию в неверн�
 395               				;		sts	TX_BUFF_OUT,	r16
 396               				;		ldi	r16,		hi8(TX_BUFF)
 397               				;		sts	TX_BUFF_IN + 1,		r16	; чтения \ записи по прерыванию в неве�
 398               				;		sts	TX_BUFF_OUT + 1,	r16
 399               	
 400 00c6 00E0      						ldi 	r16,	0
 401 00c8 0093 0B00 						sts 	UCSRA,	R16
 402               	
 403               						; Прерывания разрешены, прием-передача разрешен.
 404 00cc 08EF      						ldi 	r16,	(1 << RXEN) | (1 << TXEN) | (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE)
 405 00ce 0093 0A00 						sts 	UCSRB,	r16
 406               	
 407               						; Формат кадра - 8 бит, пишем в регистр UCSRC, за это отве
 408 00d2 06E0      						ldi 	r16,	(1 << UCSZ0) | (1 << UCSZ1)
 409 00d4 0093 2000 						sts 	UCSRC,	r16
 410               	
 411 00d8 0895      						ret
 412               	
 413               	timer0_init:				setbi	TIMSK,	TOIE0		; Включим прерывания переполнения 
 413               	>  .if TIMSK<=0x20
 413               	>  sbi TIMSK,TOIE0
 413               	>  .else
 413               	>  .if TIMSK<=0x40
 413               	>  
 413 00da 09B7      	>  in r16,TIMSK
 413 00dc 0160      	>  sbr r16,1<<TOIE0
 413 00de 09BF      	>  out TIMSK,r16
 413               	>  .else
 413               	>  lds r16,TIMSK
 413               	>  sbr r16,1<<TOIE0
GAS LISTING avr.s 			page 23


 413               	>  sts TIMSK,r16
 413               	>  .endif
 413               	>  .endif
 414               						outi	TCCR0,	1 << CS00	; Запускаем таймер. Предделитель = 1
 414 00e0 01E0      	>  ldi R16,1<<CS00
 414               	>  .if TCCR0<0x40
 414 00e2 03BF      	>  out TCCR0,R16
 414               	>  .else
 414               	>  sts TCCR0,R16
 414               	>  .endif
 415               										; Т.е. тикаем с тактовой частотой.
 416 00e4 0895      						ret
 417               	
 418 00e6 0027      	memory_init:				clr	r16			; Очищаем память в 0x0
 419 00e8 1F91      						pop	r17			; Сохраним адрес возврата
 420 00ea 2F91      						pop	r18
 421 00ec E0E6      						ldi	ZL,	lo8(SRAM_START)
 422 00ee F0E0      						ldi	ZH,	hi8(SRAM_START)
 423 00f0 0193      	memory_clr:				st	Z+,	r16
 424 00f2 E036      						cpi	ZL,	lo8(RAMEND + 1)
 425 00f4 01F4      						brne	memory_clr
 426 00f6 F430      						cpi	ZH,	hi8(RAMEND + 1)
 427 00f8 01F4      						brne	memory_clr
 428 00fa 2F93      						push	r18			; Восстановим адрес перед ret
 429 00fc 1F93      						push	r17
 430               	
 431 00fe 0895      						ret
 432               	
 433 0100 A19A      	io_ports_init:				sbi	DDRC,	1		; Initialize ports settings & set their data
 434 0102 A29A      						sbi	DDRC,	2
 435 0104 A09A      						sbi	DDRC,	0		; Initialize port's pin as input
 436               						; EMI lock mosfet
 437 0106 A59A      						sbi	DDRC,	5
 438 0108 AD9A      						sbi	PORTC,	5
 439               						; 2nd mosfet
 440 010a 8D9A      						sbi	DDRD,	5
 441 010c 959A      						sbi	PORTD,	5
 442               						; 220 Optopair
 443 010e A49A      						sbi	DDRC,	4		; Initialize port's pin as input
 444 0110 BA9A      						sbi	DDRB,	2
 445               						;sbi	PORTB,	2
 446               						;sbi	DDRD,	4
 447               						; Speaker
 448 0112 8E9A      						sbi	DDRD,	6
 449 0114 9698      						cbi	PORTD,	6
 450               	
 451               						; DANGER!!! RX line is used as button, so it shouldn't be set as output or it burns out.
 452 0116 8998      						cbi	DDRD,	1
 453               						; Add pulltup to button line
 454 0118 919A      						sbi	PORTD,	1
 455               	
 456 011a 0895      						ret
 457               	
 458               	; Подпрограммы коммуникации -----------------------------------------------
 459               	
 460               	; Процедура отправки байта
 461               	;uart_snt:				lds 	r17,	UCSR0A		; Пропуск если нет флага готовности
GAS LISTING avr.s 			page 24


 462               	;					andi	r17,	1 << UDRE0
 463               	;					cpi	r17,	1 << UDRE0
 464               	;					brne	uart_snt		; ждем готовности - флага UDRE
 465               	;
 466               	;					sts	UDR0, r16		; шлем байт
 467               	;
 468               	;					ret				; Возврат
 469               	
 470               	asm_main:
 471               	; Начало программы -----------------------------------------------------------------
 472               	; Инициализация локальной перифeрии ---------------------------------
 473               	
 474 011c 0FE5      						ldi	r16,	lo8(RAMEND)	; инициализация стека
 475 011e 0DBF      						out	SPL,	r16
 476 0120 04E0      						ldi	r16,	hi8(RAMEND)
 477 0122 0EBF      	                                        out	SPH,	r16
 478               	
 479 0124 00D0      						rcall	memory_init
 480 0126 00D0      						rcall	timer0_init
 481               						; Now we using RX line as button for door open as default line was burned by accident
 482               		;				rcall	uart_init
 483 0128 00D0      						rcall	io_ports_init
 484               	
 485 012a 00D0      						rcall	main_init
 486               	
 487 012c 7894      						sei				; Разрешаем глобальные прерывания
 488               	
 489               	; Проверяем прошел ли заданный интервал времени -----------
 490 012e 0091 0000 	loop:					lds	r16,	TCNT
 491 0132 0E32      						cpi	r16,	0x2e
 492 0134 00F0      						brcs	idle
 493 0136 0091 0000 						lds	r16,	TCNT + 1
 494 013a 0A3E      						cpi	r16,	0xea
 495 013c 00F0      						brcs	idle
 496               	
 497               	; Если прошел выполняем обработчик события иначе выше 
 498 013e 00C0      						rjmp	handle
 499               	
 500               	; Холостые итерации - обрабатываем полезную нагрузки п
 501               	idle:
 502 0140 2091 0000 						lds	r18,	NET_ACTIVITY
 503 0144 2130      						cpi	r18,	1
 504 0146 01F4      						brne	skip_net_activity
 505               	
 506 0148 AA9A      						sbi	PORTC,	2
 507               	
 508 014a 0091 0000 						lds	r16,	CTCNT3
 509 014e 0031      						cpi	r16,	0x10
 510 0150 00F0      						brcs	skip_net_activity
 511 0152 0091 0000 						lds	r16,	CTCNT3 + 1
 512 0156 0C30      						cpi	r16,	0x0c
 513 0158 00F0      						brcs	skip_net_activity
 514               	
 515 015a 80E0      						ldi	r24,	lo8(CTCNT3)
 516 015c 90E0      						ldi	r25,	hi8(CTCNT3)
 517               	
 518 015e 00D0      						rcall	clear_tcn
GAS LISTING avr.s 			page 25


 519               	
 520 0160 2227      						clr	r18
 521 0162 2093 0000 						sts	NET_ACTIVITY,	r18
 522 0166 AA98      						cbi	PORTC,	2
 523 0168 949A      						sbi	PORTD,	4
 524               	
 525               	skip_net_activity:
 526               	
 527 016a 0091 0000 						lds	r16,	CTCNT4
 528 016e 0031      						cpi	r16,	0x10
 529 0170 00F0      						brcs	rfid_end
 530 0172 0091 0000 						lds	r16,	CTCNT4 + 1
 531 0176 0A34      						cpi	r16,	0x4a
 532 0178 00F0      						brcs	rfid_end
 533 017a 00D0      						rcall	rfid
 534               						clr_tcnt	CTCNT4
 534 017c 80E0      	>  ldi r24,lo8(CTCNT4)
 534 017e 90E0      	>  ldi r25,hi8(CTCNT4)
 534               	> 
 534 0180 00D0      	>  rcall clear_tcn
 535               	
 536 0182 2091 0000 	rfid_end: 				lds	r18,	RFID_ACTIVITY
 537 0186 2130      						cpi	r18,	1
 538 0188 01F4      						brne	skip_rfid_activity
 539               	
 540 018a C29A      						sbi	PORTB,	2
 541               	
 542 018c 0091 0000 						lds	r16,	CTCNT5
 543 0190 0031      						cpi	r16,	0x10
 544 0192 00F0      						brcs	skip_rfid_activity
 545 0194 0091 0000 						lds	r16,	CTCNT5 + 1
 546 0198 0A39      						cpi	r16,	0x9a
 547 019a 00F0      						brcs	skip_rfid_activity
 548               	
 549 019c 20E0      						ldi	r18,	0
 550 019e 2093 0000 						sts	RFID_ACTIVITY,	r18
 551 01a2 C298      						cbi	PORTB,	2
 552               	
 553               	skip_rfid_activity:
 554               	
 555               	;					lds	r18,	OPEN_SIGNAL
 556               	;					cpi	r18,	1
 557               	;					brne	skip_open_signal
 558               	;
 559               	;					lds	r18,	SIGNAL_DELTA
 560               	;					cpi	r18,	0
 561               	;
 562               	;					brcs	open_delta_inc
 563               	;
 564               	;					invb	PORTD,	6
 565               	;					clr	r18
 566               	;					rjmp	open_delta_upd
 567               	;
 568               	;open_delta_inc:				inc	r18
 569               	;open_delta_upd:				sts	SIGNAL_DELTA,	r18
 570               	;
 571               	;					lds	r16,	CTCNT2
GAS LISTING avr.s 			page 26


 572               	;					cpi	r16,	0x10
 573               	;					brcs	skip_open_signal
 574               	;					lds	r16,	CTCNT2 + 1
 575               	;					cpi	r16,	0x2a
 576               	;					brcs	skip_open_signal
 577               	;
 578               	;					clr	r18
 579               	;					sts	OPEN_SIGNAL,	r18
 580               	;					cbi	PORTD,	6
 581               	;
 582               	;skip_open_signal:
 583               	
 584 01a4 2091 0000 						lds	r18,	DENIED_SIGNAL
 585 01a8 2130      						cpi	r18,	1
 586 01aa 01F4      						brne	skip_denied_signal
 587               	
 588 01ac 2091 0000 						lds	r18,	SIGNAL_DELTA
 589 01b0 2A39      						cpi	r18,	154
 590               	
 591 01b2 00F0      						brcs	denied_delta_inc
 592               	
 593               						invb	PORTD,	6
 593 01b4 00E4      	>  ldi r16,(1<<6)
 593               	>  .if PORTD<0x40
 593 01b6 12B3      	>  in r17,PORTD
 593 01b8 1027      	>  eor r17,r16
 593 01ba 12BB      	>  out PORTD,r17
 593               	>  .else
 593               	>  lds r17,PORTD
 593               	>  eor r17,r16
 593               	>  sts PORTD,r17
 593               	>  .endif
 594 01bc 2227      						clr	r18
 595 01be 00C0      						rjmp	denied_delta_upd
 596               	
 597 01c0 2395      	denied_delta_inc:			inc	r18
 598 01c2 2093 0000 	denied_delta_upd:			sts	SIGNAL_DELTA,	r18
 599               	
 600 01c6 0091 0000 						lds	r16,	CTCNT5
 601 01ca 0031      						cpi	r16,	0x10
 602 01cc 00F0      						brcs	skip_denied_signal
 603 01ce 0091 0000 						lds	r16,	CTCNT5 + 1
 604 01d2 0232      						cpi	r16,	0x22
 605 01d4 00F0      						brcs	skip_denied_signal
 606               	
 607 01d6 2227      						clr	r18
 608 01d8 2093 0000 						sts	DENIED_SIGNAL,	r18
 609 01dc 9698      						cbi	PORTD,	6
 610               	
 611               	skip_denied_signal:
 612 01de 2091 0000 						lds	r18,	OPEN_DOOR
 613 01e2 2130      						cpi	r18,	1
 614 01e4 01F4      						brne	skip_open_door
 615               	
 616               						; leds
 617 01e6 A99A      						sbi	PORTC,	1
 618 01e8 C29A      						sbi	PORTB,	2
GAS LISTING avr.s 			page 27


 619               	
 620               						; 220
 621               				;		sbi	PORTC,	4
 622 01ea AD98      						cbi	PORTC,	5
 623               						;cbi	PORTD,	5
 624               	
 625 01ec 0091 0000 						lds	r16,	CTCNT2
 626 01f0 0E3F      						cpi	r16,	0xFE
 627 01f2 00F0      						brcs	skip_open_door
 628 01f4 0091 0000 						lds	r16,	CTCNT2 + 1
 629 01f8 0E3F      						cpi	r16,	0xFE
 630 01fa 00F0      						brcs	skip_open_door
 631               	
 632               						clr_tcnt	CTCNT2
 632 01fc 80E0      	>  ldi r24,lo8(CTCNT2)
 632 01fe 90E0      	>  ldi r25,hi8(CTCNT2)
 632               	> 
 632 0200 00D0      	>  rcall clear_tcn
 633               	
 634 0202 2227      						clr	r18
 635 0204 2093 0000 						sts	OPEN_DOOR,	r18
 636 0208 A998      						cbi	PORTC,	1
 637 020a C298      						cbi	PORTB,	2
 638               				;		cbi	PORTC,	4
 639               	
 640 020c AD9A      						sbi	PORTC,	5
 641               						;sbi	PORTD,	5
 642               	
 643               	skip_open_door:
 644               	
 645 020e 9B99      	button_check:				sbic	PINC,	3
 646 0210 00C0      						rjmp	button_end
 647               	
 648 0212 00D0      	button_on: 				rcall	open_door_script
 649               	
 650 0214 00D0      	button_end: 				rcall	lan_poll
 651 0216 00C0      						rjmp	loop
 652               	
 653               	handle:
 654               						invb	PORTC,		0	; Инвертируем светодиод
 654 0218 01E0      	>  ldi r16,(1<<0)
 654               	>  .if PORTC<0x40
 654 021a 15B3      	>  in r17,PORTC
 654 021c 1027      	>  eor r17,r16
 654 021e 15BB      	>  out PORTC,r17
 654               	>  .else
 654               	>  lds r17,PORTC
 654               	>  eor r17,r16
 654               	>  sts PORTC,r17
 654               	>  .endif
 655               	
 656               	;					; Отправляем в UART строку
 657               	;					cli
 658               	;					ldi	YL,	lo8(term_var_info)
 659               	;					ldi	YH,	hi8(term_var_info)
 660               	;					ld	ZL,	Y+
 661               	;					ld	ZH,	Y
GAS LISTING avr.s 			page 28


 662               	;					rjmp	start_tx
 663               	;
 664               	;start_tx:				ld	r16,	Z+		; Достанем из флеша константную строку с
 665               	;					cpi	r16,	0x00		; Если не конец строки отсылаем в uart
 666               	;					breq	stop_tx			; Иначе завершаем передачу
 667               	;					rcall	uart_snt
 668               	;					rjmp	start_tx
 669               	
 670               	;stop_tx:
 671 0220 F894      						cli
 672 0222 0027      						clr	r16
 673 0224 0093 0000 						sts	TCNT,		r16	; Обнуляем счетчик в ОЗУ
 674 0228 0093 0000 						sts	TCNT + 1,	r16
 675 022c 0093 3200 						sts	TCNT0,		r16	; Обнуляем счетчик таймера
 676 0230 7894      						sei				; Включаем прерывания
 677               	
 678 0232 00C0      						rjmp	loop
 679               	;.global fill_net_buff
 680               	;
 681               	;fill_net_buff:
 682               	;					;movw	X,	r24
 683               	;	;				cpi	r22,	1
 684               	;					;brne	retval_ignored
 685               	;
 686               	;retval_ok:			;	ldi	ZL,	lo8(retval_ok_str)
 687               	; 				;	ldi	ZH,	hi8(retval_ok_str)
 688               	;				;	rjmp	fill_net_buff_next_char
 689               	;
 690               	;retval_ignored:				;ldi	ZL,	lo8(retval_ignored_str)
 691               	; 					;ldi	ZH,	hi8(retval_ignored_str)
 692               	;
 693               	;fill_net_buff_next_char:	;	lpm	r18,	Z+
 694               	;				;	cpi	r18,	0
 695               	;				;	breq	fill_net_buff_end
 696               	;
 697               	;					;ldi	r18,	'o
 698               	;					;st	X+,	r18
 699               	;					;ldi	r18,	'k
 700               	;					;st	X+,	r18
 701               	;					;rjmp	fill_net_buff_next_char
 702               	;
 703               	;fill_net_buff_end:
 704 0234 0895      						ret
 705               	
 706               	.global clear_tcn
 707 0236 F894      	clear_tcn:				cli
 708 0238 FC01      						movw	ZL,	r24
 709 023a 0027      						clr	r16
 710 023c 0193      						st	Z+,	r16	        ; Обнуляем счетчик в ОЗУ
 711 023e 0083      						st	Z,	r16
 712 0240 7894      						sei				; Включаем прерывания
 713 0242 0895      						ret
 714               	
 715               	; Константы записанные в памяти программы ----------------------
 716               	data:
 717               	;term_var_init:				.string	"Serial terminal initialization...\n\r"
 718               	;spi_ok_string:				.string	"spi ok\n\r"
GAS LISTING avr.s 			page 29


 719               	.global term_var_info
 720               	;term_var_info:				.ascii	"Terminal version:	Osci 0.01 alpha\n\r"
 721               	;retval_ignored_str:			.asciz	"ignored"
 722 0244 636F 6D6D 	OP_OPEN:				.asciz	"command: open"
 722      616E 643A 
 722      206F 7065 
 722      6E00 
 723               	.global OP_OPEN
 724               	;					.asciz	"Description:		Used to control, debug & measure embedded microchips\n\n\r"
 725 0252 0000      						.align	2
GAS LISTING avr.s 			page 30


DEFINED SYMBOLS
/sources/avr//include/m8Adef.inc:49     *ABS*:000000000000001e SIGNATURE_000
/sources/avr//include/m8Adef.inc:50     *ABS*:0000000000000093 SIGNATURE_001
/sources/avr//include/m8Adef.inc:51     *ABS*:0000000000000007 SIGNATURE_002
/sources/avr//include/m8Adef.inc:60     *ABS*:000000000000003f SREG
/sources/avr//include/m8Adef.inc:61     *ABS*:000000000000003d SPL
/sources/avr//include/m8Adef.inc:62     *ABS*:000000000000003e SPH
/sources/avr//include/m8Adef.inc:63     *ABS*:000000000000003b GICR
/sources/avr//include/m8Adef.inc:64     *ABS*:000000000000003a GIFR
/sources/avr//include/m8Adef.inc:65     *ABS*:0000000000000039 TIMSK
/sources/avr//include/m8Adef.inc:66     *ABS*:0000000000000038 TIFR
/sources/avr//include/m8Adef.inc:67     *ABS*:0000000000000037 SPMCR
/sources/avr//include/m8Adef.inc:68     *ABS*:0000000000000036 TWCR
/sources/avr//include/m8Adef.inc:69     *ABS*:0000000000000035 MCUCR
/sources/avr//include/m8Adef.inc:70     *ABS*:0000000000000034 MCUCSR
/sources/avr//include/m8Adef.inc:71     *ABS*:0000000000000033 TCCR0
/sources/avr//include/m8Adef.inc:72     *ABS*:0000000000000032 TCNT0
/sources/avr//include/m8Adef.inc:73     *ABS*:0000000000000031 OSCCAL
/sources/avr//include/m8Adef.inc:74     *ABS*:0000000000000030 SFIOR
/sources/avr//include/m8Adef.inc:75     *ABS*:000000000000002f TCCR1A
/sources/avr//include/m8Adef.inc:76     *ABS*:000000000000002e TCCR1B
/sources/avr//include/m8Adef.inc:77     *ABS*:000000000000002c TCNT1L
/sources/avr//include/m8Adef.inc:78     *ABS*:000000000000002d TCNT1H
/sources/avr//include/m8Adef.inc:79     *ABS*:000000000000002a OCR1AL
/sources/avr//include/m8Adef.inc:80     *ABS*:000000000000002b OCR1AH
/sources/avr//include/m8Adef.inc:81     *ABS*:0000000000000028 OCR1BL
/sources/avr//include/m8Adef.inc:82     *ABS*:0000000000000029 OCR1BH
/sources/avr//include/m8Adef.inc:83     *ABS*:0000000000000026 ICR1L
/sources/avr//include/m8Adef.inc:84     *ABS*:0000000000000027 ICR1H
/sources/avr//include/m8Adef.inc:85     *ABS*:0000000000000025 TCCR2
/sources/avr//include/m8Adef.inc:86     *ABS*:0000000000000024 TCNT2
/sources/avr//include/m8Adef.inc:87     *ABS*:0000000000000023 OCR2
/sources/avr//include/m8Adef.inc:88     *ABS*:0000000000000022 ASSR
/sources/avr//include/m8Adef.inc:89     *ABS*:0000000000000021 WDTCR
/sources/avr//include/m8Adef.inc:90     *ABS*:0000000000000020 UBRRH
/sources/avr//include/m8Adef.inc:91     *ABS*:0000000000000020 UCSRC
/sources/avr//include/m8Adef.inc:92     *ABS*:000000000000001e EEARL
/sources/avr//include/m8Adef.inc:93     *ABS*:000000000000001f EEARH
/sources/avr//include/m8Adef.inc:94     *ABS*:000000000000001d EEDR
/sources/avr//include/m8Adef.inc:95     *ABS*:000000000000001c EECR
/sources/avr//include/m8Adef.inc:96     *ABS*:0000000000000018 PORTB
/sources/avr//include/m8Adef.inc:97     *ABS*:0000000000000017 DDRB
/sources/avr//include/m8Adef.inc:98     *ABS*:0000000000000016 PINB
/sources/avr//include/m8Adef.inc:99     *ABS*:0000000000000015 PORTC
/sources/avr//include/m8Adef.inc:100    *ABS*:0000000000000014 DDRC
/sources/avr//include/m8Adef.inc:101    *ABS*:0000000000000013 PINC
/sources/avr//include/m8Adef.inc:102    *ABS*:0000000000000012 PORTD
/sources/avr//include/m8Adef.inc:103    *ABS*:0000000000000011 DDRD
/sources/avr//include/m8Adef.inc:104    *ABS*:0000000000000010 PIND
/sources/avr//include/m8Adef.inc:105    *ABS*:000000000000000f SPDR
/sources/avr//include/m8Adef.inc:106    *ABS*:000000000000000e SPSR
/sources/avr//include/m8Adef.inc:107    *ABS*:000000000000000d SPCR
/sources/avr//include/m8Adef.inc:108    *ABS*:000000000000000c UDR
/sources/avr//include/m8Adef.inc:109    *ABS*:000000000000000b UCSRA
/sources/avr//include/m8Adef.inc:110    *ABS*:000000000000000a UCSRB
/sources/avr//include/m8Adef.inc:111    *ABS*:0000000000000009 UBRRL
/sources/avr//include/m8Adef.inc:112    *ABS*:0000000000000008 ACSR
GAS LISTING avr.s 			page 31


/sources/avr//include/m8Adef.inc:113    *ABS*:0000000000000007 ADMUX
/sources/avr//include/m8Adef.inc:114    *ABS*:0000000000000006 ADCSRA
/sources/avr//include/m8Adef.inc:115    *ABS*:0000000000000004 ADCL
/sources/avr//include/m8Adef.inc:116    *ABS*:0000000000000005 ADCH
/sources/avr//include/m8Adef.inc:117    *ABS*:0000000000000003 TWDR
/sources/avr//include/m8Adef.inc:118    *ABS*:0000000000000002 TWAR
/sources/avr//include/m8Adef.inc:119    *ABS*:0000000000000001 TWSR
/sources/avr//include/m8Adef.inc:120    *ABS*:0000000000000000 TWBR
/sources/avr//include/m8Adef.inc:127    *ABS*:0000000000000003 ACME
/sources/avr//include/m8Adef.inc:130    *ABS*:0000000000000000 ACIS0
/sources/avr//include/m8Adef.inc:131    *ABS*:0000000000000001 ACIS1
/sources/avr//include/m8Adef.inc:132    *ABS*:0000000000000002 ACIC
/sources/avr//include/m8Adef.inc:133    *ABS*:0000000000000003 ACIE
/sources/avr//include/m8Adef.inc:134    *ABS*:0000000000000004 ACI
/sources/avr//include/m8Adef.inc:135    *ABS*:0000000000000005 ACO
/sources/avr//include/m8Adef.inc:136    *ABS*:0000000000000006 ACBG
/sources/avr//include/m8Adef.inc:137    *ABS*:0000000000000007 ACD
/sources/avr//include/m8Adef.inc:142    *ABS*:0000000000000000 SPDR0
/sources/avr//include/m8Adef.inc:143    *ABS*:0000000000000001 SPDR1
/sources/avr//include/m8Adef.inc:144    *ABS*:0000000000000002 SPDR2
/sources/avr//include/m8Adef.inc:145    *ABS*:0000000000000003 SPDR3
/sources/avr//include/m8Adef.inc:146    *ABS*:0000000000000004 SPDR4
/sources/avr//include/m8Adef.inc:147    *ABS*:0000000000000005 SPDR5
/sources/avr//include/m8Adef.inc:148    *ABS*:0000000000000006 SPDR6
/sources/avr//include/m8Adef.inc:149    *ABS*:0000000000000007 SPDR7
/sources/avr//include/m8Adef.inc:152    *ABS*:0000000000000000 SPI2X
/sources/avr//include/m8Adef.inc:153    *ABS*:0000000000000006 WCOL
/sources/avr//include/m8Adef.inc:154    *ABS*:0000000000000007 SPIF
/sources/avr//include/m8Adef.inc:157    *ABS*:0000000000000000 SPR0
/sources/avr//include/m8Adef.inc:158    *ABS*:0000000000000001 SPR1
/sources/avr//include/m8Adef.inc:159    *ABS*:0000000000000002 CPHA
/sources/avr//include/m8Adef.inc:160    *ABS*:0000000000000003 CPOL
/sources/avr//include/m8Adef.inc:161    *ABS*:0000000000000004 MSTR
/sources/avr//include/m8Adef.inc:162    *ABS*:0000000000000005 DORD
/sources/avr//include/m8Adef.inc:163    *ABS*:0000000000000006 SPE
/sources/avr//include/m8Adef.inc:164    *ABS*:0000000000000007 SPIE
/sources/avr//include/m8Adef.inc:169    *ABS*:000000000000003b GIMSK
/sources/avr//include/m8Adef.inc:170    *ABS*:0000000000000000 IVCE
/sources/avr//include/m8Adef.inc:171    *ABS*:0000000000000001 IVSEL
/sources/avr//include/m8Adef.inc:172    *ABS*:0000000000000006 INT0
/sources/avr//include/m8Adef.inc:173    *ABS*:0000000000000007 INT1
/sources/avr//include/m8Adef.inc:176    *ABS*:0000000000000006 INTF0
/sources/avr//include/m8Adef.inc:177    *ABS*:0000000000000007 INTF1
/sources/avr//include/m8Adef.inc:180    *ABS*:0000000000000000 ISC00
/sources/avr//include/m8Adef.inc:181    *ABS*:0000000000000001 ISC01
/sources/avr//include/m8Adef.inc:182    *ABS*:0000000000000002 ISC10
/sources/avr//include/m8Adef.inc:183    *ABS*:0000000000000003 ISC11
/sources/avr//include/m8Adef.inc:188    *ABS*:0000000000000000 TOIE0
/sources/avr//include/m8Adef.inc:191    *ABS*:0000000000000000 TOV0
/sources/avr//include/m8Adef.inc:194    *ABS*:0000000000000000 CS00
/sources/avr//include/m8Adef.inc:195    *ABS*:0000000000000001 CS01
/sources/avr//include/m8Adef.inc:196    *ABS*:0000000000000002 CS02
/sources/avr//include/m8Adef.inc:199    *ABS*:0000000000000000 TCNT00
/sources/avr//include/m8Adef.inc:200    *ABS*:0000000000000001 TCNT01
/sources/avr//include/m8Adef.inc:201    *ABS*:0000000000000002 TCNT02
/sources/avr//include/m8Adef.inc:202    *ABS*:0000000000000003 TCNT03
/sources/avr//include/m8Adef.inc:203    *ABS*:0000000000000004 TCNT04
GAS LISTING avr.s 			page 32


/sources/avr//include/m8Adef.inc:204    *ABS*:0000000000000005 TCNT05
/sources/avr//include/m8Adef.inc:205    *ABS*:0000000000000006 TCNT06
/sources/avr//include/m8Adef.inc:206    *ABS*:0000000000000007 TCNT07
/sources/avr//include/m8Adef.inc:211    *ABS*:0000000000000002 TOIE1
/sources/avr//include/m8Adef.inc:212    *ABS*:0000000000000003 OCIE1B
/sources/avr//include/m8Adef.inc:213    *ABS*:0000000000000004 OCIE1A
/sources/avr//include/m8Adef.inc:214    *ABS*:0000000000000005 TICIE1
/sources/avr//include/m8Adef.inc:217    *ABS*:0000000000000002 TOV1
/sources/avr//include/m8Adef.inc:218    *ABS*:0000000000000003 OCF1B
/sources/avr//include/m8Adef.inc:219    *ABS*:0000000000000004 OCF1A
/sources/avr//include/m8Adef.inc:220    *ABS*:0000000000000005 ICF1
/sources/avr//include/m8Adef.inc:223    *ABS*:0000000000000000 WGM10
/sources/avr//include/m8Adef.inc:224    *ABS*:0000000000000000 PWM10
/sources/avr//include/m8Adef.inc:225    *ABS*:0000000000000001 WGM11
/sources/avr//include/m8Adef.inc:226    *ABS*:0000000000000001 PWM11
/sources/avr//include/m8Adef.inc:227    *ABS*:0000000000000002 FOC1B
/sources/avr//include/m8Adef.inc:228    *ABS*:0000000000000003 FOC1A
/sources/avr//include/m8Adef.inc:229    *ABS*:0000000000000004 COM1B0
/sources/avr//include/m8Adef.inc:230    *ABS*:0000000000000005 COM1B1
/sources/avr//include/m8Adef.inc:231    *ABS*:0000000000000006 COM1A0
/sources/avr//include/m8Adef.inc:232    *ABS*:0000000000000007 COM1A1
/sources/avr//include/m8Adef.inc:235    *ABS*:0000000000000000 CS10
/sources/avr//include/m8Adef.inc:236    *ABS*:0000000000000001 CS11
/sources/avr//include/m8Adef.inc:237    *ABS*:0000000000000002 CS12
/sources/avr//include/m8Adef.inc:238    *ABS*:0000000000000003 WGM12
/sources/avr//include/m8Adef.inc:239    *ABS*:0000000000000003 CTC10
/sources/avr//include/m8Adef.inc:240    *ABS*:0000000000000003 CTC1
/sources/avr//include/m8Adef.inc:241    *ABS*:0000000000000004 WGM13
/sources/avr//include/m8Adef.inc:242    *ABS*:0000000000000004 CTC11
/sources/avr//include/m8Adef.inc:243    *ABS*:0000000000000006 ICES1
/sources/avr//include/m8Adef.inc:244    *ABS*:0000000000000007 ICNC1
/sources/avr//include/m8Adef.inc:249    *ABS*:0000000000000006 TOIE2
/sources/avr//include/m8Adef.inc:250    *ABS*:0000000000000007 OCIE2
/sources/avr//include/m8Adef.inc:253    *ABS*:0000000000000006 TOV2
/sources/avr//include/m8Adef.inc:254    *ABS*:0000000000000007 OCF2
/sources/avr//include/m8Adef.inc:257    *ABS*:0000000000000000 CS20
/sources/avr//include/m8Adef.inc:258    *ABS*:0000000000000001 CS21
/sources/avr//include/m8Adef.inc:259    *ABS*:0000000000000002 CS22
/sources/avr//include/m8Adef.inc:260    *ABS*:0000000000000003 WGM21
/sources/avr//include/m8Adef.inc:261    *ABS*:0000000000000003 CTC2
/sources/avr//include/m8Adef.inc:262    *ABS*:0000000000000004 COM20
/sources/avr//include/m8Adef.inc:263    *ABS*:0000000000000005 COM21
/sources/avr//include/m8Adef.inc:264    *ABS*:0000000000000006 WGM20
/sources/avr//include/m8Adef.inc:265    *ABS*:0000000000000006 PWM2
/sources/avr//include/m8Adef.inc:266    *ABS*:0000000000000007 FOC2
/sources/avr//include/m8Adef.inc:269    *ABS*:0000000000000000 TCNT2_0
/sources/avr//include/m8Adef.inc:270    *ABS*:0000000000000001 TCNT2_1
/sources/avr//include/m8Adef.inc:271    *ABS*:0000000000000002 TCNT2_2
/sources/avr//include/m8Adef.inc:272    *ABS*:0000000000000003 TCNT2_3
/sources/avr//include/m8Adef.inc:273    *ABS*:0000000000000004 TCNT2_4
/sources/avr//include/m8Adef.inc:274    *ABS*:0000000000000005 TCNT2_5
/sources/avr//include/m8Adef.inc:275    *ABS*:0000000000000006 TCNT2_6
/sources/avr//include/m8Adef.inc:276    *ABS*:0000000000000007 TCNT2_7
/sources/avr//include/m8Adef.inc:279    *ABS*:0000000000000000 OCR2_0
/sources/avr//include/m8Adef.inc:280    *ABS*:0000000000000001 OCR2_1
/sources/avr//include/m8Adef.inc:281    *ABS*:0000000000000002 OCR2_2
/sources/avr//include/m8Adef.inc:282    *ABS*:0000000000000003 OCR2_3
GAS LISTING avr.s 			page 33


/sources/avr//include/m8Adef.inc:283    *ABS*:0000000000000004 OCR2_4
/sources/avr//include/m8Adef.inc:284    *ABS*:0000000000000005 OCR2_5
/sources/avr//include/m8Adef.inc:285    *ABS*:0000000000000006 OCR2_6
/sources/avr//include/m8Adef.inc:286    *ABS*:0000000000000007 OCR2_7
/sources/avr//include/m8Adef.inc:289    *ABS*:0000000000000000 TCR2UB
/sources/avr//include/m8Adef.inc:290    *ABS*:0000000000000001 OCR2UB
/sources/avr//include/m8Adef.inc:291    *ABS*:0000000000000002 TCN2UB
/sources/avr//include/m8Adef.inc:292    *ABS*:0000000000000003 AS2
/sources/avr//include/m8Adef.inc:295    *ABS*:0000000000000001 PSR2
/sources/avr//include/m8Adef.inc:300    *ABS*:0000000000000000 UDR0
/sources/avr//include/m8Adef.inc:301    *ABS*:0000000000000001 UDR1
/sources/avr//include/m8Adef.inc:302    *ABS*:0000000000000002 UDR2
/sources/avr//include/m8Adef.inc:303    *ABS*:0000000000000003 UDR3
/sources/avr//include/m8Adef.inc:304    *ABS*:0000000000000004 UDR4
/sources/avr//include/m8Adef.inc:305    *ABS*:0000000000000005 UDR5
/sources/avr//include/m8Adef.inc:306    *ABS*:0000000000000006 UDR6
/sources/avr//include/m8Adef.inc:307    *ABS*:0000000000000007 UDR7
/sources/avr//include/m8Adef.inc:310    *ABS*:000000000000000b USR
/sources/avr//include/m8Adef.inc:311    *ABS*:0000000000000000 MPCM
/sources/avr//include/m8Adef.inc:312    *ABS*:0000000000000001 U2X
/sources/avr//include/m8Adef.inc:313    *ABS*:0000000000000002 UPE
/sources/avr//include/m8Adef.inc:314    *ABS*:0000000000000002 PE
/sources/avr//include/m8Adef.inc:315    *ABS*:0000000000000003 DOR
/sources/avr//include/m8Adef.inc:316    *ABS*:0000000000000004 FE
/sources/avr//include/m8Adef.inc:317    *ABS*:0000000000000005 UDRE
/sources/avr//include/m8Adef.inc:318    *ABS*:0000000000000006 TXC
/sources/avr//include/m8Adef.inc:319    *ABS*:0000000000000007 RXC
/sources/avr//include/m8Adef.inc:322    *ABS*:000000000000000a UCR
/sources/avr//include/m8Adef.inc:323    *ABS*:0000000000000000 TXB8
/sources/avr//include/m8Adef.inc:324    *ABS*:0000000000000001 RXB8
/sources/avr//include/m8Adef.inc:325    *ABS*:0000000000000002 UCSZ2
/sources/avr//include/m8Adef.inc:326    *ABS*:0000000000000002 CHR9
/sources/avr//include/m8Adef.inc:327    *ABS*:0000000000000003 TXEN
/sources/avr//include/m8Adef.inc:328    *ABS*:0000000000000004 RXEN
/sources/avr//include/m8Adef.inc:329    *ABS*:0000000000000005 UDRIE
/sources/avr//include/m8Adef.inc:330    *ABS*:0000000000000006 TXCIE
/sources/avr//include/m8Adef.inc:331    *ABS*:0000000000000007 RXCIE
/sources/avr//include/m8Adef.inc:334    *ABS*:0000000000000000 UCPOL
/sources/avr//include/m8Adef.inc:335    *ABS*:0000000000000001 UCSZ0
/sources/avr//include/m8Adef.inc:336    *ABS*:0000000000000002 UCSZ1
/sources/avr//include/m8Adef.inc:337    *ABS*:0000000000000003 USBS
/sources/avr//include/m8Adef.inc:338    *ABS*:0000000000000004 UPM0
/sources/avr//include/m8Adef.inc:339    *ABS*:0000000000000005 UPM1
/sources/avr//include/m8Adef.inc:340    *ABS*:0000000000000006 UMSEL
/sources/avr//include/m8Adef.inc:341    *ABS*:0000000000000007 URSEL
/sources/avr//include/m8Adef.inc:343    *ABS*:0000000000000020 UBRRHI
/sources/avr//include/m8Adef.inc:347    *ABS*:0000000000000000 I2BR
/sources/avr//include/m8Adef.inc:348    *ABS*:0000000000000000 TWBR0
/sources/avr//include/m8Adef.inc:349    *ABS*:0000000000000001 TWBR1
/sources/avr//include/m8Adef.inc:350    *ABS*:0000000000000002 TWBR2
/sources/avr//include/m8Adef.inc:351    *ABS*:0000000000000003 TWBR3
/sources/avr//include/m8Adef.inc:352    *ABS*:0000000000000004 TWBR4
/sources/avr//include/m8Adef.inc:353    *ABS*:0000000000000005 TWBR5
/sources/avr//include/m8Adef.inc:354    *ABS*:0000000000000006 TWBR6
/sources/avr//include/m8Adef.inc:355    *ABS*:0000000000000007 TWBR7
/sources/avr//include/m8Adef.inc:358    *ABS*:0000000000000036 I2CR
/sources/avr//include/m8Adef.inc:359    *ABS*:0000000000000000 TWIE
GAS LISTING avr.s 			page 34


/sources/avr//include/m8Adef.inc:360    *ABS*:0000000000000000 I2IE
/sources/avr//include/m8Adef.inc:361    *ABS*:0000000000000002 TWEN
/sources/avr//include/m8Adef.inc:362    *ABS*:0000000000000002 I2EN
/sources/avr//include/m8Adef.inc:363    *ABS*:0000000000000002 ENI2C
/sources/avr//include/m8Adef.inc:364    *ABS*:0000000000000003 TWWC
/sources/avr//include/m8Adef.inc:365    *ABS*:0000000000000003 I2WC
/sources/avr//include/m8Adef.inc:366    *ABS*:0000000000000004 TWSTO
/sources/avr//include/m8Adef.inc:367    *ABS*:0000000000000004 I2STO
/sources/avr//include/m8Adef.inc:368    *ABS*:0000000000000005 TWSTA
/sources/avr//include/m8Adef.inc:369    *ABS*:0000000000000005 I2STA
/sources/avr//include/m8Adef.inc:370    *ABS*:0000000000000006 TWEA
/sources/avr//include/m8Adef.inc:371    *ABS*:0000000000000006 I2EA
/sources/avr//include/m8Adef.inc:372    *ABS*:0000000000000007 TWINT
/sources/avr//include/m8Adef.inc:373    *ABS*:0000000000000007 I2INT
/sources/avr//include/m8Adef.inc:376    *ABS*:0000000000000001 I2SR
/sources/avr//include/m8Adef.inc:377    *ABS*:0000000000000000 TWPS0
/sources/avr//include/m8Adef.inc:378    *ABS*:0000000000000000 TWS0
/sources/avr//include/m8Adef.inc:379    *ABS*:0000000000000000 I2GCE
/sources/avr//include/m8Adef.inc:380    *ABS*:0000000000000001 TWPS1
/sources/avr//include/m8Adef.inc:381    *ABS*:0000000000000001 TWS1
/sources/avr//include/m8Adef.inc:382    *ABS*:0000000000000003 TWS3
/sources/avr//include/m8Adef.inc:383    *ABS*:0000000000000003 I2S3
/sources/avr//include/m8Adef.inc:384    *ABS*:0000000000000004 TWS4
/sources/avr//include/m8Adef.inc:385    *ABS*:0000000000000004 I2S4
/sources/avr//include/m8Adef.inc:386    *ABS*:0000000000000005 TWS5
/sources/avr//include/m8Adef.inc:387    *ABS*:0000000000000005 I2S5
/sources/avr//include/m8Adef.inc:388    *ABS*:0000000000000006 TWS6
/sources/avr//include/m8Adef.inc:389    *ABS*:0000000000000006 I2S6
/sources/avr//include/m8Adef.inc:390    *ABS*:0000000000000007 TWS7
/sources/avr//include/m8Adef.inc:391    *ABS*:0000000000000007 I2S7
/sources/avr//include/m8Adef.inc:394    *ABS*:0000000000000003 I2DR
/sources/avr//include/m8Adef.inc:395    *ABS*:0000000000000000 TWD0
/sources/avr//include/m8Adef.inc:396    *ABS*:0000000000000001 TWD1
/sources/avr//include/m8Adef.inc:397    *ABS*:0000000000000002 TWD2
/sources/avr//include/m8Adef.inc:398    *ABS*:0000000000000003 TWD3
/sources/avr//include/m8Adef.inc:399    *ABS*:0000000000000004 TWD4
/sources/avr//include/m8Adef.inc:400    *ABS*:0000000000000005 TWD5
/sources/avr//include/m8Adef.inc:401    *ABS*:0000000000000006 TWD6
/sources/avr//include/m8Adef.inc:402    *ABS*:0000000000000007 TWD7
/sources/avr//include/m8Adef.inc:405    *ABS*:0000000000000002 I2AR
/sources/avr//include/m8Adef.inc:406    *ABS*:0000000000000000 TWGCE
/sources/avr//include/m8Adef.inc:407    *ABS*:0000000000000001 TWA0
/sources/avr//include/m8Adef.inc:408    *ABS*:0000000000000002 TWA1
/sources/avr//include/m8Adef.inc:409    *ABS*:0000000000000003 TWA2
/sources/avr//include/m8Adef.inc:410    *ABS*:0000000000000004 TWA3
/sources/avr//include/m8Adef.inc:411    *ABS*:0000000000000005 TWA4
/sources/avr//include/m8Adef.inc:412    *ABS*:0000000000000006 TWA5
/sources/avr//include/m8Adef.inc:413    *ABS*:0000000000000007 TWA6
/sources/avr//include/m8Adef.inc:418    *ABS*:0000000000000021 WDTCSR
/sources/avr//include/m8Adef.inc:419    *ABS*:0000000000000000 WDP0
/sources/avr//include/m8Adef.inc:420    *ABS*:0000000000000001 WDP1
/sources/avr//include/m8Adef.inc:421    *ABS*:0000000000000002 WDP2
/sources/avr//include/m8Adef.inc:422    *ABS*:0000000000000003 WDE
/sources/avr//include/m8Adef.inc:423    *ABS*:0000000000000004 WDCE
/sources/avr//include/m8Adef.inc:424    *ABS*:0000000000000004 WDTOE
/sources/avr//include/m8Adef.inc:429    *ABS*:0000000000000000 PORTB0
/sources/avr//include/m8Adef.inc:430    *ABS*:0000000000000000 PB0
GAS LISTING avr.s 			page 35


/sources/avr//include/m8Adef.inc:431    *ABS*:0000000000000001 PORTB1
/sources/avr//include/m8Adef.inc:432    *ABS*:0000000000000001 PB1
/sources/avr//include/m8Adef.inc:433    *ABS*:0000000000000002 PORTB2
/sources/avr//include/m8Adef.inc:434    *ABS*:0000000000000002 PB2
/sources/avr//include/m8Adef.inc:435    *ABS*:0000000000000003 PORTB3
/sources/avr//include/m8Adef.inc:436    *ABS*:0000000000000003 PB3
/sources/avr//include/m8Adef.inc:437    *ABS*:0000000000000004 PORTB4
/sources/avr//include/m8Adef.inc:438    *ABS*:0000000000000004 PB4
/sources/avr//include/m8Adef.inc:439    *ABS*:0000000000000005 PORTB5
/sources/avr//include/m8Adef.inc:440    *ABS*:0000000000000005 PB5
/sources/avr//include/m8Adef.inc:441    *ABS*:0000000000000006 PORTB6
/sources/avr//include/m8Adef.inc:442    *ABS*:0000000000000006 PB6
/sources/avr//include/m8Adef.inc:443    *ABS*:0000000000000007 PORTB7
/sources/avr//include/m8Adef.inc:444    *ABS*:0000000000000007 PB7
/sources/avr//include/m8Adef.inc:447    *ABS*:0000000000000000 DDB0
/sources/avr//include/m8Adef.inc:448    *ABS*:0000000000000001 DDB1
/sources/avr//include/m8Adef.inc:449    *ABS*:0000000000000002 DDB2
/sources/avr//include/m8Adef.inc:450    *ABS*:0000000000000003 DDB3
/sources/avr//include/m8Adef.inc:451    *ABS*:0000000000000004 DDB4
/sources/avr//include/m8Adef.inc:452    *ABS*:0000000000000005 DDB5
/sources/avr//include/m8Adef.inc:453    *ABS*:0000000000000006 DDB6
/sources/avr//include/m8Adef.inc:454    *ABS*:0000000000000007 DDB7
/sources/avr//include/m8Adef.inc:457    *ABS*:0000000000000000 PINB0
/sources/avr//include/m8Adef.inc:458    *ABS*:0000000000000001 PINB1
/sources/avr//include/m8Adef.inc:459    *ABS*:0000000000000002 PINB2
/sources/avr//include/m8Adef.inc:460    *ABS*:0000000000000003 PINB3
/sources/avr//include/m8Adef.inc:461    *ABS*:0000000000000004 PINB4
/sources/avr//include/m8Adef.inc:462    *ABS*:0000000000000005 PINB5
/sources/avr//include/m8Adef.inc:463    *ABS*:0000000000000006 PINB6
/sources/avr//include/m8Adef.inc:464    *ABS*:0000000000000007 PINB7
/sources/avr//include/m8Adef.inc:469    *ABS*:0000000000000000 PORTC0
/sources/avr//include/m8Adef.inc:470    *ABS*:0000000000000000 PC0
/sources/avr//include/m8Adef.inc:471    *ABS*:0000000000000001 PORTC1
/sources/avr//include/m8Adef.inc:472    *ABS*:0000000000000001 PC1
/sources/avr//include/m8Adef.inc:473    *ABS*:0000000000000002 PORTC2
/sources/avr//include/m8Adef.inc:474    *ABS*:0000000000000002 PC2
/sources/avr//include/m8Adef.inc:475    *ABS*:0000000000000003 PORTC3
/sources/avr//include/m8Adef.inc:476    *ABS*:0000000000000003 PC3
/sources/avr//include/m8Adef.inc:477    *ABS*:0000000000000004 PORTC4
/sources/avr//include/m8Adef.inc:478    *ABS*:0000000000000004 PC4
/sources/avr//include/m8Adef.inc:479    *ABS*:0000000000000005 PORTC5
/sources/avr//include/m8Adef.inc:480    *ABS*:0000000000000005 PC5
/sources/avr//include/m8Adef.inc:481    *ABS*:0000000000000006 PORTC6
/sources/avr//include/m8Adef.inc:482    *ABS*:0000000000000006 PC6
/sources/avr//include/m8Adef.inc:485    *ABS*:0000000000000000 DDC0
/sources/avr//include/m8Adef.inc:486    *ABS*:0000000000000001 DDC1
/sources/avr//include/m8Adef.inc:487    *ABS*:0000000000000002 DDC2
/sources/avr//include/m8Adef.inc:488    *ABS*:0000000000000003 DDC3
/sources/avr//include/m8Adef.inc:489    *ABS*:0000000000000004 DDC4
/sources/avr//include/m8Adef.inc:490    *ABS*:0000000000000005 DDC5
/sources/avr//include/m8Adef.inc:491    *ABS*:0000000000000006 DDC6
/sources/avr//include/m8Adef.inc:494    *ABS*:0000000000000000 PINC0
/sources/avr//include/m8Adef.inc:495    *ABS*:0000000000000001 PINC1
/sources/avr//include/m8Adef.inc:496    *ABS*:0000000000000002 PINC2
/sources/avr//include/m8Adef.inc:497    *ABS*:0000000000000003 PINC3
/sources/avr//include/m8Adef.inc:498    *ABS*:0000000000000004 PINC4
/sources/avr//include/m8Adef.inc:499    *ABS*:0000000000000005 PINC5
GAS LISTING avr.s 			page 36


/sources/avr//include/m8Adef.inc:500    *ABS*:0000000000000006 PINC6
/sources/avr//include/m8Adef.inc:505    *ABS*:0000000000000000 PORTD0
/sources/avr//include/m8Adef.inc:506    *ABS*:0000000000000000 PD0
/sources/avr//include/m8Adef.inc:507    *ABS*:0000000000000001 PORTD1
/sources/avr//include/m8Adef.inc:508    *ABS*:0000000000000001 PD1
/sources/avr//include/m8Adef.inc:509    *ABS*:0000000000000002 PORTD2
/sources/avr//include/m8Adef.inc:510    *ABS*:0000000000000002 PD2
/sources/avr//include/m8Adef.inc:511    *ABS*:0000000000000003 PORTD3
/sources/avr//include/m8Adef.inc:512    *ABS*:0000000000000003 PD3
/sources/avr//include/m8Adef.inc:513    *ABS*:0000000000000004 PORTD4
/sources/avr//include/m8Adef.inc:514    *ABS*:0000000000000004 PD4
/sources/avr//include/m8Adef.inc:515    *ABS*:0000000000000005 PORTD5
/sources/avr//include/m8Adef.inc:516    *ABS*:0000000000000005 PD5
/sources/avr//include/m8Adef.inc:517    *ABS*:0000000000000006 PORTD6
/sources/avr//include/m8Adef.inc:518    *ABS*:0000000000000006 PD6
/sources/avr//include/m8Adef.inc:519    *ABS*:0000000000000007 PORTD7
/sources/avr//include/m8Adef.inc:520    *ABS*:0000000000000007 PD7
/sources/avr//include/m8Adef.inc:523    *ABS*:0000000000000000 DDD0
/sources/avr//include/m8Adef.inc:524    *ABS*:0000000000000001 DDD1
/sources/avr//include/m8Adef.inc:525    *ABS*:0000000000000002 DDD2
/sources/avr//include/m8Adef.inc:526    *ABS*:0000000000000003 DDD3
/sources/avr//include/m8Adef.inc:527    *ABS*:0000000000000004 DDD4
/sources/avr//include/m8Adef.inc:528    *ABS*:0000000000000005 DDD5
/sources/avr//include/m8Adef.inc:529    *ABS*:0000000000000006 DDD6
/sources/avr//include/m8Adef.inc:530    *ABS*:0000000000000007 DDD7
/sources/avr//include/m8Adef.inc:533    *ABS*:0000000000000000 PIND0
/sources/avr//include/m8Adef.inc:534    *ABS*:0000000000000001 PIND1
/sources/avr//include/m8Adef.inc:535    *ABS*:0000000000000002 PIND2
/sources/avr//include/m8Adef.inc:536    *ABS*:0000000000000003 PIND3
/sources/avr//include/m8Adef.inc:537    *ABS*:0000000000000004 PIND4
/sources/avr//include/m8Adef.inc:538    *ABS*:0000000000000005 PIND5
/sources/avr//include/m8Adef.inc:539    *ABS*:0000000000000006 PIND6
/sources/avr//include/m8Adef.inc:540    *ABS*:0000000000000007 PIND7
/sources/avr//include/m8Adef.inc:545    *ABS*:0000000000000000 EEDR0
/sources/avr//include/m8Adef.inc:546    *ABS*:0000000000000001 EEDR1
/sources/avr//include/m8Adef.inc:547    *ABS*:0000000000000002 EEDR2
/sources/avr//include/m8Adef.inc:548    *ABS*:0000000000000003 EEDR3
/sources/avr//include/m8Adef.inc:549    *ABS*:0000000000000004 EEDR4
/sources/avr//include/m8Adef.inc:550    *ABS*:0000000000000005 EEDR5
/sources/avr//include/m8Adef.inc:551    *ABS*:0000000000000006 EEDR6
/sources/avr//include/m8Adef.inc:552    *ABS*:0000000000000007 EEDR7
/sources/avr//include/m8Adef.inc:555    *ABS*:0000000000000000 EERE
/sources/avr//include/m8Adef.inc:556    *ABS*:0000000000000001 EEWE
/sources/avr//include/m8Adef.inc:557    *ABS*:0000000000000002 EEMWE
/sources/avr//include/m8Adef.inc:558    *ABS*:0000000000000002 EEWEE
/sources/avr//include/m8Adef.inc:559    *ABS*:0000000000000003 EERIE
/sources/avr//include/m8Adef.inc:564    *ABS*:0000000000000000 SREG_C
/sources/avr//include/m8Adef.inc:565    *ABS*:0000000000000001 SREG_Z
/sources/avr//include/m8Adef.inc:566    *ABS*:0000000000000002 SREG_N
/sources/avr//include/m8Adef.inc:567    *ABS*:0000000000000003 SREG_V
/sources/avr//include/m8Adef.inc:568    *ABS*:0000000000000004 SREG_S
/sources/avr//include/m8Adef.inc:569    *ABS*:0000000000000005 SREG_H
/sources/avr//include/m8Adef.inc:570    *ABS*:0000000000000006 SREG_T
/sources/avr//include/m8Adef.inc:571    *ABS*:0000000000000007 SREG_I
/sources/avr//include/m8Adef.inc:578    *ABS*:0000000000000004 SM0
/sources/avr//include/m8Adef.inc:579    *ABS*:0000000000000005 SM1
/sources/avr//include/m8Adef.inc:580    *ABS*:0000000000000006 SM2
GAS LISTING avr.s 			page 37


/sources/avr//include/m8Adef.inc:581    *ABS*:0000000000000007 SE
/sources/avr//include/m8Adef.inc:584    *ABS*:0000000000000034 MCUSR
/sources/avr//include/m8Adef.inc:585    *ABS*:0000000000000000 PORF
/sources/avr//include/m8Adef.inc:586    *ABS*:0000000000000001 EXTRF
/sources/avr//include/m8Adef.inc:587    *ABS*:0000000000000002 BORF
/sources/avr//include/m8Adef.inc:588    *ABS*:0000000000000003 WDRF
/sources/avr//include/m8Adef.inc:591    *ABS*:0000000000000000 CAL0
/sources/avr//include/m8Adef.inc:592    *ABS*:0000000000000001 CAL1
/sources/avr//include/m8Adef.inc:593    *ABS*:0000000000000002 CAL2
/sources/avr//include/m8Adef.inc:594    *ABS*:0000000000000003 CAL3
/sources/avr//include/m8Adef.inc:595    *ABS*:0000000000000004 CAL4
/sources/avr//include/m8Adef.inc:596    *ABS*:0000000000000005 CAL5
/sources/avr//include/m8Adef.inc:597    *ABS*:0000000000000006 CAL6
/sources/avr//include/m8Adef.inc:598    *ABS*:0000000000000007 CAL7
/sources/avr//include/m8Adef.inc:601    *ABS*:0000000000000000 SPMEN
/sources/avr//include/m8Adef.inc:602    *ABS*:0000000000000001 PGERS
/sources/avr//include/m8Adef.inc:603    *ABS*:0000000000000002 PGWRT
/sources/avr//include/m8Adef.inc:604    *ABS*:0000000000000003 BLBSET
/sources/avr//include/m8Adef.inc:605    *ABS*:0000000000000004 RWWSRE
/sources/avr//include/m8Adef.inc:606    *ABS*:0000000000000006 RWWSB
/sources/avr//include/m8Adef.inc:607    *ABS*:0000000000000007 SPMIE
/sources/avr//include/m8Adef.inc:610    *ABS*:0000000000000000 PSR10
/sources/avr//include/m8Adef.inc:611    *ABS*:0000000000000002 PUD
/sources/avr//include/m8Adef.inc:612    *ABS*:0000000000000004 ADHSM
/sources/avr//include/m8Adef.inc:617    *ABS*:0000000000000000 MUX0
/sources/avr//include/m8Adef.inc:618    *ABS*:0000000000000001 MUX1
/sources/avr//include/m8Adef.inc:619    *ABS*:0000000000000002 MUX2
/sources/avr//include/m8Adef.inc:620    *ABS*:0000000000000003 MUX3
/sources/avr//include/m8Adef.inc:621    *ABS*:0000000000000005 ADLAR
/sources/avr//include/m8Adef.inc:622    *ABS*:0000000000000006 REFS0
/sources/avr//include/m8Adef.inc:623    *ABS*:0000000000000007 REFS1
/sources/avr//include/m8Adef.inc:626    *ABS*:0000000000000006 ADCSR
/sources/avr//include/m8Adef.inc:627    *ABS*:0000000000000000 ADPS0
/sources/avr//include/m8Adef.inc:628    *ABS*:0000000000000001 ADPS1
/sources/avr//include/m8Adef.inc:629    *ABS*:0000000000000002 ADPS2
/sources/avr//include/m8Adef.inc:630    *ABS*:0000000000000003 ADIE
/sources/avr//include/m8Adef.inc:631    *ABS*:0000000000000004 ADIF
/sources/avr//include/m8Adef.inc:632    *ABS*:0000000000000005 ADFR
/sources/avr//include/m8Adef.inc:633    *ABS*:0000000000000006 ADSC
/sources/avr//include/m8Adef.inc:634    *ABS*:0000000000000007 ADEN
/sources/avr//include/m8Adef.inc:639    *ABS*:0000000000000000 LB1
/sources/avr//include/m8Adef.inc:640    *ABS*:0000000000000001 LB2
/sources/avr//include/m8Adef.inc:641    *ABS*:0000000000000002 BLB01
/sources/avr//include/m8Adef.inc:642    *ABS*:0000000000000003 BLB02
/sources/avr//include/m8Adef.inc:643    *ABS*:0000000000000004 BLB11
/sources/avr//include/m8Adef.inc:644    *ABS*:0000000000000005 BLB12
/sources/avr//include/m8Adef.inc:649    *ABS*:0000000000000000 CKSEL0
/sources/avr//include/m8Adef.inc:650    *ABS*:0000000000000001 CKSEL1
/sources/avr//include/m8Adef.inc:651    *ABS*:0000000000000002 CKSEL2
/sources/avr//include/m8Adef.inc:652    *ABS*:0000000000000003 CKSEL3
/sources/avr//include/m8Adef.inc:653    *ABS*:0000000000000004 SUT0
/sources/avr//include/m8Adef.inc:654    *ABS*:0000000000000005 SUT1
/sources/avr//include/m8Adef.inc:655    *ABS*:0000000000000006 BODEN
/sources/avr//include/m8Adef.inc:656    *ABS*:0000000000000007 BODLEVEL
/sources/avr//include/m8Adef.inc:659    *ABS*:0000000000000000 BOOTRST
/sources/avr//include/m8Adef.inc:660    *ABS*:0000000000000001 BOOTSZ0
/sources/avr//include/m8Adef.inc:661    *ABS*:0000000000000002 BOOTSZ1
GAS LISTING avr.s 			page 38


/sources/avr//include/m8Adef.inc:662    *ABS*:0000000000000003 EESAVE
/sources/avr//include/m8Adef.inc:663    *ABS*:0000000000000004 CKOPT
/sources/avr//include/m8Adef.inc:664    *ABS*:0000000000000005 SPIEN
/sources/avr//include/m8Adef.inc:665    *ABS*:0000000000000006 WTDON
/sources/avr//include/m8Adef.inc:666    *ABS*:0000000000000007 RSTDISBL
/sources/avr//include/m8Adef.inc:677    *ABS*:000000000000001b XH
/sources/avr//include/m8Adef.inc:678    *ABS*:000000000000001a XL
/sources/avr//include/m8Adef.inc:679    *ABS*:000000000000001d YH
/sources/avr//include/m8Adef.inc:680    *ABS*:000000000000001c YL
/sources/avr//include/m8Adef.inc:681    *ABS*:000000000000001f ZH
/sources/avr//include/m8Adef.inc:682    *ABS*:000000000000001e ZL
/sources/avr//include/m8Adef.inc:687    *ABS*:0000000000000fff FLASHEND
/sources/avr//include/m8Adef.inc:688    *ABS*:000000000000003f IOEND
/sources/avr//include/m8Adef.inc:689    *ABS*:0000000000000060 SRAM_START
/sources/avr//include/m8Adef.inc:690    *ABS*:0000000000000400 SRAM_SIZE
/sources/avr//include/m8Adef.inc:691    *ABS*:000000000000045f RAMEND
/sources/avr//include/m8Adef.inc:692    *ABS*:0000000000000000 XRAMEND
/sources/avr//include/m8Adef.inc:693    *ABS*:00000000000001ff E2END
/sources/avr//include/m8Adef.inc:694    *ABS*:00000000000001ff EEPROMEND
/sources/avr//include/m8Adef.inc:695    *ABS*:0000000000000009 EEADRBITS
/sources/avr//include/m8Adef.inc:704    *ABS*:0000000000000c00 NRWW_START_ADDR
/sources/avr//include/m8Adef.inc:705    *ABS*:0000000000000fff NRWW_STOP_ADDR
/sources/avr//include/m8Adef.inc:706    *ABS*:0000000000000000 RWW_START_ADDR
/sources/avr//include/m8Adef.inc:707    *ABS*:0000000000000bff RWW_STOP_ADDR
/sources/avr//include/m8Adef.inc:708    *ABS*:0000000000000020 PAGESIZE
/sources/avr//include/m8Adef.inc:709    *ABS*:0000000000000f80 FIRSTBOOTSTART
/sources/avr//include/m8Adef.inc:710    *ABS*:0000000000000f00 SECONDBOOTSTART
/sources/avr//include/m8Adef.inc:711    *ABS*:0000000000000e00 THIRDBOOTSTART
/sources/avr//include/m8Adef.inc:712    *ABS*:0000000000000c00 FOURTHBOOTSTART
/sources/avr//include/m8Adef.inc:713    *ABS*:0000000000000f80 SMALLBOOTSTART
/sources/avr//include/m8Adef.inc:714    *ABS*:0000000000000c00 LARGEBOOTSTART
/sources/avr//include/m8Adef.inc:719    *ABS*:0000000000000001 INT0addr
/sources/avr//include/m8Adef.inc:720    *ABS*:0000000000000002 INT1addr
/sources/avr//include/m8Adef.inc:721    *ABS*:0000000000000003 OC2addr
/sources/avr//include/m8Adef.inc:722    *ABS*:0000000000000004 OVF2addr
/sources/avr//include/m8Adef.inc:723    *ABS*:0000000000000005 ICP1addr
/sources/avr//include/m8Adef.inc:724    *ABS*:0000000000000006 OC1Aaddr
/sources/avr//include/m8Adef.inc:725    *ABS*:0000000000000007 OC1Baddr
/sources/avr//include/m8Adef.inc:726    *ABS*:0000000000000008 OVF1addr
/sources/avr//include/m8Adef.inc:727    *ABS*:0000000000000009 OVF0addr
/sources/avr//include/m8Adef.inc:728    *ABS*:000000000000000a SPIaddr
/sources/avr//include/m8Adef.inc:729    *ABS*:000000000000000b URXCaddr
/sources/avr//include/m8Adef.inc:730    *ABS*:000000000000000c UDREaddr
/sources/avr//include/m8Adef.inc:731    *ABS*:000000000000000d UTXCaddr
/sources/avr//include/m8Adef.inc:732    *ABS*:000000000000000e ADCCaddr
/sources/avr//include/m8Adef.inc:733    *ABS*:000000000000000f ERDYaddr
/sources/avr//include/m8Adef.inc:734    *ABS*:0000000000000010 ACIaddr
/sources/avr//include/m8Adef.inc:735    *ABS*:0000000000000011 TWIaddr
/sources/avr//include/m8Adef.inc:736    *ABS*:0000000000000012 SPMRaddr
/sources/avr//include/m8Adef.inc:738    *ABS*:0000000000000013 INT_VECTORS_SIZE
               avr.s:6      *ABS*:00000000007a1200 XTAL
               avr.s:7      *ABS*:0000000000002580 baudrate
               avr.s:8      *ABS*:0000000000000033 bauddivider
               avr.s:9      *ABS*:0000000000000000 F_JOB
               avr.s:11     *ABS*:0000000000000001 F_RFID
               avr.s:12     *ABS*:0000000000000002 F_ETH
               avr.s:13     *ABS*:0000000000000003 F_RX
GAS LISTING avr.s 			page 39


               avr.s:16     *ABS*:0000000000000000 rx
               avr.s:17     *ABS*:0000000000000001 tx
               avr.s:18     *ABS*:0000000000000000 read
               avr.s:19     *ABS*:0000000000000001 write
               avr.s:21     *ABS*:0000000000000014 FLAGS
               avr.s:225    .data:0000000000000060 TCNT
               avr.s:226    .data:0000000000000062 CTCNT2
               avr.s:227    .data:0000000000000064 CTCNT3
               avr.s:228    .data:0000000000000066 CTCNT4
               avr.s:229    .data:0000000000000068 CTCNT5
               avr.s:252    .data:000000000000006a DOOR_IS_OPENED
               avr.s:253    .data:000000000000006c OPEN_DOOR
               avr.s:254    .data:000000000000006e OPEN_SIGNAL
               avr.s:255    .data:0000000000000070 DENIED_SIGNAL
               avr.s:256    .data:0000000000000072 NET_ACTIVITY
               avr.s:257    .data:0000000000000074 RFID_ACTIVITY
               avr.s:258    .data:0000000000000076 SIGNAL_DELTA
               avr.s:288    .text:0000000000000000 reset
               avr.s:470    .text:000000000000011c asm_main
               avr.s:308    .text:0000000000000020 TIMER0_OVF
               avr.s:364    .text:00000000000000b6 SPI_OK
               avr.s:326    .text:000000000000009e RX_OK
               avr.s:377    .text:00000000000000b8 DRE_OK
               avr.s:376    .text:00000000000000b8 TX_OK
               avr.s:342    .text:00000000000000a0 RX_OK_LIGHT
               avr.s:348    .text:00000000000000aa RX_OK_DARK
               avr.s:381    .text:00000000000000ba uart_init
               avr.s:413    .text:00000000000000da timer0_init
               avr.s:418    .text:00000000000000e6 memory_init
               avr.s:423    .text:00000000000000f0 memory_clr
               avr.s:433    .text:0000000000000100 io_ports_init
               avr.s:490    .text:000000000000012e loop
               avr.s:501    .text:0000000000000140 idle
               avr.s:653    .text:0000000000000218 handle
               avr.s:525    .text:000000000000016a skip_net_activity
               avr.s:707    .text:0000000000000236 clear_tcn
               avr.s:536    .text:0000000000000182 rfid_end
               avr.s:553    .text:00000000000001a4 skip_rfid_activity
               avr.s:611    .text:00000000000001de skip_denied_signal
               avr.s:597    .text:00000000000001c0 denied_delta_inc
               avr.s:598    .text:00000000000001c2 denied_delta_upd
               avr.s:643    .text:000000000000020e skip_open_door
               avr.s:645    .text:000000000000020e button_check
               avr.s:650    .text:0000000000000214 button_end
               avr.s:648    .text:0000000000000212 button_on
               avr.s:716    .text:0000000000000244 data
               avr.s:722    .text:0000000000000244 OP_OPEN

UNDEFINED SYMBOLS
SIGNAL_DELTA_THRESHOLD
main_init
rfid
open_door_script
lan_poll
term_var_info
