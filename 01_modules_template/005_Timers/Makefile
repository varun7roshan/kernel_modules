IP_ADDR=192.168.0.110
MODULE_KO_NAME=HRT_002_timer_module
#C_FILES=main_001_timer.o
C_FILES=main_002_HRT_timer.o

obj-m := $(MODULE_KO_NAME).o # this has to be mateched with below "-objs" and MODULE.ko
$(MODULE_KO_NAME)-objs := $(C_FILES) #C-File name here

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR_TARGET=/home/varun/Raspberry_PI/kernel/linux
KERN_DIR_HOST=/lib/modules/$(shell uname -r)/build/


all:
	#make -C $(KERN_DIR_HOST) M=$(PWD) modules
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) modules
	scp *.ko pi@$(IP_ADDR):~/

clean:
	#make -C $(KERN_DIR_HOST) M=$(PWD) clean
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) clean

target:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) modules

clean_target:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) clean


