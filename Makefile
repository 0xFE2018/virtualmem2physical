obj-m += my_mem.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
insmod:
	sudo insmod my_mem.ko
rmmod:
	sudo rmmod my_mem.ko

