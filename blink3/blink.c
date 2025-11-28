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

void
main ( void )
{
	uart_init ();

	gpio_init ();

	/* This will run the hello demo */
	// talker ();

	/* This will run the blink demo */
	printf ( "Blinking ...\n" );
	blinker ();

	/* NOTREACHED */
}

/* THE END */
