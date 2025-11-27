/* Second bare metal program for the Rockchip RK3328
 *
 * Tom Trebisky  12-31-2021  11-27-2025
 */

typedef volatile unsigned int vu32;
typedef unsigned int u32;

#define BIT(x)	(1<<(x))

/* The TRM describes the UART in chapter 19
 */
struct rock_uart {
	vu32	data;
	vu32	ier;
	vu32	iir;
	vu32	lcr;

	vu32	mcr;
	vu32	lsr;
	vu32	msr;
	vu32	scr;

	u32	_pad0[4];

	vu32	srbr;		/* 0x30 */
	u32	_pad1[3];

	u32	_pad2[12];

	vu32	far;		/* 0x70 */
	vu32	tfr;		/* 0x74 */
	vu32	rfw;		/* 0x78 */
	vu32	status;		/* 0x7c */

	vu32	tfl;		/* 0x80 */
	vu32	rfl;		/* 0x84 */
	vu32	srr;		/* 0x88 */
	vu32	srts;		/* 0x8c */

	vu32	sbcr;		/* 0x90 */
	vu32	sdmam;		/* 0x94 */
	vu32	sfe;		/* 0x98 */
	vu32	srt;		/* 0x9c */

	vu32	stet;		/* 0xa0 */
	vu32	htx;		/* 0xa4 */
	vu32	dmasa;		/* 0xa8 */
	u32	_pad3;		/* 0xac */

	u32	_pad4[16];

	u32	_pad5;		/* 0xf0 */
	vu32	cpr;		/* 0xf4 */
	vu32	ucv;		/* 0xf8 */
	vu32	ctr;		/* 0xfc */

};

#ifdef notdef
/* RK3399 */
#define UART0_BASE	((struct rock_uart *) 0xff180000)
#define UART1_BASE	((struct rock_uart *) 0xff190000)
#define UART2_BASE	((struct rock_uart *) 0xff1a0000)
#define UART3_BASE	((struct rock_uart *) 0xff1b0000)
#define UART4_BASE	((struct rock_uart *) 0xff370000)
#define UART_BASE	UART2_BASE
#endif

/* For the RK3328 */
#define UART0_BASE	((struct rock_uart *) 0xff110000)
#define UART1_BASE	((struct rock_uart *) 0xff120000)
#define UART2_BASE	((struct rock_uart *) 0xff130000)
#define UART_BASE	UART2_BASE

/* Bits in the status register */
#define ST_BUSY		BIT(0)
#define ST_TNF		BIT(1)
#define ST_TE		BIT(2)
#define ST_RNE		BIT(3)
#define ST_RF		BIT(4)

/* The TRM describes the GPIO in chapter 18
 */

struct rock_gpio {
	vu32		data;
	vu32		dir;
	u32		_pad0[2];

	u32		_pad1[8];

	vu32		ie;		/* 0x30 */
	vu32		im;		/* 0x34 */
	vu32		il;		/* 0x38 */
	vu32		ip;		/* 0x3c */

	vu32		is;		/* 0x40 */
	vu32		ris;		/* 0x44 */
	vu32		debounce;	/* 0x48 */
	vu32		eoi;		/* 0x4c */

	vu32		ext;		/* 0x50 */
	u32		_pad2[3];

	vu32		sync;		/* 0x60 */
};

#ifdef notdef
/* RK3399 */
#define GPIO0_BASE	((struct rock_gpio *) 0xff720000)
#define GPIO1_BASE	((struct rock_gpio *) 0xff730000)
#define GPIO2_BASE	((struct rock_gpio *) 0xff780000)
#define GPIO3_BASE	((struct rock_gpio *) 0xff788000)
#define GPIO4_BASE	((struct rock_gpio *) 0xff790000)
#define GPIO_BASE	GPIO0_BASE
#endif

/* RK3328 */
#define GPIO0_BASE	((struct rock_gpio *) 0xff210000)
#define GPIO1_BASE	((struct rock_gpio *) 0xff220000)
#define GPIO2_BASE	((struct rock_gpio *) 0xff230000)
#define GPIO3_BASE	((struct rock_gpio *) 0xff240000)
#define GPIO_BASE	GPIO3_BASE

/* GRF - general register file */
struct rock_grf {
	vu32		gpio0a_iomux;	/* 00 */
	vu32		gpio0b_iomux;
	vu32		gpio0c_iomux;
	vu32		gpio0d_iomux;

	vu32		gpio1a_iomux;	/* 10 */
	vu32		gpio1b_iomux;
	vu32		gpio1c_iomux;
	vu32		gpio1d_iomux;

	vu32		gpio2a_iomux;	/* 20 */
	vu32		gpio2bl_iomux;
	vu32		gpio2bh_iomux;
	vu32		gpio2cl_iomux;
	vu32		gpio2ch_iomux;
	vu32		gpio2d_iomux;

	vu32		gpio3al_iomux;	/* 38 */
	vu32		gpio3ah_iomux;
	vu32		gpio3bl_iomux;
	vu32		gpio3bh_iomux;
	vu32		gpio3c_iomux;
	vu32		gpio3d_iomux;

	vu32		common_iomux;	/* 50 */
	u32			_pad1[43];

	vu32		gpio0a_pull;	/* 100 */
	vu32		gpio0b_pull;
	vu32		gpio0c_pull;
	vu32		gpio0d_pull;

	vu32		gpio1a_pull;	/* 110 */
	vu32		gpio1b_pull;
	vu32		gpio1c_pull;
	vu32		gpio1d_pull;

