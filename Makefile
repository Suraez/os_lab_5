obj-m += page_alloc.o 
obj-m += kmem_alloc.o
obj-m += cache_alloc.o
all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 
 
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean