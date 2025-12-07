/*
 * psci.c
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
 * Tom Trebisky  12/5/2025
 *
 * This should be general code to make PSCI calls to the ATF firmware.
 * In the case of the RK3228, this is BL31
 *
 * This is supported by assembly language code in start.S
 *
 */

#include "protos.h"

void
psci_test ( void )
{
		int val;
		int major, minor;
		int cluster, core, thread;
		u64 cpu_id;

		val = smc_version ();
		printf ( "SMC version = %X\n", val );
		major = val >>16;
		minor = val & 0xffff;
		printf ( " SMC version %d.%d\n", major, minor );

		val = smc_socid ( 1 );
		printf ( "PSCI socid %d = %X\n", 1, val );

		val = psci_version ();
		printf ( "PSCI version = %X\n", val );
		major = val >>16;
		minor = val & 0xffff;
		printf ( " PSCI version %d.%d\n", major, minor );

		/* Fields in the MPIDR */
		cluster = 0;
		core = 1;
		thread = 0;

		cpu_id = cluster<<16 | core<<8 | thread;

		val = psci_cpu_on ( cpu_id );
		printf ( "PSCI cpu on = %Y %d\n", val, val );
}

/* THE END */
