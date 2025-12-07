/*
 * pll.c
 *
 * New for the RK3328
 *
 * Copyright (C) 2025  Tom Trebisky  <tom@mmto.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * Tom Trebisky  12/4/2025
 *
 * Driver RK3328 PLL values in the CRU
 *
 */

#include "protos.h"
// #include "rk3328_ints.h"

#define CRU_BASE	0xff440000

#define A_BASE		(CRU_BASE + 0x00)	/* Arm */
#define D_BASE		(CRU_BASE + 0x20)	/* DDR */
#define C_BASE		(CRU_BASE + 0x40)	/* Codec */
#define G_BASE		(CRU_BASE + 0x60)	/* General */
#define N_BASE		(CRU_BASE + 0xA0)	/* New */

/* Notes:
 * CRU_CLKSEL_CON0 has fields that select which PLL runs the
 * ARM cores.  I can use A, G, D, or N.
 * By experiment I determine that A is being used.
 * There is also a bit in CRU_CLKGATE_CON0 that can gate
 * the N pll, so beware.
 * CRU_CLKGATE_CON7 has some bits to gate core clocks,
 *  but these must be something other than cpu cores.
 *  (the manual always says cpu, not core).
 * I don't see any bits to selectively gate CPU clocks.
 * I also note that the reset value of all the gate
 *  registers is 0, which enables all the clocks on
 *  boot.  You set 1 to disable things should you want
 *  to.  Much better than Allwinner which does the
 *  opposite.
 */

static void
pll_dump ( char *who, u32 base, int extra )
{
		u32 *p;
		unsigned long l;
		int val;
		int fbdiv, refdiv;
		int postdiv1, postdiv2;
		int foutvco, fout;

		/* Avoid warning */
		l = base;
		p = (u32 *) l;
		printf ( "\n" );
		printf ( "%s CON0 = %X\n", who, *p++ );
		printf ( "%s CON1 = %X\n", who, *p++ );
		printf ( "%s CON2 = %X\n", who, *p++ );
		printf ( "%s CON3 = %X\n", who, *p++ );
		printf ( "%s CON4 = %X\n", who, *p++ );

		p = (u32 *) l;
		printf ( "%s bypass = %d\n", who, (p[0] >>15) & 0x1 );

		if ( ! extra ) return;

		val = (p[0] >> 12) & 0x7;
		postdiv1 = val;
		printf ( "%s postdiv1 = %x %d\n", who, val, val );
		val = p[0] & 0xfff;
		fbdiv = val;
		printf ( "%s fbdiv = %x %d\n", who, val, val );

		printf ( "%s dsmpd = %d\n", who, (p[1] >>12) & 0x1 );
		printf ( "%s lock = %d\n", who, (p[1] >>10) & 0x1 );
		val = (p[1] >> 6) & 0x7;
		postdiv2 = val;
		printf ( "%s postdiv2 = %x %d\n", who, val, val );
		val = (p[1] >> 0) & 0x3f;
		refdiv = val;
		printf ( "%s refdiv = %x %d\n", who, val, val );

		val = p[2] & 0xffffff;
		printf ( "%s fracdiv = %x %d\n", who, val, val );

		foutvco = 24 * fbdiv / refdiv;
		printf ( "%s   foutvco = %d\n", who, foutvco );

		fout = foutvco / postdiv1 / postdiv2;
		printf ( "%s   fout = %d\n", who, fout );
}

static void
pll_cut ( char *who, u32 base, int cut )
{
		u32 *p;
		unsigned long l;
		u32 val;
		u32 mask;

		/* Avoid warning */
		l = base;
		p = (u32 *) l;

		/* change postdiv1 in CON0 3 --> 6 */
		val = *p;
		mask = 0x3 << 12;
		val &= ~mask;
		val |= cut << 12;
		val |= 0xffff0000;
		*p = val;

		// pll_dump ( who, base, 1 );
}

void
pll_show ( void )
{
		pll_dump ( "A", A_BASE, 1 );
		pll_dump ( "D", D_BASE, 1 );
		pll_dump ( "C", C_BASE, 1 );
		pll_dump ( "G", G_BASE, 1 );
		pll_dump ( "N", N_BASE, 1 );

		/* This changes the A pll from 600 to 300
		 * and YES, it changes the blink rate
		 */
		// pll_cut ( "A", A_BASE, 6 );

		/* Run at 900 Mhz */
		// pll_cut ( "A", A_BASE, 2 );

		/* This changes the N pll from 800 to 400
		 * but I see no change in blink rate.
		 */
		// pll_cut ( "N", N_BASE, 6 );
}

void
cpu_clock_100 ( void )
{
		u32 *p;

		p = (u32 *) A_BASE;

		/* Set postdiv2 to 6 to divide
		 * 600 to 100
		 */
		p[1] = 0x7<<(16+6) | 6<<6;
}

void
cpu_clock_900 ( void )
{
		pll_cut ( "A", A_BASE, 2 );
}

/* THE END */
