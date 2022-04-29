#!/bin/bash
echo "building application..."
make

echo "building driver..."
cd driver
make

echo "installing driver..."
sudo rmmod de2i-150-ioctl.ko
sudo rmmod de2i-150-ioctl-mock.ko
sudo insmod de2i-150-ioctl.ko
sudo insmod de2i-150-ioctl-mock.ko

echo "setup driver access..."
sudo chmod 666 /dev/stanley_pci
sudo chmod 666 /dev/stanley_pci_mock

echo "installation successfull!"
echo ""
echo "in order to run the application run:"
echo "    ./app/build/release/app"
echo ""
echo "in order to run the application for validation with a mock run:"
echo "    ./app/build/release/app /dev/stanley_pci_mock echo"
echo ""
echo "in order to run the application for validation on the board run:"
echo "    ./app/build/release/app /dev/stanley_pci echo"
#./app/build/release/app /dev/stanley_pci echo