STEP 1:

git clone https://github.com/PolykarposV/FLEX-RV-Emulator.git

STEP 2:

cd ./FLEX-RV-Emulator

STEP 3:

mv CFU-Playground/ Container/

STEP 4:

docker build -t cfu ./Container

STEP 5:

docker run --rm --privileged --device=/dev/bus/usb/001/004 -ti -v /home/pvergos/tools:/workspace/tools:ro cfu bash

STEP 6:

cd CFU-Playground/proj/demo

STEP 7:

make TARGET=digilent_arty_s7 USE_VIVADO=1 EXTRA_LITEX_ARGS="--cpu-type=serv --cpu-variant=cfu" SERV=1 TTY=/dev/ttyUSB1 prog

STEP 8:

make TARGET=digilent_arty_s7 USE_VIVADO=1 EXTRA_LITEX_ARGS="--cpu-type=serv --cpu-variant=cfu" SERV=1 TTY=/dev/ttyUSB1 load

STEP 9:

_press_ Enter

STEP 10:

_type_ reboot and _press_ Enter

STEP 11:

_press_ d
