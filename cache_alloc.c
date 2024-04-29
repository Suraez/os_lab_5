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

static struct kmem_cache *my_cache;

struct my_struct {
    unsigned int num[8];
};



static int thread_func(void *) {
    struct my_struct *ptr;
    struct my_struct my_data;
    struct timespec64 start_time, end_time;
    unsigned long elapsed_time;

    printk(KERN_INFO "Value of objnum is %u\n", objnum);
    printk(KERN_INFO "Size of my_struct: %zu bytes\n", sizeof(my_data));

    ktime_get_real_ts64(&start_time);

        // Allocate and free memory 'objnum' times
    for (int i = 0; i < objnum; i++) {
        // Allocate memory
        ptr = (struct my_struct *)kmem_cache_alloc(my_cache, GFP_KERNEL);
        if (!ptr) {
            printk(KERN_ERR "Memory allocation failed\n");
            return -ENOMEM;
        }

        // Free memory from cache
        kmem_cache_free(my_cache, ptr);
    }

    ktime_get_real_ts64(&end_time);

    elapsed_time = timespec64_to_ns(&end_time) - timespec64_to_ns(&start_time);
    printk(KERN_INFO "Time taken for memory allocation: %lu ns\n", elapsed_time);
    return 0;
}

static int __init cache_init(void) 
{ 

    // thread binding
    my_thread = kthread_create(thread_func, NULL, "page_thread");
    kthread_bind(my_thread, 0); // binding the thread to CPU core 0
    wake_up_process(my_thread); // start the thread

    // instantiating cache
    my_cache = kmem_cache_create("my_cache", sizeof(struct my_struct), 0, 0, NULL);
    if (!my_cache) {
        printk(KERN_ERR "Failed to create cache\n");
        return -ENOMEM;
    }

    return 0; 
} 
 
static void __exit cache_exit(void) 
{ 
    printk(KERN_INFO "Value of objnum is %u\n", objnum);

    // cache free
    if (my_cache) {
      kmem_cache_destroy(my_cache);  
    } 


} 
 
module_init(cache_init); 
module_exit(cache_exit); 
 
MODULE_LICENSE("GPL");