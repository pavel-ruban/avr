; ------------------------------------------------

.include "tn2313def.inc"
.include "config.h"

; ------------------------------------------------

.equ F_CPU			= 16000000

; ------------------------------------------------

; delay 5 to 3000us
; arg 0 = latency in us
; sets to zero R24, R25
.macro wait_us
	ldi		R24,low(F_CPU*@0/4000000)
	ldi		R25,high(F_CPU*@0/4000000)
wait_us_loop:
	sbiw	R25:R24,1
	brne	wait_us_loop
.endm

; ------------------------------------------------

; code
.cseg

; ------------------------------------------------

; interrupt vector table
.org 0
	rjmp	reset

; ------------------------------------------------

; main program start
.org INT_VECTORS_SIZE

; ------------------------------------------------

; reset vector
reset:
	
	; init stack
	ldi		R16,low(RAMEND)
	out		SPL,R16
	;ldi	R16,high(RAMEND)
	;out	SPH,R16

	ldi		R16,(0xff>>(8-DATA_BITS))
	out		DATA_DDR,R16

	; init
	rcall	init_enc

	sbi		PORTB,0
	sbi		PORTB,1


	; catch packets
mainloop:
	rcall	poll_ip
	rjmp	mainloop

; ------------------------------------------------

; additional modules
.include "net.h"
.include "enc.h"
.include "enc.inc"
.include "ip.inc"
.include "http.inc"

; ------------------------------------------------
