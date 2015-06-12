; ------------------------------------------------

; fill buffer with index page
; in R23:R22 - url (ram)
; in R23:R22 - params (ram)
; i/o R11 - tcp flags
index_page:

	; set port state
	clr		R16
	cp		R22,R16
	cpc		R23,R16
	breq	no_command

	movw	Y,R23:R22
	ld		R16,Y+
	ld		R17,Y+

	subi	R17,'0'
	cpi		R17,DATA_BITS
	brge	no_command
	
	cpi		R16,'t'
	brne	no_command

	ldi		R18,1
shift_bit:
	tst		R17
	breq	toggle_bit
	lsl		R18
	dec		R17
	rjmp	shift_bit

toggle_bit:
	in		R16,DATA_PORT
	eor		R16,R18
	out		DATA_PORT,R16

no_command:


	; write HTTP header
	ldi		R20,page_header_end-page_header
	ldi		YL,low(page_header*2)
	ldi		YH,high(page_header*2)
	rcall	write_page

	; write <pre>
	ldi		ZL,low(html_pre1*2)
	ldi		ZH,high(html_pre1*2)
	rcall	enc_write_string

	; write hello
	ldi		ZL,low(http_hello*2)
	ldi		ZH,high(http_hello*2)
	rcall	enc_write_string

	; write port state
	clr		R20
	in		R21,DATA_PORT

write_port_state:

	movw	Y,X							; buffer

	ldi		ZL,low(html_hlink1*2)		; put <a href='
	ldi		ZH,high(html_hlink1*2)
	rcall	strcpy_P

	ldi		ZL,low(data_toggle*2)		; put /?t
	ldi		ZH,high(data_toggle*2)
	rcall	strcpy_P

	ldi		R16,'0'						; put n
	add		R16,R20
	st		Y+,R16

	ldi		ZL,low(html_hlink2*2)		; put '>
	ldi		ZH,high(html_hlink2*2)
	rcall	strcpy_P

	ldi		R16,'P'						; put Pn
	st		Y+,R16
	ldi		R16,'0'
	add		R16,R20
	st		Y+,R16

	ldi		ZL,low(html_hlink3*2)		; put </a>
	ldi		ZH,high(html_hlink3*2)
	rcall	strcpy_P

	mov		R16,R21
	andi	R16,1						; check bit state
	breq	port_bit_is_off

	ldi		ZL,low(data_on*2)			; put on
	ldi		ZH,high(data_on*2)
	rjmp	write_port_bit_state

port_bit_is_off:
	ldi		ZL,low(data_off*2)			; put off
	ldi		ZH,high(data_on*2)

write_port_bit_state:
	rcall	strcpy_P

	mov		R16,YL						; write data
	sub		R16,XL
	movw	Z,X
	rcall	enc_write_mem

	lsr		R21							; loop
	inc		R20
	cpi		R20,DATA_BITS
	brne	write_port_state

	
	; write refresh
	ldi		ZL,low(data_refresh*2)
	ldi		ZH,high(data_refresh*2)
	rcall	enc_write_string
	
	; write </pre>
	ldi		ZL,low(html_pre2*2)
	ldi		ZH,high(html_pre2*2)
	rcall	enc_write_string

	ret	

; ------------------------------------------------

; in R15:R14 - received data length
; in/out R11 - tcp flags
tcp_data:

	; read request header
	ldi		R16,PBUF_SZ-1
	movw	Z,X
	rcall	enc_read_mem

	; add space to buffer end
	movw	Z,X
	adiw	Z,PBUF_SZ-1
	ldi		R16,' '
	st		Z,R16


	; check "GET "
	movw	Y,X

	ldi		R16,4
	ldi		ZL,low(http_get*2)
	ldi		ZH,high(http_get*2)
	rcall	memcmp_P
	brne	send_finack


get_ok:
	; R21:R20 = (extract URL from request)
	movw	R21:R20,Y

	movw	Z,Y
find_url_end:
	ld		R16,Z+
	cpi		R16,' '
	brne	find_url_end

	clr		R16
	st		-Z,R16

	; R23:R22 = (extract params from URL)
	clr		R22
	clr		R23

	movw	Z,Y
