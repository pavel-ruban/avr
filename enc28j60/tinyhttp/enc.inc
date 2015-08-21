; ------------------------------------------------

.dseg

rx_status_vector:
rxrdpt:	.byte	2
rxlen:	.byte	2
rxstat:	.byte	2

temp:	.byte	2

; ------------------------------------------------

.cseg

; ------------------------------------------------

; trancieves byte through SPI
; in R16 - byte to transmit
; out R16 - received byte
;spi:
;	out		SPDR,R16
;spi_wait:
;	sbis	SPSR,SPIF
;	rjmp	spi_wait
;	in		R16,SPDR
;	ret

; ------------------------------------------------

; trancieves byte through SPI
; in R16 - byte to transmit
; out R16 - received byte
; uses R18,R19
spi:

	out		USIDR,R16

	ldi		R18,(1<<USIWM0)|(1<<USITC)
	ldi		R19,(1<<USIWM0)|(1<<USITC)|(1<<USICLK)

	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19
	out		USICR,R18
	out		USICR,R19

	in		R16,USIDR

	ret

; ------------------------------------------------

; chip select enc28j60
.macro select_enc
	cbi		ENC_PORT,ENC_SEL
.endm

; ------------------------------------------------

; chip release enc28j60
.macro release_enc
	sbi		ENC_PORT,ENC_SEL
.endm

; ------------------------------------------------

; initializes SPI
.macro init_spi_enc
	
	; setup I/O
	sbi		ENC_DDR,ENC_SEL
	sbi		ENC_DDR,ENC_SCK
	sbi		ENC_DDR,ENC_DI
	cbi		ENC_DDR,ENC_DO
	release_enc

;	; setup SPI
;	sbi		SPSR,SPI2X
;	sbi		SPCR,MSTR
;	sbi		SPCR,SPE

.endm

; ------------------------------------------------

; software reset enc28j60
; uses R16-R19,R24,R25
.macro soft_reset_enc

	; send RESET cmd
	select_enc
	ldi		R16,ENC_CMD_RESET
	rcall	spi
	release_enc

	; wait 1ms while enc28j60 initializes
	wait_us	1000

.endm

; ------------------------------------------------

; read control register from enc28j60
; in R16 - regaddr
; out R16 - data from reg.
; uses R17-R19
enc_rcr:
	select_enc
	mov		R17,R16
	andi	R16,0x1f
	rcall	spi
	ser		R16
	rcall	spi
	sbrc	R17,7
	rcall	spi
	release_enc
	ret

; ------------------------------------------------

; write operation on enc28j60 control register 
; in R16 - write command
; in R17 - data to write
; uses R16,R18,R19
enc_wr_op:
	select_enc
	rcall	spi
	mov		R16,R17
	rcall	spi
	release_enc
	ret

; ------------------------------------------------

; write enc28j60 control register
; arg 0 - register address
; arg 1 - data to write
; uses R16-R19
.macro enc_wcr
	ldi		R16,ENC_CMD_WCR|(@0&0x1f)
	ldi		R17,@1
	rcall	enc_wr_op
.endm

; ------------------------------------------------

; set bits in enc28j60 control register
; arg 0 - register address
; arg 1 - data to write
; uses R16-R19
.macro enc_bfs
	ldi		R16,ENC_CMD_BFS|(@0&0x1f)
	ldi		R17,@1
	rcall	enc_wr_op
.endm

; ------------------------------------------------

; clear bits in enc28j60 control register
; arg 0 - register address
; arg 1 - data to write
; uses R16-R19
.macro enc_bfc
	ldi		R16,ENC_CMD_BFC|(@0&0x1f)
	ldi		R17,@1
	rcall	enc_wr_op
.endm

; ------------------------------------------------

; select enc28j60 register bank
; in R16 - bank
; uses R16-R19
enc_bank:
	mov		R17,R16
	ldi		R16,ENC_CMD_BFS|(ECON1&0x1f)
	rcall	enc_wr_op
	ldi		R16,ENC_CMD_BFC|(ECON1&0x1f)
	com		R17
	andi	R17,3
	rcall	enc_wr_op
	ret

.macro enc_bank_sel
	ldi		R16,@0
	rcall	enc_bank
.endm

; ------------------------------------------------

; write phy register
; in R16 - register addr
; in Y - data to write
; uses R16-R20
enc_write_phy:

	mov		R20,R16
	
	enc_bank_sel 2

	; MIREGADR = R16
	mov		R17,R20
	ldi		R16,ENC_CMD_WCR|(0x1f&MIREGADR)
	rcall	enc_wr_op

	; MIWR = Y
	ldi		R16,ENC_CMD_WCR|(0x1f&MIWRL)
	mov		R17,YL
	rcall	enc_wr_op

	ldi		R16,ENC_CMD_WCR|(0x1f&MIWRH)
	mov		R17,YH
	rcall	enc_wr_op

	; loop while MII busy
	enc_bank_sel 3

