obj-m += de2i-150-ioctl.o
obj-m += de2i-150-ioctl-mock.o
# obj-m += char-lkm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
