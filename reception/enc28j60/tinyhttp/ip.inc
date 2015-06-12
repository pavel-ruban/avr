; ------------------------------------------------

.dseg

pbuf:	.byte	PBUF_SZ

; ------------------------------------------------

.cseg

; ------------------------------------------------

; process tcp packet
; in X = pbuf
; in R25:R24 = tcp plen
process_tcp:

	; save tcp packet length
	movw	R15:R14,R25:R24

	
	; read TCP header
	ldi		R16,TCP_SIZE
	movw	Z,X
	rcall	enc_read_mem

	
	; swap srcport/dstport
	movw	Y,X
	ldd		R23,Y+TCP_SRCPORT+0
	ldd		R22,Y+TCP_SRCPORT+1
	ldd		R25,Y+TCP_DSTPORT+0
	ldd		R24,Y+TCP_DSTPORT+1
	std		Y+TCP_SRCPORT+0,R25
	std		Y+TCP_SRCPORT+1,R24
	std		Y+TCP_DSTPORT+0,R23
	std		Y+TCP_DSTPORT+1,R22

	
	; check dstport
	ldi		R17,high(TCP_PORT)
	cpi		R24,low(TCP_PORT)
	cpc		R25,R17
	breq	tcp_port_opened
	ret

tcp_port_opened:

	; R16 = (calculate tcp header length)
	ldd		R16,Y+TCP_DO	; R16 = (tcp.do & 0xf0) >> 2
	andi	R16,0xf0
	lsr		R16
	lsr		R16
	
	mov		R12,R16			; R13:R12 = packet header length
	clr		R13

	
	; R11 = tcp.flags
	ldd		R11,Y+TCP_FLAGS

	
	; step up seq/ack
	adiw	Y,TCP_SEQNUM
	ld		R25,Y+			; R25:R24:R23:R22 = tcp.seq
	ld		R24,Y+
	ld		R23,Y+
	ld		R22,Y+
	ld		R21,Y+			; R21:R20:R19:R18 = tcp.ack
	ld		R20,Y+
	ld		R19,Y+
	ld		R18,Y+

	movw	Y,R15:R14		; Y = (calculate tcp data length)
	sub		YL,R12
	sbc		YH,R13

	mov		R16,R11			; if (syn or ack is set) Y++;
	andi	R16,(1<<SYN)|(1<<FIN)
	breq	not_fin_not_syn
	adiw	Y,1
not_fin_not_syn:

	clr		R16				; make ack pointer
	add		R22,YL
	adc		R23,YH
	adc		R24,R16
	adc		R25,R16

	movw	Y,X
	adiw	Y,TCP_SEQNUM
	st		Y+,R21			; tcp.seq = R21:R20:R19:R18
	st		Y+,R20
	st		Y+,R19
	st		Y+,R18
	st		Y+,R25			; tcp.ack = R25:R24:R23:R22
	st		Y+,R24
	st		Y+,R23
	st		Y+,R22

	
	; set tcp.hdr_len
	movw	Y,X
	ldi		R16,TCP_SIZE<<2
	std		Y+TCP_DO,R16

	; set dummy windows size
	ser		R16
	std		Y+TCP_WINDOW+0,R16
	std		Y+TCP_WINDOW+1,R16

	; clear checksum
	clr		R16
	std		Y+TCP_CHECKSUM+0,R16
	std		Y+TCP_CHECKSUM+1,R16

	; clear urgent
	std		Y+TCP_URGENT+0,R16
	std		Y+TCP_URGENT+1,R16

	
	; SYN is set? - add MAX_SS option
	mov		R16,R11
	andi	R16,(1<<SYN)
	breq	no_mss_option

	movw	Y,X

	ldi		R16,(TCP_SIZE+4)<<2		; header size with options
	std		Y+TCP_DO,R16

	adiw	Y,TCP_SIZE				; add MSS option
	ldi		R16,2
	st		Y+,R16
	ldi		R16,4
	st		Y+,R16
	ldi		R16,high(556)
	st		Y+,R16
	ldi		R16,low(556)
	st		Y+,R16

	ldi		R16,TCP_SIZE+4
	rjmp	with_mss_option


	; write tcp header
no_mss_option:
	ldi		R16,TCP_SIZE
with_mss_option:
	movw	Z,X
	rcall	enc_write_mem

	
	; skip tcp options
	mov		R16,R12
	ldi		R17,TCP_SIZE
	sub		R16,R17
	breq	no_tcp_options
	movw	Z,X
	rcall	enc_read_mem
