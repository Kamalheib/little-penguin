obj-m += main.o

all:
ifndef KERNEL_PATH
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) modules
else
	make -C ${KERNEL_PATH} M=$(PWD) modules
endif

clean:
ifndef KERNEL_PATH
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) clean
else
	make -C ${KERNEL_PATH} M=$(PWD) clean
endif
