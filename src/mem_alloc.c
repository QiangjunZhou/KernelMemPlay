#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

static int __init mem_alloc_init(void)
{
    struct page *page;
    void *vaddr;
    unsigned long paddr;
    int i;

    printk(KERN_ALERT "mem_alloc enter\n");

    // Allocate a physical page
    page = alloc_page(GFP_KERNEL);
    if (!page) {
        printk(KERN_ALERT "mem_alloc: Page allocation failed\n");
        return -ENOMEM;
    }

    // Get the physical address of the allocated page
    paddr = page_to_phys(page);
    printk("mem_alloc: Physical address of the allocated page: %lx\n", paddr);

    // Get the virtual address of the allocated page in kernel space
    vaddr = kmap(page);
    if (!vaddr) {
        __free_page(page);
        printk(KERN_ALERT "mem_alloc: kmap failed\n");
        return -ENOMEM;
    }

    printk("mem_alloc: Virtual address of the allocated page: %p\n", vaddr);

    // Fill the entire page with 0x55
    memset(vaddr, 0x55, PAGE_SIZE);

    // Verify the content of the page
    for (i = 0; i < PAGE_SIZE; i++) {
        if (((unsigned char *)vaddr)[i] != 0x55) {
            printk(KERN_ALERT "mem_alloc: Memory verification failed at byte %d\n", i);
            kunmap(page);
            __free_page(page);
            return -EFAULT;
        }
    }
    printk("mem_alloc: Memory filled with 0x55 successfully\n");

    // Unmap and free the allocated page
    kunmap(page);
    __free_page(page);

    return 0;
}

static void __exit mem_alloc_exit(void)
{
    printk(KERN_ALERT "mem_alloc exit\n");
}

module_init(mem_alloc_init);
module_exit(mem_alloc_exit);

MODULE_AUTHOR("QiangjunZhou");
MODULE_DESCRIPTION("A simple module to allocate a physical page, output addresses, and fill it with 0x55");
