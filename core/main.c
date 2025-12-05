/* Third bare metal program for the Rockchip RK3328
 *
 * This has accumulated a fair bit of random code
 * for tests and experiments leading up to the SVC demo
 *
 * Tom Trebisky  12-31-2021  11-27-2025
 */

#include "protos.h"
#include "rk3328_ints.h"

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

/* If and when a new core starts up, it should come here */
void
core_main ( void )
{
	printf ( "New core!!\n" );
	for ( ;; )
		printf ( "!" );
}

extern int core_start;

#define CORE1		0xff090004
#define CORE2		0xff090008
#define CORE_MAGIC	0xdeadbeaf
#define CORE_START	0x02000000

/* This is where the bootrom parks the cores.
 * I try this and nothing seems to happen.
 * Probably U-boot has already moved them
 * elsewhere.  I see:
 Core 1 FF090004: 94000003
 Core 2 FF090008: 1400000E
 */
static void
core_probe ( void )
{
		int *p;
		int loc;

		p = (int *) CORE1;
		printf ( "Core 1 %X: %X\n", p, *p );
		p = (int *) CORE2;
		printf ( "Core 2 %X: %X\n", p, *p );

		p = (int *) CORE2;
		loc = CORE_START;
		/* Gets warning, but works */
		// loc = (int) &core_start;
		printf ( "New core will start at %X\n", loc );
		*p = loc;
		p = (int *) CORE1;
		*p = CORE_MAGIC;

		p = (int *) CORE1;
		printf ( "Core 1 %X: %X\n", p, *p );
		p = (int *) CORE2;
		printf ( "Core 2 %X: %X\n", p, *p );
}

#define PMU_BASE 0xff140000

struct rock_pmu {
	vu32	wakeup;
	u32		_pad1[2];
	vu32	pwrdn_con;	/* 0c */
	vu32	pwrdn_st;	/* 10 */
	u32		_pad2;
	vu32	pwrmode;	/* 18 */
	vu32	sft_con;	/* 1c */
	vu32	int_con;	/* 20 */
	vu32	int_st;		/* 24 */
	u32		_pad3[7];
	vu32	power;		/* 44 */
	u32		_pad4[14];
	vu32	cpu0_apm;	/* 80 */
	vu32	cpu1_apm;	/* 84 */
	vu32	cpu2_apm;	/* 88 */
	vu32	cpu3_apm;	/* 8c */
	u32		_pad5[4];
	vu32	reg0;		/* a0 */
	vu32	reg1;		/* a4 */
	vu32	reg2;		/* a8 */
	vu32	reg3;		/* ac */
};

/* We don't really fix anything */
void
pmu_fix ( void )
{
		struct rock_pmu *pp;

		pp = (struct rock_pmu *) PMU_BASE;
		// printf ( "Check pmu = %X\n", &pp->power );
		// printf ( "Check pmu = %X\n", &pp->cpu0_apm );
		// printf ( "Check pmu = %X\n", &pp->reg0 );

		printf ( "Pwrdn con = %X\n", pp->pwrdn_con );
		printf ( "Pwrdn  st = %X\n", pp->pwrdn_st );

		// pp->pwrdn_con = 0x0c;
		pp->pwrdn_con = 0x0;
		printf ( "Pwrdn con = %X\n", pp->pwrdn_con );
		printf ( "Pwrdn  st = %X\n", pp->pwrdn_st );
		pp->cpu1_apm = 0xf;
}

/* This initialization does not work because we
 * don't have a proper C language startup that
 * initializes BSS or static variables.
 */
static int show_state = 0;

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

/* This might give about a 1 second delay */
void
delay ( void )
{
        volatile int count = 100000000;

        while ( count-- )
            ;
}

/* Use delay loop instead of interrupts to investigate
 * PLL changes
 */
static void
dumb_delay_loop ( void )
{
	for ( ;; ) {
		delay ();
		light_show ();
	}
}

void
main ( void )
{
	int n;

	uart_init ();
	gpio_init ();
	gic_init ();
	gic_cpu_init ();

	show_cpu ();
	pmu_fix ();

	timer_init ();

	// gic_test ();

	printf ( "\n" );
	printf ( "Probing second core\n" );
	core_probe ();

	pll_show ();

	printf ( "Blinking via dumb delays   ...\n" );
	show_state = 0;
	dumb_delay_loop ();

	printf ( "Blinking via timer interrupts  ...\n" );
	enable_irq ();
	show_state = 0;
	for ( ;; ) {
		asm volatile ( "wfe" );
	}
	/* NOTREACHED */
}

/* THE END */
