# pinetime_display_driver

This is the display driver used in TT-time, it uses dma spi to write data to the PineTime's ST7789 display controller.


Because this display controller needs a pin to be high or low to signify if a command or data byte is being sent. With most drivers this means stopping the dma transfer, changing the pin state and starting it again. This slows down the driver. In this driver this is overcome by using a timer and some PPI's to reliably turn this pin on and off at the right moments in a dma transfer.


Currently the driver uses timer 3 and the first 8 PPI channels. I am planning to not hard code these in the future. This will probably mean that the way display_init() is called will change a bit at some point in the future.

## Dependencies
The nordic sdk from which it uses
* nrf_gpio.h
* nrf_delay.h
* nrf.h
<br>Eventially the plan is to make it so that these are no longer needed cause it is just writing to registers. (You probably are still gonna need something to translate register names to their addresses, but I am guessing pretty much any kind of developement environment for this chip has that).


## NOTE
The driver isn't quite done yet, and I am not the greatest programmer, so use with a bit of caution.

## Planned improvements
* Make timer and PPI channels not hardcoded
* Make it so that nordic sdk isn't required anymore
