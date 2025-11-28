/* protos.h
 * Tom Trebisky  11-28-2025
 *
 * Prototypes and some common typedefs
 */

typedef volatile unsigned int vu32;
typedef unsigned int u32;

#define BIT(x)	(1<<(x))

void uart_init ( void );
void uart_putc ( char );
void uart_puts ( char * );

// THE END
