/*
 * timer.c
 * driver for the Rockchip RK3328 timer
 *
 * Copied from /u2/Projects/OrangePi/pi4/Github/inter/timer.c
 *
 * However, the RK3328 is NOT the same as the RK3399 timer
 *
 * Tom Trebisky  1-2-2022, 12-3-2025
 */

#include "protos.h"
#include "rk3328_ints.h"

typedef volatile unsigned int vu32;
typedef unsigned int u32;

#define BIT(x)	(1<<(x))

#define TIMER_CLOCK	24000000

/* The RK3328 has 6 of these timers
 * (as well as another 2 set aside for secure mode).
 * The first 5 are count down timers.
 * The last is a count up timer.
 * For my purposes, this is neither here nor there,
 *  they both work the same to subdivide the main
 *  24 Mhz clock.
 * You would only know if you were reading the value in
 *  the "val" registers - and why do that?
 */

/* Apparently this is a 64 bit counter.
 * the "0" register is the low 32 bits
 * the "1" register is the high 32 bits
 * We guess at the final pad -- and win!
 * There is an "array" of these structures.
 */
struct rk_timer {
	vu32	count0;		/* 00 - load register */
	vu32	count1;		/* 04 - load register */
	vu32	val0;		/* 08 - current value */
	vu32	val1;		/* 0c - current value */
	vu32	control;	/* 10 */
	u32		_pad1;
	vu32	intstatus;	/* 18 */
	u32		_pad2;
};

/* The control register has only these 3 bits */
#define CTRL_ENA	BIT(0)
#define CTRL_MODE	BIT(1)
#define CTRL_INTENA	BIT(2)

#ifdef RK3399
#define TIMER0_BASE	((struct rk_timer *) 0xff850000)
#define TIMER6_BASE	((struct rk_timer *) 0xff858000)
#endif

/* The RK3328 TRM is pretty terrible.
 * It shows a base address marked "TIMER (6ch)"
 * and the interrupt list shows 6 timer interrupts.
 * It also shows a base address marked "STIMER (2ch)"
 * and shows 2 interrupt IRQ dedicated to this secure timer
 *
 * Chapter 14 of the TRM describes a 2 channel timer, and
 * perhaps it will do for now to just ignore the 4 other
 * channels and try to work with this.
 */

#define TIMER0_BASE	((struct rk_timer *) 0xff1C0000)
#define STIMER_BASE	((struct rk_timer *) 0xff1D0000)

#ifdef notdef
#define SYS_TIMER 	0
#define SYS_TIMER_IRQ	IRQ_TIMER0
#define SYS_TIMER 	1
#define SYS_TIMER_IRQ	IRQ_TIMER1
#define SYS_TIMER 	4
#define SYS_TIMER_IRQ	IRQ_TIMER4
#endif

#define SYS_TIMER 	5
#define SYS_TIMER_IRQ	IRQ_TIMER5

/* Comment from my RK3399 code */
/* The TRM is entirely vague on how 6 timers are arranged within
 * the address block given by the base address, but experiment
 * shows that it is a contiguous array at the start of the block.
 */

/* The timer counts up until it reaches a value, then interrupts.
 */

static struct rk_timer *cur_timer;

/* The intstatus register has only one bit.
 * you write 1 to it to clear the interrupt */
void
timer_handler ( int irq )
{
	struct rk_timer *tp = cur_timer;

	/* clear the interrupt */
	tp->intstatus = 1;
	printf ( "Tick for IRQ %d\n", irq );
}

void
timer_setup ( int t, u32 val )
{
	struct rk_timer *tp;

	if ( t > 5 )
	    return;

	tp = &TIMER0_BASE[t];
	cur_timer = tp;
	printf ( "Timer %d base addr: %X\n", t, tp );

	tp->control &= ~CTRL_ENA;
	tp->control &= ~CTRL_MODE;	/* free run */
	tp->control |= CTRL_INTENA;

	tp->count0 = val;		/* 0,1 are end value */
	tp->count1 = 0;

	tp->control |=  CTRL_ENA;

	intcon_ena ( SYS_TIMER_IRQ );

	printf ( "Timer: %d  %h %h\n", tp->intstatus, tp->count1, tp->count0 );
}

void
timer_show ( void )
{
	struct rk_timer *tp = cur_timer;
	u32 hi, lo;

	hi = tp->val1;
	lo = tp->val0;
	printf ( "Timer: %d  %h %h\n", tp->intstatus, hi, lo );
}

void
timer_init ( void )
{
	u32 val;
	struct rk_timer *tp = TIMER0_BASE;

	/* This gives about a 2 Hz tick rate */
	val = TIMER_CLOCK / 2;

	timer_setup ( SYS_TIMER, val );
}

/* THE END */
