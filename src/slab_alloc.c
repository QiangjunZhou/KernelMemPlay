#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/slab_def.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/vmstat.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/delay.h> 

MODULE_LICENSE("Dual BSD/GPL");

/* The list of all slab caches on the system */
extern struct list_head slab_caches;

static struct kmem_cache *test_cache;
static void *test_object;

static int __init slab_alloc_init(void)
{
    struct kmem_cache *cache;

    printk(KERN_ALERT "slab_alloc enter\n");

    // Create a slab cache named "test object"
    test_cache = kmem_cache_create("test_object", 20, 8, 0, NULL);
    if (!test_cache) {
        printk(KERN_ALERT "slab_alloc: Cache creation failed\n");
        return -ENOMEM;
    }

    // Allocate an object from the slab cache
    test_object = kmem_cache_alloc(test_cache, GFP_KERNEL);
    if (!test_object) {
        kmem_cache_destroy(test_cache);
        printk(KERN_ALERT "slab_alloc: Object allocation failed\n");
        return -ENOMEM;
    }

    printk("slab_alloc: Allocated object from slab cache 'test_object': %p\n", test_object);

    printk("Listing all slabs:\n");

    // Access the global list of kmem_cache structures
    list_for_each_entry(cache, &slab_caches, list) {
        printk(KERN_INFO "Name: %s, Size: %u\n",
               cache->name, cache->size);
    }

    return 0;
}

static void __exit slab_alloc_exit(void)
{
    printk(KERN_ALERT "slab_alloc exit\n");

    // Free the allocated object
    if (test_object) {
        kmem_cache_free(test_cache, test_object);
    }

    // Destroy the slab cache
    if (test_cache) {
        kmem_cache_destroy(test_cache);
    }
}

module_init(slab_alloc_init);
module_exit(slab_alloc_exit);

MODULE_AUTHOR("QiangjunZhou");
MODULE_DESCRIPTION("A simple module to create a slab cache, allocate an object, and list current slabs");
