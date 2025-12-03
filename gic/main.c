/* Third bare metal program for the Rockchip RK3328
 *
 * This has accumulated a fair bit of random code
 * for tests and experiments leading up to the SVC demo
 *
 * Tom Trebisky  12-31-2021  11-27-2025
 */

#include <protos.h>

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

static inline unsigned int
get_el(void)
{
        unsigned int val;

        asm volatile("mrs %0, CurrentEL" : "=r" (val) : : "cc");
        return val >> 2;
}

/* From RK3399 "bare_dump" */
void
show_cpu ( void )
{
		unsigned int val;
		unsigned long lval;
		int core_type;
		int core;
		int el;

		el = get_el ();
		printf ( "Running at EL %d\n", el );

        asm volatile("mrs %0, mpidr_el1" : "=r" (val) : : "cc");
		printf ( "MPidr_el1 = %h\n", val );
		core = val & 0x1ff;
		printf ( "Core %d\n", core );

		asm volatile("mrs %0, CurrentEL" : "=r" (val) : : "cc");
		printf ( "CurrentEL reg = %h\n", val );
		printf ( "CurrentEL = %d\n", val >> 2 );

		/* This is midr, not mpidr */
		asm volatile("mrs %0, midr_el1" : "=r" (val) : : "cc");
		printf ( "Midr_el1 = %h\n", val );
		/* This yields 410FD034
		 * The "D03" indicates an A53 core
		 * If we saw "D08" it would be A72
		 */
		core_type = (val>>4) & 0xfff;
		if ( core_type == 0xD03 )
			printf ( "This is an A53 core (%X)\n", core_type );
		else if ( core_type == 0xD08 )
			printf ( "This is an A72 core (%X)\n", core_type );
		else
			printf ( "This is an unknown core (%X)\n", core_type );

		asm volatile("mrs %0, hcr_el2" : "=r" (val) : : "cc");
		printf ( "HCR_el2 = %h\n", val );
		asm volatile("mrs %0, hcr_el2" : "=r" (lval) : : "cc");
		printf ( "HCR_el2 = %Y\n", lval );

		/* XXX XXX -- grab FMO, IMO */
		lval |= 0x18;
		asm volatile("msr hcr_el2, %0" : : "r" (lval) : "cc");

		asm volatile("mrs %0, spsr_el2" : "=r" (val) : : "cc");
		printf ( "SPSR_el2 = %h\n", val );
		asm volatile("mrs %0, spsr_el2" : "=r" (lval) : : "cc");
		printf ( "SPSR_el2 = %Y\n", lval );

		// Attempting this at EL2 yields a synchronous abort,
		//  exactly as we would expect.
		// asm volatile("mrs %0, spsr_el3" : "=r" (val) : : "cc");
		// printf ( "SPSR_el3 = %h\n", val );

		// Attempting this at EL2 yields a synchronous abort,
		// asm volatile("mrs %0, scr_el3" : "=r" (val) : : "cc");
		// printf ( "SCR_el3 = %h\n", val );

		asm volatile("mrs %0, vbar_el2" : "=r" (val) : : "cc");
		printf ( "VBAR_el2 = %h\n", val );
}

void
testY ( void )
{
		unsigned long lval;

		lval = 0xaabbccdddeadbeef;
		printf ( "SPSR_el2 = %Y\n", lval );
		lval = 0xdeadbeef;
		printf ( "SPSR_el2 = %Y\n", lval );
		lval = 0xdeadbeefL << 16;
		printf ( "SPSR_el2 = %Y\n", lval );
}

/* The argument is in x0, so this works.
 * A lot of serendipity though.
 */
void
try_syscall ( int a, int b )
{
		asm volatile("svc #0x123" );
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

void handle_bad ( int );

void
main ( void )
{
	int n;

	uart_init ();
	gpio_init ();
	gic_init ();
	gic_cpu_init ();

	/* This will run the hello demo */
	// talker ();

#ifdef notdef
    n = sizeof(int);
	printf ( "Int is %d bytes on aarch64\n", n );
    n = sizeof(long);
	printf ( "Long is %d bytes on aarch64\n", n );
	testY ();
#endif

	show_cpu ();

#ifdef notdef
	printf ( "Fire off an SVC\n" );
	try_syscall ( 789, 123 );
#endif

	enable_irq ();
	gic_test ();

	/* This will run the blink demo */
	printf ( "Blinking ...\n" );
	blinker ();

	/* NOTREACHED */
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

	printf ( "Interrupt %d!\n", irq );
}

/* THE END */
