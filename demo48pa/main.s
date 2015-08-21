	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	shift_reg_upd
	.type	shift_reg_upd, @function
shift_reg_upd:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	ret
	.size	shift_reg_upd, .-shift_reg_upd
.global	sr_init
	.type	sr_init, @function
sr_init:
/* prologue: function */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
	cbi 0xb,2
	sbi 0xb,0
	lds r24,s
	cpse r24,__zero_reg__
	rjmp .L3
	ldi r24,lo8(-128)
	sts data,r24
	rjmp .L4
.L3:
	cpi r24,lo8(1)
	brne .L5
	ldi r24,lo8(64)
	sts data,r24
	rjmp .L4
.L5:
	cpi r24,lo8(2)
	brne .L6
	ldi r24,lo8(32)
	sts data,r24
	rjmp .L4
.L6:
	cpi r24,lo8(3)
	brne .L7
	ldi r24,lo8(16)
	sts data,r24
	rjmp .L4
.L7:
	cpi r24,lo8(4)
	brne .L8
	ldi r24,lo8(8)
	sts data,r24
	rjmp .L4
.L8:
	cpi r24,lo8(5)
	brne .L9
	ldi r24,lo8(4)
	sts data,r24
	rjmp .L4
.L9:
	cpi r24,lo8(6)
	brne .L10
	ldi r24,lo8(2)
	sts data,r24
	rjmp .L4
.L10:
	cpi r24,lo8(7)
	brne .L4
	ldi r24,lo8(1)
	sts data,r24
.L4:
	lds r24,xxx
	subi r24,lo8(-(-1))
	sts xxx,r24
	ldi r18,lo8(8)
.L11:
	subi r18,lo8(-(-1))
	in r20,0xb
	in r19,0xb
	lds r25,data
	mov r24,r25
	ldi r25,0
	mov r0,r18
	rjmp 2f
	1:
	asr r25
	ror r24
	2:
	dec r0
	brpl 1b
	andi r24,1
	clr r25
	neg r25
	neg r24
	sbc r25,__zero_reg__
	eor r24,r20
	andi r24,2
	clr r25
	eor r24,r19
	out 0xb,r24
	sbi 0xb,2
	cbi 0xb,2
	cpse r18,__zero_reg__
	rjmp .L11
.L12:
	lds r24,yyy
	subi r24,lo8(-(-1))
	sts yyy,r24
	lds r24,s
	subi r24,lo8(-(1))
	cpi r24,lo8(8)
	brsh .L13
	sts s,r24
	rjmp .L14
.L13:
	sts s,__zero_reg__
.L14:
	cbi 0xb,0
	ldi r24,lo8(-9537)
	ldi r25,hi8(-9537)
	1: sbiw r24,1
	brne 1b
	rjmp .
	nop
	sbi 0xb,0
	ret
	.size	sr_init, .-sr_init
.global	yyy
	.section .bss
	.type	yyy, @object
	.size	yyy, 1
yyy:
	.zero	1
.global	xxx
	.type	xxx, @object
	.size	xxx, 1
xxx:
	.zero	1
	.comm	data,1,1
.global	s
	.type	s, @object
	.size	s, 1
s:
	.zero	1
	.ident	"GCC: (GNU) 4.9.2 20141224 (prerelease)"
.global __do_clear_bss
