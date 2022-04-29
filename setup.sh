#!/bin/bash
make
cd driver
make
sudo rmmod de2i-150-ioctl.ko
sudo insmod de2i-150-ioctl.ko
cd ..
sudo chmod 666 /dev/stanley_pci
#./app/build/release/app /dev/stanley_pci echo