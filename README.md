# Pinetime bad apple
This repo contains the code to run bad apple on the pinetime. The code to process the video beforehand, on your computer, can be fount at: https://github.com/TT-392/Pinetime-bad-apple-video-processor.

## Running bad apple on your own pinetime
You can run bad apple on a dev kit through the programming pins or over Bluetooth through the wasp-os bootloader (though I'm not sure I'd recommend this if you are not prepared to open it up if something goes wrong). If you are going to be using the wasp-os bootloader, you should go to wasp-os for information on how to install it (this is probably the riskiest step, in terms of bricking a sealed watch). If you have installed the wasp-os bootloader, you can use something like the nrf connect android app to actually flash the files. If you are programming over the programming header, you probably already have some experience flashing binaries onto your watch, but if not, info on this can be found on https://wiki.pine64.org/index.php/Reprogramming_the_PineTime.

There are 2 parts to this application, one is the video in flash, and the other is the core, which is actually the code that runs on the smartwatch. 

### Writing the video to the spi flash
First, you have to flash the video. Because I haven't looked into the Bluetooth stack yet, my solution to write the video to the spi flash is to divide the video file into 14 binaries which should be flashed one by one (If you have a better method of doing this, the video file can be found at: https://github.com/TT-392/Pinetime-bad-apple-video-processor). 

If you are uploading to a watch with the wasp-os bootloader, you are gonna want to get the ota binaries: x_bad_apple_flash_ota_x.zip, and if you are uploading to an open watch, over the programming pins, you are gonna want to use the raw binaries: bad_apple_flash_x.hex. After uploading each one of these binaries, make sure to follow the ui on the screen to actually write that part of the video to spi flash (you can also play the video through this ui, but it will stop playing / do strange stuff at some point if you haven't flashed the entire thing yet).

### Flashing the core binary to actually play the video
If one of the flash binaries is still on the watch, you should already be able to play bad apple. In case you are going to want to run bad apple more than once (or have used a different method of writing to the spi flash), you are probably gonna want to flash the core binary (mainly so you aren't writing a bunch of stuff to the spi flash every time you restart). The core binary is basically a flash binary with the writing to flash part stripped out.

Just like with the flash binaries, there is an ota binary (bad_apple_ota.zip), and a binary for flashing over the programming pins (bad_apple.hex).

## Compiling  
Extract the nRF5 SDK (https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download) somewhere on your computer, the default in the compile script expects it to be in the root directory of this project, but you can change this path by changing the ```SDK_ROOT``` path in the compile.sh script. Also, don't forget to load the submodules ```git submodule update --init```. There are also a couple of dependencies you are gonna need to install:\
arm-none-eabi\
arm-none-eabi-newlib (or newlib-arm-none-eabi depending on your distro)\
And to compile the ota binaries, the following python packages:\
click\
pyserial


The compile.sh script can be used to compile different targets:

```./compile.sh core``` will compile the core for flashing through the programming pins\
```./compile.sh spi flash``` will compile the 14 spi flash files for flashing through the programming pins\
```./compile.sh core_ota``` will compile the core for flashing over bluetooth\
```./compile.sh spi flash_ota``` will compile the 14 spi flash files for flashing over bluetooth\




