#!/bin/bash
make -f Makefile_ota
cd Adafruit_nRF52_nrfutil
#so, like, why does this work??
python3 -m nordicsemi dfu genpkg --dev-type 0x0052 --application ../_build/nrf52832_xxaa.hex ../_build/pine.zip
cd -
