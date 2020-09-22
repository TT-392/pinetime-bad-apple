#!/bin/bash
#make
cd Adafruit_nRF52_nrfutil
#so, like, why does this work??
python3 -m nordicsemi dfu genpkg --dev-type 0x0052 --application ../_build/file.hex ../_build/pine.zip
cd -
scp _build/pine.zip emile@emile-thinkpad:~

ssh emile@emile-thinkpad "~/programming/wasp-os/tools/ota-dfu/dfu.py --legacy -z pine.zip -a D8:23:21:7B:C2:B3"
