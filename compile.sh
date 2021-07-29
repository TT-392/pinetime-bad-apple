#!/bin/bash

#I know this can be done faster (not having to rebuild everything for each file), but good chance you will only have to run this stuff once, so I am not gonna put in too much effort.

if [ "$1" = "core" ];
then
    make OUTFILENAME=bad_apple
elif [ "$1" = "spiflash" ];
then
    for f in src/bad_apple/resources/video*; do
        fileNr=${f#src/bad_apple/resources/video_}
        fileNr=${fileNr%\.h}
        echo $fileNr
        make OUTFILENAME=bad_apple_flash_$fileNr EXTRAFLAGS=-DSPIFLASH=$fileNr
    done
elif [ "$1" = "core_ota" ];
then
    make -f Makefile_ota OUTFILENAME=bad_apple_ota

    mkdir -p _build/ota
    cd Adafruit_nRF52_nrfutil
    #so, like, why does this work??
    python3 -m nordicsemi dfu genpkg --dev-type 0x0052 --application ../_build/bad_apple_ota.hex ../_build/ota/bad_apple_ota.zip
    cd -
    elif [ "$1" = "spiflash_ota" ];
then
    for f in src/bad_apple/resources/video*; do
        fileNr=${f#src/bad_apple/resources/video_}
        fileNr=${fileNr%\.h}
        echo $fileNr
        make OUTFILENAME=bad_apple_flash_ota_$fileNr EXTRAFLAGS=-DSPIFLASH=$fileNr

        mkdir -p _build/ota
        cd Adafruit_nRF52_nrfutil
        #so, like, why does this work??
        python3 -m nordicsemi dfu genpkg --dev-type 0x0052 --application ../_build/bad_apple_flash_ota_$fileNr.hex ../_build/ota/bad_apple_flash_ota$fileNr.zip
        cd -
    done
fi
