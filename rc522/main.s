	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
	.comm	term_var_info,2,1
	.text
.global	main
	.type	main, @function
main:
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
/* prologue: function */
/* frame size = 0 */
/* stack size = 2 */
.L__stack_usage = 2
	lds r24,term_var_info
	lds r25,term_var_info+1
	ldi r18,lo8(100)
	movw r30,r24
	st Z,r18
	lds r24,term_var_info
	lds r25,term_var_info+1
	adiw r24,1
	ldi r18,lo8(121)
	movw r30,r24
	st Z,r18
/* epilogue start */
	pop r29
	pop r28
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.9.2 20141224 (prerelease)"
.global __do_clear_bss
