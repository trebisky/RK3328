/* Second bare metal program for the Rockchip RK3328
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
		int core_type;
		int core;

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
}

void
main ( void )
{
	int el;

	uart_init ();

	gpio_init ();

	/* This will run the hello demo */
	// talker ();

	el = get_el ();
	printf ( "Running at EL %d\n", el );
	show_cpu ();

	/* This will run the blink demo */
	printf ( "Blinking ...\n" );
	blinker ();

	/* NOTREACHED */
}

/* THE END */
