#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");

static int pid;

static int vma_traverse_init(void)
{
    struct task_struct *task;
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    char perms[5];  // Permissions: rwxp
    unsigned long inode;
    char *pathname = NULL;
    char *buf = (char *)__get_free_page(GFP_KERNEL);

    if (!buf) {
        printk(KERN_ALERT "Failed to allocate memory for pathname buffer.\n");
        return -ENOMEM;
    }

    if (pid == 0) {
        pid = current->pid;
    }

    printk(KERN_ALERT "vma_traverse enter\n");
	printk("pid to be traverse: %d\n", pid);

    for_each_process(task) {
        if (pid && task->pid == pid)  {
            mm = task->mm;
            if (!mm) {
                printk(KERN_ALERT "No memory management structure found for PID %d.\n", pid);
                return -EFAULT;
            }

            for (vma = mm->mmap; vma; vma = vma->vm_next) {
                snprintf(perms, sizeof(perms), "%c%c%c%c",
                         vma->vm_flags & VM_READ ? 'r' : '-',
                         vma->vm_flags & VM_WRITE ? 'w' : '-',
                         vma->vm_flags & VM_EXEC ? 'x' : '-',
                         vma->vm_flags & VM_SHARED ? 's' : 'p');

                if (vma->vm_file && vma->vm_file->f_inode) {
                    inode = vma->vm_file->f_inode->i_ino;
                    pathname = d_path(&vma->vm_file->f_path, buf, PAGE_SIZE);
                } else {
                    inode = 0;
                    pathname = NULL;
                }

                printk(KERN_INFO "%08lx-%08lx %s %08lx %02x:%02x %lu %s\n",
                       vma->vm_start, vma->vm_end, perms, vma->vm_pgoff << PAGE_SHIFT,
                       vma->vm_file ? MAJOR(vma->vm_file->f_inode->i_sb->s_dev) : 0,
                       vma->vm_file ? MINOR(vma->vm_file->f_inode->i_sb->s_dev) : 0,
                       inode, pathname ? pathname : "");

                if (pathname) {
                    buf[0] = '\0'; // Clear buffer for next use
                }
            }

            break;
        }
    }

    if (!task) {
        printk(KERN_ALERT "Process with PID %d not found.\n", pid);
        return -ESRCH;
    }

    return 0;
}

static void vma_traverse_exit(void)
{
    printk(KERN_ALERT "vma_traverse exit\n");
}

module_init(vma_traverse_init);
module_exit(vma_traverse_exit);
module_param(pid, int, S_IRUGO);
MODULE_PARM_DESC(pid, "The PID of the process to traverse (default: current process)");

MODULE_AUTHOR("QiangjunZhou");
MODULE_DESCRIPTION("A simple module to traverse virtual address space memory layout of a process (default: current process)");
