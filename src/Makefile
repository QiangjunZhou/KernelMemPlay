obj-m := mem_usage.o vma_traverse.o mem_alloc.o slab_alloc.o
CURRENT_PATH := $(shell pwd)
KERNEL_PATH := $(HOME)/Kernel/$(VERSION)/$(ARCH)

all:
		make -C $(KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
		make -C $(KERNEL_PATH) M=$(CURRENT_PATH) clean
