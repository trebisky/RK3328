/*
 * assembly language startup file.
 * Added the header to allow the
 *  U-boot "booti" command to work
 *  with this.
 */

#define ARM64_IMAGE_MAGIC   "ARM\x64"

	.global start
start:

	/*
	 * Taken from linux-git/arch/arm64/kernel/head.S
     * Image header expected by U-boot "booti"
     */
    nop						// first instruction
    b   primary_entry       // second instruction
    .quad   0               // Image load offset from start of RAM, little-endian
    .quad	_end-start		// Effective size of kernel image, little-endian
    .quad	0xa				// Informative flags, little-endian
    .quad   0               // reserved
    .quad   0               // reserved
    .quad   0               // reserved
    .ascii  ARM64_IMAGE_MAGIC       // Magic number
    .long   0		        // Offset to the PE header.


/* I began my experiments by setting the kernel image size and
 *  the flags to zero, but later set up realistic values.
 * The current value for flags is 0xa, which is:
 *  Bit 3 - 1 - kernel can be anywhere, but must be 2M aligned
 *          0 -  would say to move it a close as possible to 0 address in ram
 *  Bits 1-2 - 01 = 4K page size
 *  Bit 0  -  0 says little endian.
 *
 * I set the PE offset to zero, expecting that U-boot will just
 * ignore it.
 * PE is "portable executable" and has to do with EFI booting.
 *
 * Also --  if we just use "go" to run this, it will work
 * by virtue of the first two words of the header being
 * machine instructions that jump to the code below.
 */

primary_entry:

# this is for arm64
	msr		DAIFSet, #7		// disable interrupts

	bl		main

spin:	b		spin


// THE END
