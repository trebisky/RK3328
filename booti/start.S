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
    .long   0		      // Offset to the PE header.

/* Note in the above that I have simply set the kernel image size,
 * flags, and PE offset to zero, expecting that U-boot just
 * ignores them.
 * Also note that if we just use "go" to run this, it will work
 * by virtue of the first two words of the header being
 * machine instructions.
 */

primary_entry:

# this is for arm64
	msr		DAIFSet, #7		// disable interrupts

	bl		main

spin:	b		spin


// THE END