no_tcp_options:


	; SYN is set? - reply with SYN/ACK
	sbrs	R11,SYN
	breq	tcp_not_syn

	ldi		R16,(1<<SYN)|(1<<ACK)
	mov		R11,R16

	rjmp	tcp_send_out
tcp_not_syn:

	
	; FIN is set? - reply with ACK
	sbrs	R11,FIN
	breq	tcp_not_fin

	ldi		R16,(1<<ACK);|(1<<FIN)
	mov		R11,R16

	rjmp	tcp_send_out
tcp_not_fin:
	
	
	; ACK is set (no SYN, no FIN)?
	sbrs	R11,ACK
	breq	process_tcp_exit
	sub		R14,R12
	sbc		R15,R13
	breq	process_tcp_exit

	ldi		R16,1<<ACK
	mov		R11,R16

	rcall	tcp_data			; app callback

tcp_send_out:
	rcall	update_tcp
	rcall	update_ip
	rcall	enc_send_done

process_tcp_exit:
	ret

; ------------------------------------------------

; update tcp checksum and flags
; in R11 - tcp flags
update_tcp:

	; save pointer
	rcall	enc_get_wrptr
	movw	R13:R12,Z

	
	; write tcp.flags
	ldi		ZL,low(TX_BASE+ETH_SIZE+IP_SIZE+TCP_FLAGS)
	ldi		ZH,high(TX_BASE+ETH_SIZE+IP_SIZE+TCP_FLAGS)
	rcall	enc_set_wrptr

	ldi		R16,1
	movw	Z,X
	st		Z,R11
	rcall	enc_write_mem

	
	; R25:R24 = (calculate tcp packet length)
	movw	R25:R24,R13:R12
	ldi		R16,low(TX_BASE+ETH_SIZE+IP_SIZE)
	ldi		R17,high(TX_BASE+ETH_SIZE+IP_SIZE)
	sub		R24,R16
	sbc		R25,R17
	
	; Y = (calculate tcp checksum)
	ldi		YL,low(TX_BASE+ETH_SIZE+IP_SIZE-8)		; pointer
	ldi		YH,high(TX_BASE+ETH_SIZE+IP_SIZE-8)

	movw	R21:R20,R25:R24					; iv
	ldi		R16,IP_PROTO_TCP
	clr		R17
	add		R20,R16
	adc		R21,R17

	adiw	R25:R24,8						; size

	rcall	enc_checksum

	; write tcp.checksum
	ldi		ZL,low(TX_BASE+ETH_SIZE+IP_SIZE+TCP_CHECKSUM)
	ldi		ZH,high(TX_BASE+ETH_SIZE+IP_SIZE+TCP_CHECKSUM)
	rcall	enc_set_wrptr

	ldi		R16,2
	movw	Z,X
	std		Z+0,YH
	std		Z+1,YL
	rcall	enc_write_mem


	; restore pointer
	movw	Z,R13:R12
	rcall	enc_set_wrptr

	ret

; ------------------------------------------------

; process udp packet
; X = pbuf
; R25:R24 = len
;process_udp:
;	ret

; ------------------------------------------------

; process icmp packet
; X = pbuf
; R25:R24 = len
process_icmp:

	; read ICMP header
	ldi		R16,ICMP_SIZE
	movw	Z,X
	rcall	enc_read_mem

	; icmp.type = echo request?
	movw	Y,X
	ldd		R16,Y+ICMP_TYPE
	cpi		R16,ICMP_ECHO_REQ
	brne	process_icmp_exit

	; icmp.type = echo reply
	ldi		R16,ICMP_ECHO_REPLY
	std		Y+ICMP_TYPE,R16

	; update ICMP chekcsum
	ldd		ZL,Y+ICMP_CHECKSUM+0
	ldd		ZH,Y+ICMP_CHECKSUM+1
	adiw	Z,8
	std		Y+ICMP_CHECKSUM+0,ZL
	std		Y+ICMP_CHECKSUM+1,ZH

	; write icmp header
	ldi		R16,ICMP_SIZE
	movw	Z,X
	rcall	enc_write_mem

	; Y = ICMP data size
	movw	Y,R25:R24
	sbiw	Y,ICMP_SIZE
	brlo	process_icmp_exit
	
	; copy ICMP data
	ldi		R20,PBUF_SZ			; max block size = 64
	clr		R21

icmp_copy_loop:

	sbiw	Y,0					; if(!Y) goto done;
	breq	icmp_copy_done

	movw	R23:R22,Y			; temp = Y

	cp		R22,R20				; if(temp>64) temp = 64
	cpc		R23,R21
	brlo	icmp_copy_full
	movw	R23:R22,R21:R20