write_phy_wait:
	ldi		R16,MISTAT
	rcall	enc_rcr
	sbrc	R16,0
	rjmp	write_phy_wait

	ret

; ------------------------------------------------

; read data from enc28j60's buffer
; in R16 - byte count to read
; in Z - buffer to save data to
; uses R17
enc_read_mem:
	select_enc
	mov		R17,R16
	ldi		R16,ENC_CMD_RBM
	rcall	spi
enc_read_mem_loop:
	ser		R16
	rcall	spi
	st		Z+,R16
	dec		R17
	brne	enc_read_mem_loop
	release_enc
	ret

; ------------------------------------------------

; write data to enc28j60's buffer
; in R16 - byte count to write
; in Z - buffer to write
; uses R17
enc_write_mem:
	select_enc
	mov		R17,R16
	ldi		R16,ENC_CMD_WBM
	rcall	spi
enc_write_mem_loop:
	ld		R16,Z+
	rcall	spi
	dec		R17
	brne	enc_write_mem_loop
	release_enc
	ret

; ------------------------------------------------

; write zero-term string to enc28j60's buffer
; in Z - buffer to write (flash)
; uses R16,R18,R19
enc_write_string:
	select_enc
	ldi		R16,ENC_CMD_WBM
	rcall	spi
enc_write_string_loop:
	lpm		R16,Z+
	tst		R16
	breq	enc_write_string_loop_exit
	rcall	spi
	rjmp	enc_write_string_loop
enc_write_string_loop_exit:
	release_enc
	ret

; ------------------------------------------------

; set ERDPT
; in Z - rdptr
; uses R17-R19
enc_set_rdptr:
	enc_bank_sel 0
	ldi		R16,ENC_CMD_WCR|(0x1f&ERDPTL)
	mov		R17,ZL
	rcall	enc_wr_op
	ldi		R16,ENC_CMD_WCR|(0x1f&ERDPTH)
	mov		R17,ZH
	rcall	enc_wr_op
	ret

; ------------------------------------------------

; read ERDPT
; out Z - rdptr
; uses R16-R19
enc_get_rdptr:
	enc_bank_sel 0
	ldi		R16,ERDPTL
	rcall	enc_rcr
	mov		ZL,R16
	ldi		R16,ERDPTH
	rcall	enc_rcr
	mov		ZH,R16
	ret

; ------------------------------------------------

; set EWRPT
; in Z - wrptr
; trashes R17-R19
enc_set_wrptr:
	enc_bank_sel 0
	ldi		R16,ENC_CMD_WCR|(0x1f&EWRPTL)
	mov		R17,ZL
	rcall	enc_wr_op
	ldi		R16,ENC_CMD_WCR|(0x1f&EWRPTH)
	mov		R17,ZH
	rcall	enc_wr_op
	ret

; ------------------------------------------------

; read EWRPT
; out Z - wrptr
; uses R16-R19
enc_get_wrptr:
	enc_bank_sel 0
	ldi		R16,EWRPTL
	rcall	enc_rcr
	mov		ZL,R16
	ldi		R16,EWRPTH
	rcall	enc_rcr
	mov		ZH,R16
	ret

; ------------------------------------------------

