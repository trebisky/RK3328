The "booti" project for the Rockchip RK3328.

This is a failed experiment.
I had hoped this would start my code in EL1,
but it still starts in EL2.

This was begun by copying the files from blink3,
since those have a decent printf() available.

I added a header to my startup file
that allows me to use the U-boot
"booti" command to start this code.

I also needed a "dtb" file to keep the U-boot
booti command happy.

Then from U-boot I do:

1. dhcp
2. tftpboot 03000000 bogus.dtb
3. booti 0x02000000 - 03000000

Tom Trebisky  12-22-2025