icmp_copy_full:

	mov		R16,R22				; read block
	movw	Z,X
	rcall	enc_read_mem

	mov		R16,R22				; write block
	movw	Z,X
	rcall	enc_write_mem

	sub		YL,R22				; Y -= temp
	sbc		YH,R23

	rjmp	icmp_copy_loop

icmp_copy_done:

	; update ip header
	rcall	update_ip

	; send packet
	rcall	enc_send_done

process_icmp_exit:
	ret

; ------------------------------------------------

; update ip header totlen and checksum
; trashes R12-R25,Y,Z
update_ip:

	; save pointer
	rcall	enc_get_wrptr
	movw	R13:R12,Z


	; R21:R20 = (calculate ip totlen)
	movw	R21:R20,Z
	ldi		R24,low(TX_BASE+ETH_SIZE)
	ldi		R25,high(TX_BASE+ETH_SIZE)
	sub		R20,R24
	sbc		R21,R25

	; write ip.totlen
	ldi		ZL,low(TX_BASE+ETH_SIZE+IP_LENGTH)
	ldi		ZH,high(TX_BASE+ETH_SIZE+IP_LENGTH)
	rcall	enc_set_wrptr

	ldi		R16,2
	movw	Z,X
	std		Z+0,R21
	std		Z+1,R20
	rcall	enc_write_mem


	; Y = (calculate ip checksum)
	ldi		YL,low(TX_BASE+ETH_SIZE)	; start
	ldi		YH,high(TX_BASE+ETH_SIZE)
	ldi		R24,IP_SIZE					; size
	clr		R25
	clr		R20							; iv
	clr		R21
	rcall	enc_checksum

	; write ip.checksum
	ldi		ZL,low(TX_BASE+ETH_SIZE+IP_HCS)
	ldi		ZH,high(TX_BASE+ETH_SIZE+IP_HCS)
	rcall	enc_set_wrptr

	ldi		R16,2
	movw	Z,X
	std		Z+0,YH
	std		Z+1,YL
	rcall	enc_write_mem


	; restore pointer
	movw	Z,R13:R12
	rcall	enc_set_wrptr

	ret

; ------------------------------------------------

; process ip packet
; X = pbuf
process_ip:

	; read ip header
	ldi		R16,IP_SIZE
	movw	Z,X
	rcall	enc_read_mem

	; check ip header
	movw	Y,X
	
	ldd		R16,Y+IP_VER_HLEN	; check ip.ver_hlen
	cpi		R16,0x45
	brne	process_ip_exit

	
	; prepare ip header
	ldi		R16,4
	ldi		ZL,low(myip*2)
	ldi		ZH,high(myip*2)
prepare_ip:

	ldd		R17,Y+IP_DST		; if(ip.dst != myip) goto exit;
	lpm		R18,Z+
	cp		R17,R18
	brne	process_ip_exit

	ldd		R17,Y+IP_SRC
	std		Y+IP_DST,R17		; ip.dst = ip.src

	std		Y+IP_SRC,R18		; ip.src = myip
	
	adiw	Y,1
	dec		R16
	brne	prepare_ip


	; ip.tos = 0
	movw	Y,X
	clr		R16
	std		Y+IP_TOS,R16

	; ip.id = 0
	std		Y+IP_ID+0,R16
	std		Y+IP_ID+1,R16

	; ip.fragment = 0
	std		Y+IP_FLAGS_FO+0,R16
	std		Y+IP_FLAGS_FO+1,R16

	; ip.checksum = 0
	std		Y+IP_HCS+0,R16
	std		Y+IP_HCS+1,R16

	; ip.ttl = 64
	ldi		R16,IP_TTLVALUE
	std		Y+IP_TTL,R16


	; write ip header
	ldi		R16,IP_SIZE
	movw	Z,X
	rcall	enc_write_mem

	
	; packet size
	movw	Y,X
	ldd		R25,Y+IP_LENGTH+0
	ldd		R24,Y+IP_LENGTH+1
	sbiw	R25:R24,IP_SIZE
	brlo	process_ip_exit

	
	; R16 = ip.proto
	ldd		R16,Y+IP_PROTO

	; ip.proto = ICMP?
	cpi		R16,IP_PROTO_ICMP
	brne	not_icmp
	rcall	process_icmp
	rjmp	process_ip_exit
not_icmp:

	; ip.proto = UDP?
