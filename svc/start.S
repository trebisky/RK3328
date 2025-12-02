/* Extremely simple (and virtually unnecessary)
 * assembly language startup file.
 */

	.global start
start:
# this is for arm64
	msr		DAIFSet, #7		// disable interrupts

	ldr		x0, =vectors
	msr		VBAR_el2, x0

	bl		main

	.globl spin
spin:	b		spin

    .align  11
    .globl  vectors
vectors:
    .align  7       /* Current EL, SP = SP_ELx */
	/* Synchronous abort */
	mov	w0,#1
	bl handle_bad
    .align  7       /* FIQ */
	mov	w0,#2
	bl handle_bad
    .align  7       /* IRQ */
	mov	w0,#3
	bl handle_bad
    .align  7       /* Current EL syserr */
	mov	w0,#4
	bl handle_bad

    .align  7       /* Current EL, SP = SP_EL0 */
	/* Synchronous abort */
    stp x29, x30, [sp, #-16]!
	bl handle_sync
    .align  7       /* FIQ */
	mov	w0,#12
	bl handle_bad
    .align  7       /* IRQ */
	mov	w0,#13
	bl handle_bad
    .align  7       /* Current EL syserr */
	mov	w0,#14
	bl handle_bad

	/* The Gnu assembler pads with nop instructions */
    .align  11

	/* Handle fall through from all other cases */
	mov	w0,#99
	bl handle_bad

// THE END