	vu32		gpio2a_pull;	/* 120 */
	vu32		gpio2b_pull;
	vu32		gpio2c_pull;
	vu32		gpio2d_pull;

	vu32		gpio3a_pull;	/* 130 */
	vu32		gpio3b_pull;
	vu32		gpio3c_pull;
	vu32		gpio3d_pull;
	u32			_pad2[48];

	vu32		gpio0a_drive;	/* 200 */
	vu32		gpio0b_drive;
	vu32		gpio0c_drive;
	vu32		gpio0d_drive;

	vu32		gpio1a_drive;	/* 210 */
	vu32		gpio1b_drive;
	vu32		gpio1c_drive;
	vu32		gpio1d_drive;

	vu32		gpio2a_drive;	/* 220 */
	vu32		gpio2b_drive;
	vu32		gpio2c_drive;
	vu32		gpio2d_drive;

	vu32		gpio3a_drive;	/* 230 */
	vu32		gpio3b_drive;
	vu32		gpio3c_drive;
	vu32		gpio3d_drive;
	u32			_pad3[48];

	vu32		gpio0ab_sr;	/* 300 */
	vu32		gpio0cd_sr;

	vu32		gpio1ab_sr;	/* 308 */
	vu32		gpio1cd_sr;

	vu32		gpio2ab_sr;	/* 310 */
	vu32		gpio2cd_sr;

	vu32		gpio3ab_sr;	/* 318 */
	vu32		gpio3cd_sr;
	u32			_pad4[56];

	vu32		gpio0ab_smt;	/* 380 */
	vu32		gpio0cd_smt;

	vu32		gpio1ab_smt;	/* 388 */
	vu32		gpio1cd_smt;

	vu32		gpio2ab_smt;	/* 390 */
	vu32		gpio2cd_smt;

	vu32		gpio3ab_smt;	/* 398 */
	vu32		gpio3cd_smt;
};

#define GRF_BASE	((struct rock_grf *) 0xff100000)

void
uart_init ( void )
{
	/* nothing here, we rely on U-Boot */
}

void
uart_putc ( char c )
{
	struct rock_uart *up = UART_BASE;

	while ( ! (up->status & ST_TNF) )
	    ;

	up->data = c;
}

/* Portable code below here */

void
puts ( char *s )
{
	while ( *s ) {
	    if (*s == '\n')
		uart_putc('\r');
	    uart_putc(*s++);
	}
}

int limit = 500;

void
talker ( void )
{
	if ( ! limit ) {
	    for ( ;; )
		puts ( "hello " );
	} else {
	    for ( ;limit--; )
		puts ( "hello " );
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

#ifdef RK3399_BRUTE
/* This works, but manipulates all 32 bits.
 * setting the direction to output (1) is essential
 */
void
blinker_BRUTE ( void )
{
	struct rock_gpio *gp = GPIO_BASE;

	gp->dir = 0xffffffff;

	for ( ;; ) {
	    puts ( "on\n" );
	    gp->data = 0xffffffff;
	    delay ();
	    puts ( "off\n" );
	    gp->data = 0;
	    delay ();
	}
}

/* Trial and error to find the bit (since I was lied to and
 *  told it was B5 originally
 *  It is actually B3
 */
// #define LED_MASK	0xff<<8	/* yes */
// #define LED_MASK	0xf0<<8	/* no */
// #define LED_MASK	0x0f<<8	/* yes */
// #define LED_MASK	0x03<<8	/* no */
// #define LED_MASK	0x0c<<8	/* yes */
// #define LED_MASK	0x04<<8	/* no */
// #define LED_MASK	0x08<<8	/* yes */
#endif	/* RK3399_BRUTE */

#ifdef notdef
/* RK3399 - B3 */
#define LED_BIT		(8+3)
#define LED_MASK	BIT(LED_BIT)
#endif

/*
 * The RK3328 status LED is connected to GPIO3 C5
 */
#define LED_BIT		(16+5)
#define LED_MASK	BIT(LED_BIT)

/* This version is polite and doesn't disturb any bits other
 * than the one of interest.
 */
void
blinker ( void )
{
	struct rock_gpio *gp = GPIO_BASE;

	gp->dir |= LED_MASK;

	/* Setting the bit does turn the LED on */
	for ( ;; ) {
	    puts ( "on\n" );
	    // gp->data |= LED_MASK;
	    gp->data = 0xffffffff;
	    delay ();
	    puts ( "off\n" );
	    // gp->data &= ~LED_MASK;
	    gp->data = 0;
	    delay ();
	}
}

/* We didn't need these for the RK3399.
 * Probably U-boot blinked the LED and had
 * everything set up for us.
 */
void
grf_init ( void )
{
	struct rock_grf *gp = GRF_BASE;

	gp->gpio3c_iomux = 0xffff0000;
	gp->gpio3c_pull = 0xffff0000;
	gp->gpio3cd_sr = 0xffff0000;
	gp->gpio3cd_smt = 0xffff0000;
}

void
gpio_init ( void )
{
	struct rock_gpio *gp = GPIO_BASE;

	/* All outputs */
	gp->dir = 0xffffffff;
}

void
main ( void )
{
	uart_init ();

	grf_init ();
	gpio_init ();

	/* This will run the hello demo */
	// talker ();

	puts ( "Blinking ...\n" );
	/* This will run the blink demo */
	blinker ();

	/* NOTREACHED */
}

/* THE END */