;	cpi		R16,IP_PROTO_UDP
;	brne	not_udp
;	rcall	process_udp
;	rjmp	process_ip_exit
;not_udp:

	; ip.proto = TCP?
	cpi		R16,IP_PROTO_TCP
	brne	not_tcp
	rcall	process_tcp
;	rjmp	process_ip_exit
not_tcp:


process_ip_exit:
	ret

; ------------------------------------------------

; process arp packet
; X = pbuf
process_arp:

	; read arp packet
	ldi		R16,ARP_SIZE
	movw	Z,X
	rcall	enc_read_mem

	movw	Y,X

	; arp.ptype = IPv4?
	ldd		R17,Y+ARP_PTYPE+0
	ldd		R16,Y+ARP_PTYPE+1
	ldi		R18,high(ETHTYPE_IP)
	cpi		R16,low(ETHTYPE_IP)
	cpc		R17,R18
	brne	process_arp_exit

	; arp.oper = request?
	ldd		R16,Y+ARP_OPER+1
	cpi		R16,ARP_REQUEST
	brne	process_arp_exit

	
	; prepare arp packet
	ldi		R16,ARP_REPLY
	std		Y+ARP_OPER+1,R16	; arp.oper = reply

	; prepare arp packet (ip)
	ldi		R16,4
	ldi		ZL,low(myip*2)
	ldi		ZH,high(myip*2)
arp_prepare:
	ldd		R17,Y+ARP_TPA		; if(arp.tpa != myip) goto exit;
	lpm		R18,Z+
	cp		R17,R18
	brne	process_arp_exit

	ldd		R17,Y+ARP_SPA		; arp.tpa = arp.spa
	std		Y+ARP_TPA,R17

	std		Y+ARP_SPA,R18		; arp.spa = myip

	adiw	Y,1
	dec		R16
	brne	arp_prepare

	; prepare arp packet (mac)
	movw	Y,X
	ldi		R16,6
	ldi		ZL,low(mymac*2)
	ldi		ZH,high(mymac*2)
arp_prepare_mac:
	
	ldd		R17,Y+ARP_SHA
	std		Y+ARP_THA,R17		; arp.tha = arp.sha

	lpm		R17,Z+
	std		Y+ARP_SHA,R17		; arp.sha = mymac

	adiw	Y,1
	dec		R16
	brne	arp_prepare_mac

	; write arp packet
	ldi		R16,ARP_SIZE
	movw	Z,X
	rcall	enc_write_mem

	; send arp reply
	rcall	enc_send_done


process_arp_exit:
	ret

; ------------------------------------------------

poll_ip:

	; packet received?
	rcall	enc_recv_start
	sbiw	Y,0
	breq	poll_ip_exit

	
	; prepare send buffer
	rcall	enc_send_start
	

	; X = &pbuf
	ldi		XL,low(pbuf)
	ldi		XH,high(pbuf)
	
	; read eth header
	ldi		R16,ETH_SIZE
	movw	Z,X
	rcall	enc_read_mem

	
	; prepare eth header
	ldi		R16,6
	ldi		ZL,low(mymac*2)
	ldi		ZH,high(mymac*2)
	movw	Y,X
prepare_eth:
	ldd		R17,Y+ETH_SRC
	std		Y+ETH_DST,R17 ; eth.dst=eth.src
	lpm		R17,Z+
	std		Y+ETH_SRC,R17 ; eth.src=mymac
	adiw	Y,1
	dec		R16
	brne	prepare_eth

	; write eth header
	ldi		R16,ETH_SIZE	
	movw	Z,X
	rcall	enc_write_mem

	
	; Y = eth.type
	movw	Z,X
	ldd		YH,Z+ETH_TYPE+0
	ldd		YL,Z+ETH_TYPE+1

	; eth.type = IP?
	ldi		R16,high(ETHTYPE_IP)
	cpi		YL,low(ETHTYPE_IP)
	cpc		YH,R16
	brne	not_ip
	rcall	process_ip
	rjmp	poll_ip_done
not_ip:

	; eth.type = ARP?
	ldi		R16,high(ETHTYPE_ARP)
	cpi		YL,low(ETHTYPE_ARP)
	cpc		YH,R16
	brne	poll_ip_done
	rcall	process_arp


poll_ip_done:
	rcall	enc_recv_done

poll_ip_exit:
	ret

; ------------------------------------------------

mymac:	.db		MAC0,MAC1,MAC2,MAC3,MAC4,MAC5
myip:	.db		IP0,IP1,IP2,IP3

; ------------------------------------------------
