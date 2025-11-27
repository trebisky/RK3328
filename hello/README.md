This is the first ever code that runs on the RK3328.

It is actually a copy of the same first "hello" project for
the RK3399 that I wrote back in 12-31-2021

It relies upon U-boot initialization of the uart.

I just copied the hello project from the RK3399 and changed
the base address for the uart.
The R1+ uses uart1 (not uart0) for the console.

I gambled (and won) that the register layout for uart registers
on the RK3328 were the same as on the RK3399.

Tom Trebisky  11-26-2025

This program prints "hello" 500 times on the serial port.
