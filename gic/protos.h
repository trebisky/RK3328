/* protos.h
 * Tom Trebisky  11-28-2025
 *
 * Prototypes and some common typedefs
 */

typedef volatile unsigned int vu32;
typedef unsigned int u32;

#define BIT(x)	(1<<(x))

void spin ( void );

void uart_init ( void );
void uart_putc ( char );
void uart_puts ( char * );

void printf ( char *, ... );

void gpio_init ( void );
void status_led_on ( void );
void status_led_off ( void );
void wan_led_on ( void );
void wan_led_off ( void );
void lan_led_on ( void );
void lan_led_off ( void );

// THE END
