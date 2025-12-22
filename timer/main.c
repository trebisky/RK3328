/* Third bare metal program for the Rockchip RK3328
 *
 * This has accumulated a fair bit of random code
 * for tests and experiments leading up to the SVC demo
 *
 * Tom Trebisky  12-31-2021  11-27-2025
 */

#include "protos.h"
#include "rk3328_ints.h"

/* Portable code below here */

int limit = 500;

void
talker ( void )
{
	if ( ! limit ) {
	    for ( ;; )
		uart_puts ( "hello " );
	} else {
	    for ( ;limit--; )
		uart_puts ( "hello " );
	}
}

/* This might give about a 1 second delay */
void
delay ( void )
{
        volatile int count = 100000000;

        while ( count-- )
            ;
}

void
blinker ( void )
{
	for ( ;; ) {
	    // uart_puts ( "on\n" );
		status_led_on ();
		wan_led_on ();
		lan_led_on ();
	    delay ();
	    // uart_puts ( "off\n" );
		status_led_off ();
		wan_led_off ();
		lan_led_off ();
	    delay ();
	}
}

/* These only go inline with gcc -O of some kind */
static inline void
INT_unlock ( void )
{
    asm volatile("msr DAIFClr, #3" : : : "cc");
}

static inline void
INT_lock ( void )
{
    asm volatile("msr DAIFSet, #3" : : : "cc");
}

/* We enable both IRQ and FIQ
 * IRQ = 0x1, RIQ = 0x2
 */
static inline void
enable_irq ( void )
{
    //COMPILER_BARRIER();
    //write_daifclr(DAIF_IRQ_BIT);
    //isb();
	asm volatile ("" ::: "memory");
    asm volatile ( "msr DAIFClr, #3" : : : "cc");
    asm volatile ( "isb" );
}

/* This initialization does not work because we
 * don't have a proper C language startup that
 * initializes BSS or static variables.
 */
static int show_state = 0;

void handle_bad ( int );

/* This really belongs in start.S */
static void
el2_fixup ( void )
{
		unsigned long lval;

		/* XXX XXX -- grab FMO, IMO */
		asm volatile("mrs %0, hcr_el2" : "=r" (lval) : : "cc");
		lval |= 0x18;
		asm volatile("msr hcr_el2, %0" : : "r" (lval) : "cc");
}

void
main ( void )
{
	int n;

	el2_fixup ();

	uart_init ();
	gpio_init ();
	gic_init ();
	gic_cpu_init ();

	/* This will run the hello demo */
	// talker ();

	timer_init ();

	enable_irq ();
	// gic_test ();

	/* This will run the blink demo */
	// printf ( "Blinking ...\n" );
	// blinker ();

	printf ( "Blinking via timer interrupts  ...\n" );
	show_state = 0;
	for ( ;; ) {
		asm volatile ( "wfe" );
	}

	/* NOTREACHED */
}

/* Called at interrupt level by timer interrupts
 */

void
light_show ( void )
{
	// printf ( "Show %d\n", show_state );

	if ( show_state == 0 ) {
		show_state = 1;
		status_led_off ();
		wan_led_on ();
	} else if ( show_state == 1 ) {
		show_state = 2;
		wan_led_off ();
		lan_led_on ();
	} else if ( show_state == 2 ) {
		show_state = 0;
		lan_led_off ();
		status_led_on ();
	}
}

/* When we get a synchronous exception,
 * it should come here.
 */
void
handle_sync ( int a, int b )
{
	printf ( "Synch exception: %d %d\n", a, b );
	// spin ();
}

void
handle_bad ( int who )
{
	printf ( "Bad exception: %d\n", who );
	spin ();
}

void
handle_int ( void )
{
	int irq;

	irq = intcon_irqwho ();
	intcon_irqack ( irq );

	if ( irq >= IRQ_TIMER0 && irq <= IRQ_TIMER5 ) {
		timer_handler ( irq );
		light_show ();
		return;
	}

	printf ( "Unexpected interrupt %d!\n", irq );
}

/* THE END */
