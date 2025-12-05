This is a "bare metal" project for the Rockchip RK3328.

The idea here is to accomplish these goals:

* start a second core running

This began by copying the files from "timer"

I learn how to inspect and fiddle with the PLL.
I find that the "A" Pll is running at 600 Mhz
and that this is the clock for the 0 core ("main" core).

** Starting cores does NOT work yet

Tom Trebisky  1-1-2022  12-4-2025