; calculates checksum for IP/UDP/TCP/ICMP/etc.
; in Y (ptr in enc28j60's buffer)
; in R25:R24 - block size
; in R21:R20 - checksum iv
; out Y - checksum
; uses R14-R25,Z
enc_checksum:

	clr		R23
	clr		R22

	; save pointer
	rcall	enc_get_rdptr
	movw	R15:R14,Z

	; set pointer
	movw	Z,Y
	rcall	enc_set_rdptr


	; sum 2-byte blocks
checksum_loop1:

	ldi		R16,2			; if(R25:R24 < 2) break;
	clr		R17
	cp		R24,R16
	cpc		R25,R17
	brlo	checksum_loop1_end

	ldi		R16,2			; read word
	ldi		ZL,low(temp)
	ldi		ZH,high(temp)
	rcall	enc_read_mem

	lds		ZH,temp+0
	lds		ZL,temp+1

	clr		R16				; add to checksum
	add		R20,ZL
	adc		R21,ZH
	adc		R22,R16
	adc		R23,R16

	sbiw	R25:R24,2
	rjmp	checksum_loop1

checksum_loop1_end:

	
	; add last byte
	tst		R24
	breq	checksum_not_odd

	ldi		R16,1			; read byte
	ldi		ZL,low(temp)
	ldi		ZH,high(temp)
	rcall	enc_read_mem
	lds		ZH,temp

	clr		R16				; add to checksum
	add		R21,ZH
	adc		R22,R16
	adc		R23,R16

checksum_not_odd:


	; pack checksum
checksum_loop2:
	clr		R16						; while chesksum >= 0x 0001 0000
	cpi		R22,1
	cpc		R23,R16
	brlo	checksum_loop2_end

	movw	Z,R23:R22				; Z = checksum>>16
	
	clr		R23						; checksum &= 0xffff
	clr		R22

	clr		R16						; checksum += Z
	add		R20,ZL
	adc		R21,ZH
	adc		R22,R16
	adc		R23,R16

	rjmp	checksum_loop2

checksum_loop2_end:


	; restore pointer
	movw	Z,R15:R14
	rcall	enc_set_rdptr


	; Y = ~checksum
	com		R20
	com		R21
	movw	Y,R21:R20

	ret

; ------------------------------------------------

; receive packet
; out Y - packet length
;	(zero - no packet received)
; uses R16-R19,Z
enc_recv_start:

	clr		YL
	clr		YH
	
	; if no packet received goto exit
	enc_bank_sel 1
	ldi		R16,EPKTCNT
	rcall	enc_rcr

	tst		R16
	breq	enc_recv_start_no_packet


	; set pointer to start of packet
	lds		ZL,rxrdpt+0
	lds		ZH,rxrdpt+1
	rcall	enc_set_rdptr


	; read status vector
	ldi		R16,6
	ldi		ZL,low(rx_status_vector)
	ldi		ZH,high(rx_status_vector)
	rcall	enc_read_mem

	
	; if(rxstat & 0x80) Y = rxlen;
	lds		R16,rxstat
	sbrs	R16,7
	rjmp	enc_recv_start_pktdec

	lds		YL,rxlen
	lds		YH,rxlen+1

enc_recv_start_pktdec:
	enc_bfs	ECON2,ECON2_PKTDEC	

enc_recv_start_no_packet:
	ret

; ------------------------------------------------

; free buffer memory from recived packet
; uses R16-R19,Z
enc_recv_done:

	; Z = (rxrdpt - 1) & 0x1fff
	lds		ZL,rxrdpt
	lds		ZH,rxrdpt+1
	sbiw	Z,1
	andi	ZL,0xff
	andi	ZH,0x1f


	; ERXRDPT = Z
	enc_bank_sel 0

	ldi		R16,ENC_CMD_WCR|(0x1f&ERXRDPTL)
	mov		R17,ZL
	rcall	enc_wr_op

	ldi		R16,ENC_CMD_WCR|(0x1f&ERXRDPTH)
	mov		R17,ZH
	rcall	enc_wr_op

	ret

; ------------------------------------------------

; prepares buffer to send packet
; uses R16-R19
enc_send_start:

	; EWRPT = TX_START
	ldi		ZL,low(TX_START)
	ldi		ZH,high(TX_START)
	rcall	enc_set_wrptr

	; write 0
	ldi		R16,ENC_CMD_WBM
	clr		R17
	rcall	enc_wr_op

	ret

; ------------------------------------------------

; sends out packet
; trashes R16,R17,R18,R19,Z
enc_send_done:
	
	; ETXND = (EWRPT - 1) & 0x1fff
	rcall	enc_get_wrptr

	sbiw	Z,1
	andi	ZL,0xff
	andi	ZH,0x1f

	; write ETXND 
	ldi		R16,ENC_CMD_WCR|(0x1f&ETXNDL)
	mov		R17,ZL
	rcall	enc_wr_op

	ldi		R16,ENC_CMD_WCR|(0x1f&ETXNDH)
	mov		R17,ZH
	rcall	enc_wr_op

	
	; set ECON1.TXRTS
	enc_bfs ECON1,ECON1_TXRTS
	

	; wait until tx ready
wait_tx_rdy:

	; if(!(ECON1 & TXRTS)) goto ready;
	ldi		R16,ECON1
	rcall	enc_rcr
	sbrs	R16,3;econ1.txrts
	rjmp	tx_rdy

	; if(!(EIR & EIR_TXERIF)) continue;
	ldi		R16,EIR
	rcall	enc_rcr
	sbrs	R16,1;eir.txerif
	rjmp	wait_tx_rdy

	; tx reset
	enc_bfs	ECON1,ECON1_TXRST
	enc_bfc	ECON1,ECON1_TXRST
	rjmp	wait_tx_rdy

tx_rdy:

	ret

; ------------------------------------------------

; initialize enc28j60
; trashes R16-R25,Y,Z
init_enc:

	; reset enc28j60
	init_spi_enc
	soft_reset_enc
	
	; load control registers
	ldi		ZL,low(enc_init_seq*2)
	ldi		ZH,high(enc_init_seq*2)
	ldi		R22,enc_init_seq_end-enc_init_seq

init_enc_loop:
	lpm		R16,Z+
	lpm		R17,Z+
	rcall	enc_wr_op
	dec		R22
	brne	init_enc_loop

	
	; load PHY registers
	ldi		ZL,low(enc_phy_init_seq*2)
	ldi		ZH,high(enc_phy_init_seq*2)
	ldi		R22,(enc_phy_init_seq_end-enc_phy_init_seq)/2

init_phy_loop:
	lpm		R16,Z+
	lpm		R17,Z+
	lpm		YL,Z+
	lpm		YH,Z+
	rcall	enc_write_phy
	dec		R22
	brne	init_phy_loop

	
	; zero status vector
	ldi		R16,6
	clr		R17
	ldi		YL,low(rx_status_vector)
	ldi		YH,high(rx_status_vector)
clear_status_vector:
	st		Y+,R17
	dec		R16
	brne	clear_status_vector

	
	; enable packet reception
	enc_bfs	ECON1,ECON1_RXEN

	ret

; ------------------------------------------------

enc_init_seq:
	
	; Select bank 0
	.db ENC_CMD_BFC|(0x1f&ECON1), 3

	; Bank0.ERXST = 0
	.db ENC_CMD_WCR|(0x1f&ERXSTL), 0
	.db ENC_CMD_WCR|(0x1f&ERXSTH), 0

	; Bank0.ERXRDPT = 0
	.db ENC_CMD_WCR|(0x1f&ERXRDPTL), 0
	.db ENC_CMD_WCR|(0x1f&ERXRDPTH), 0

	; Bank0.ERXND = RX_END
	.db	ENC_CMD_WCR|(0x1f&ERXNDL),low(RX_END)
	.db	ENC_CMD_WCR|(0x1f&ERXNDH),high(RX_END)

	; Bank0.ETXST = TX_START
	.db ENC_CMD_WCR|(0x1f&ETXSTL),low(TX_START)
	.db ENC_CMD_WCR|(0x1f&ETXSTH),high(TX_START)

	
	; Select bank 2
	.db ENC_CMD_BFS|(0x1f&ECON1), 2

	; Bank2.MACON[1-3]
	.db ENC_CMD_WCR|(0x1f&MACON1), MACON1_TXPAUS|MACON1_RXPAUS|MACON1_MARXEN
	.db ENC_CMD_WCR|(0x1f&MACON2), 0
	.db ENC_CMD_WCR|(0x1f&MACON3), MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX

	; Bank2.MAMXFL = 1518
	.db ENC_CMD_WCR|(0x1f&MAMXFLL), low(1518)
	.db ENC_CMD_WCR|(0x1f&MAMXFLH), high(1518)

	; Bank2.MABBIPG = 0x15
	.db ENC_CMD_WCR|(0x1f&MABBIPG), 0x15

	; Bank2.MAIPGL = 0x12
	.db ENC_CMD_WCR|(0x1f&MAIPGL), 0x12
	
	; Bank2.MAIPGH = 0x0c
	.db ENC_CMD_WCR|(0x1f&MAIPGH), 0x0c

	
	; Select bank 3
	.db ENC_CMD_BFS|(0x1f&ECON1), 3

	; MAC Address
	.db ENC_CMD_WCR|(0x1f&MAADR5), MAC0
	.db ENC_CMD_WCR|(0x1f&MAADR4), MAC1
	.db ENC_CMD_WCR|(0x1f&MAADR3), MAC2
	.db ENC_CMD_WCR|(0x1f&MAADR2), MAC3
	.db ENC_CMD_WCR|(0x1f&MAADR1), MAC4
	.db ENC_CMD_WCR|(0x1f&MAADR0), MAC5
enc_init_seq_end:

; ------------------------------------------------

enc_phy_init_seq:
	
	; PHCON1.PDPXMD = 1
	.dw	PHCON1
	.dw PHCON1_PDPXMD

	; PHCON2.HDLDIS = 1
	.dw PHCON2
	.dw PHCON2_HDLDIS

	; PHLCON init
	.dw PHLCON
	.dw PHLCON_LACFG2 |\
		PHLCON_LBCFG2|PHLCON_LBCFG1|PHLCON_LBCFG0 |\
		PHLCON_LFRQ0|PHLCON_STRCH
enc_phy_init_seq_end:

; ------------------------------------------------
