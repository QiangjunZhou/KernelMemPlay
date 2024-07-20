#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/mmzone.h>
#include <linux/vmstat.h>
#include <linux/swap.h>
#include <linux/swapfile.h>
#include <linux/page-flags.h>
#include <linux/blk_types.h>

MODULE_LICENSE("Dual BSD/GPL");

static int mem_usage_init(void)
{
    unsigned long totalram, freeram, usedram, sharedram, bufferram, availram;
    unsigned long total_pages = totalram_pages();
    unsigned long free_pages = 0, used_pages = 0;
    unsigned long reserved_pages = 0, shared_pages = 0;
    unsigned long buff_cache_pages = 0, available_pages = 0;
    struct page *page;
    unsigned long pfn;
    unsigned long totalswap_pages = 0, freeswap_pages = 0; 
    unsigned long totalswap = 0, freeswap = 0, usedswap = 0;
    // compare
    struct sysinfo si;
    struct swap_info_struct *sis;

    printk(KERN_ALERT "mem_usage enter\n");

    // Use totalram_pages to get the total number of pages
    for (pfn = 0; pfn < total_pages; pfn++) {
        page = pfn_to_page(pfn);

        if (PageReserved(page)) {
            reserved_pages++;
        } else if (page_count(page) == 0) {
            free_pages++;
        } else {
            used_pages++;

            if (PageSwapCache(page) || PageSlab(page) || PageLRU(page)) {
                buff_cache_pages++;
                if (PageSwapCache(page)) {
                    sis = page_swap_info(page);
                    if (sis && (sis->flags & SWP_USED)) {
                        totalswap_pages += sis->pages;
                        freeswap_pages += sis->inuse_pages;
                    }
                }
            }

            if (PageAnon(page) && page_count(page) > 1) {
                shared_pages++;
            } else if (PageSwapBacked(page) && page_count(page) > 1) {
                shared_pages++;
            }
        }
    }

    available_pages = global_node_page_state(NR_FREE_PAGES) + buff_cache_pages + free_pages;

    totalram = total_pages << (PAGE_SHIFT - 10);
    usedram = used_pages << (PAGE_SHIFT - 10);
    freeram = free_pages << (PAGE_SHIFT - 10);
    sharedram = shared_pages << (PAGE_SHIFT - 10);
    // sharedram = global_node_page_state(NR_SHMEM) << (PAGE_SHIFT - 10);
    bufferram = buff_cache_pages << (PAGE_SHIFT - 10);
    availram = available_pages << (PAGE_SHIFT - 10);

    // Get swap info
    totalswap = totalswap_pages << (PAGE_SHIFT - 10);
    freeswap = freeswap_pages << (PAGE_SHIFT - 10);
    usedswap = totalswap - freeswap;

    printk("Method1:\n");
    printk("              total        used        free      shared  buff/cache   available\n");
    printk("Mem:     %10lu %10lu %10lu %10lu %10lu %10lu\n",
        totalram, usedram, freeram, sharedram, bufferram, availram);
    printk("Swap:    %10lu %10lu %10lu\n", totalswap, usedswap, freeswap);

    // Get memory info using si_meminfo
    si_meminfo(&si);

    totalram = si.totalram << (PAGE_SHIFT - 10);
    freeram = si.freeram << (PAGE_SHIFT - 10);
    sharedram = si.sharedram << (PAGE_SHIFT - 10);
    bufferram = si.bufferram << (PAGE_SHIFT - 10);
    availram = freeram + bufferram;
    usedram = totalram - availram;

    // Get swap info using si_swapinfo
    si_swapinfo(&si);
    totalswap = si.totalswap << (PAGE_SHIFT - 10);
    freeswap = si.freeswap << (PAGE_SHIFT - 10);
    usedswap = totalswap - freeswap;

    printk("Method2:\n");
    printk("              total        used        free      shared  buff/cache   available\n");
    printk("Mem:     %10lu %10lu %10lu %10lu %10lu %10lu\n",
           totalram, usedram, freeram, sharedram, bufferram, availram);
    printk("Swap:    %10lu %10lu %10lu\n", totalswap, usedswap, freeswap);

    return 0;
}

static void mem_usage_exit(void)
{
    printk(KERN_ALERT "mem_usage exit\n");
}

module_init(mem_usage_init);
module_exit(mem_usage_exit);

MODULE_AUTHOR("QiangjunZhou");
MODULE_DESCRIPTION("A simple module to get memory usage using two different methods");
