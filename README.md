# KernelMemPlay

## Overview

This repository contains four Linux kernel modules designed to help beginners (myself included) understand the memory management aspects of the Linux kernel. Each module focuses on a different area of memory management, from examining physical memory usage to creating and managing slab caches.

## Modules

### 1. mem_usage

This module traverses the `mem_map` array to obtain physical memory usage, providing information similar to the output of the `free` command.

### 2. vma_traverse

This module traverses the virtual address space memory layout of a process.

### 3. mem_alloc

This module utilizes the `alloc_page()` function to allocate a physical page. It outputs the physical address of the page, the virtual address of the page in the kernel space, and fills the entire physical page with 0x55.

### 4. slab_alloc

This module creates a slab descriptor named "test object" with a size of 20 bytes, an alignment of 8 bytes, and flags set to 0. It then allocates a free object from this slab descriptor and examines the current system's slabs.
