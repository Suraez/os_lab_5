#include <linux/init.h> /* Needed for the macros */ 
#include <linux/module.h> /* Needed by all modules */ 
#include <linux/printk.h> /* Needed for pr_info() */ 
#include <linux/kthread.h>

static struct task_struct *my_thread;

static unsigned int objnum = 2000;

module_param(objnum, int, 0);


struct my_struct {
    unsigned int num[8];
};



static int thread_func(void *) {
    struct my_struct my_data;
    printk(KERN_INFO "Value of objnum is %u\n", objnum);
    printk(KERN_INFO "Size of my_struct: %zu bytes\n", sizeof(my_data));
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