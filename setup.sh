#!/bin/bash
make
cd driver
make
sudo insmod de2i-150-ioctl.ko
cd ..
sudo chmod 666 /dev/stanley_pci
make
./app/build/release/app /dev/stanley_pci echo