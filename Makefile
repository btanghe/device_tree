obj-m:= devicetree.o

ALL:
	make -C /home/bta/udoo/linux-imx ARCH=arm CROSS_COMPILE=make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- M=`pwd` modules