find_url_params:
	ld		R16,Z+
	tst		R16
	breq	no_url_params
	cpi		R16,'?'
	brne	find_url_params

	movw	R23:R22,Z

	clr		R16
	st		-Z,R16
no_url_params:


	; is index page requested?
	movw	Y,R21:R20
	ldi		ZL,low(http_indexurl*2)
	ldi		ZH,high(http_indexurl*2)
	rcall	strcmp_P
	brne	not_index_page
	rcall	index_page
	rjmp	send_finack
not_index_page:


	; is info page requested?
	;movw	Y,R21:R20
	;ldi	ZL,low(http_infourl*2)
	;ldi	ZH,high(http_infourl*2)
	;rcall	strcmp_P
	;brne	not_info_page

	;ldi	R20,page_header_end-page_header		; write HTTP header
	;ldi	YL,low(page_header*2)
	;ldi	YH,high(page_header*2)
	;rcall	write_page

	;ldi	ZL,low(http_info*2)					; write info line
	;ldi	ZH,high(http_info*2)
	;rcall	enc_write_string
	;rjmp	send_finack

;not_info_page:


	; 404 page
	ldi		R20,page_404_end-page_404
	ldi		YL,low(page_404*2)
	ldi		YH,high(page_404*2)
	rcall	write_page


send_finack:
	ldi		R16,(1<<FIN)|(1<<ACK)
	mov		R11,R16

	ret

; ------------------------------------------------

; write page
; R20 - count
; Y - data
write_page:
	movw	Z,Y
	adiw	Y,2
	lpm		R16,Z+
	lpm		R17,Z+
	movw	Z,R17:R16
	rcall	enc_write_string
	dec		R20
	brne	write_page
	ret

; ------------------------------------------------

; compare strings
; in Y - string1 (ram)
; in Z - string2 (flash)
; out eq
; uses R17,R18
strcmp_P:
	ld		R17,Y+
	lpm		R18,Z+
	cp		R17,R18
	brne	strcmp_P_done
	tst		R17
	brne	strcmp_P
strcmp_P_done:
	ret

; ------------------------------------------------

; compare blocks
; in Y - block1 (ram)
; in Z - block2 (flash)
; in R16 - len
; out eq
; uses R17,R18,Y,Z
memcmp_P:
	ld		R17,Y+
	lpm		R18,Z+
	cp		R17,R18
	brne	memcmp_P_done
	dec		R16
	brne	memcmp_P
memcmp_P_done:
	ret

; ------------------------------------------------

; copy string
; in Y - dst (ram)
; in Z - src (flash)
; uses R17,Y,Z
strcpy_P:
	lpm		R17,Z+
	tst		R17
	breq	strcpy_P_done
	st		Y+,R17
	rjmp	strcpy_P
strcpy_P_done:
	ret

; ------------------------------------------------

http_indexurl:	.db		"/",0
http_get:		.db		"GET "
http_200:		.db		"HTTP/1.0 200 OK",13,10,0
http_404:		.db		"HTTP/1.0 404 Not Found",13,10,0,0
http_content:	.db		"Content-Type: text/html; charset=windows-1251",13,10,0
http_server:	.db		"Server: ",HTTP_SERV,13,10,0,0
http_breakline:	.db		10,13,0,0
http_notfound:	.db		"<h1>404 Not Found</h1>",0,0
http_hello:		.db		"Привет из тиньки!",13,10,0

html_pre1:		.db		"<pre>",13,10,0
html_pre2:		.db		"</pre>",13,10,0,0

html_hlink1:	.db		"<a href='",0
html_hlink2:	.db		"'>",0,0
html_hlink3:	.db		"</a>",0,0

data_toggle:	.db		"/?t",0
data_on:		.db		" is on",13,10,0,0
data_off:		.db		" is off",13,10,0

data_refresh:	.db		13,10,"[<a href='/'>refresh</a>]",13,10,0


page_header:
	.dw		http_200 * 2
	.dw		http_content * 2
	.dw		http_server * 2
	.dw		http_breakline * 2
page_header_end:

page_404:
	.dw		http_404 * 2
	.dw		http_content * 2
	.dw		http_server * 2
	.dw		http_breakline * 2
	.dw		http_notfound * 2
page_404_end:

; ------------------------------------------------
