#include <linux/init.h> /* Needed for the macros */ 
#include <linux/module.h> /* Needed by all modules */ 
#include <linux/printk.h> /* Needed for pr_info() */ 
#include <linux/kthread.h>
#include <linux/slab.h> // Needed for alloc_pages and __free_pages functions
#include <linux/time.h> // Needed for timespec and ktime_get functions
#include <linux/timekeeping.h>

static struct task_struct *my_thread;

static unsigned int objnum = 2000;

module_param(objnum, int, 0);


struct my_struct {
    unsigned int num[8];
};



static int thread_func(void *) {
    struct my_struct my_data;
    struct timespec64 start_time, end_time;
    unsigned long elapsed_time;

    printk(KERN_INFO "Value of objnum is %u\n", objnum);
    printk(KERN_INFO "Size of my_struct: %zu bytes\n", sizeof(my_data));

    unsigned long pages_needed = (objnum * sizeof(struct my_struct) + PAGE_SIZE - 1) / PAGE_SIZE;

    ktime_get_real_ts64(&start_time);

    // allocating pages
    struct page *pages = alloc_pages(GFP_KERNEL, get_order(pages_needed * PAGE_SIZE));
    // deallocating pages
    __free_pages(pages, get_order(pages_needed * PAGE_SIZE));

    ktime_get_real_ts64(&end_time);

    elapsed_time = timespec64_to_ns(&end_time) - timespec64_to_ns(&start_time);
    printk(KERN_INFO "Time taken for memory allocation: %lu ns\n", elapsed_time);
    return 0;
}

static int __init page_init(void) 
{ 
    // printk(KERN_INFO "page module loaded\n");

    // thread binding
    my_thread = kthread_create(thread_func, NULL, "page_thread");
    kthread_bind(my_thread, 0); // binding the thread to CPU core 0
    wake_up_process(my_thread); // start the thread

    return 0; 
} 
 
static void __exit page_exit(void) 
{ 
    printk(KERN_INFO "Value of objnum is %u\n", objnum);

} 
 
module_init(page_init); 
module_exit(page_exit); 
 
MODULE_LICENSE("GPL");