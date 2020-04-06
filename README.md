# pinetime_display
This project provides some functions to write a square to the display using fast direct memory access spi functions

## compiling
(this part needs some work)
Extract the nRF5 SDK (https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download#infotabs) somewhere on your computer
In the Makefile. replace ../sdk with the path to where you extracted the sdk.

## uploading
To upload this code using the instructions on the pine64 wiki:
https://wiki.pine64.org/index.php/Reprogramming_the_PineTime
The .hex file is located in the _build directory
I personally use a J-link programmer with the nrfjprog commantline tool.
To upload using this tool, use the following commands:
cd _build
nrfjprog -f NRF52 --program nrf52832_xxaa.hex --sectorerase
nrfjprog -f NRF52 --reset
