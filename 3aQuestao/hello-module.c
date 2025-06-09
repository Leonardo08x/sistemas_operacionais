#define MODULE
#define LINUX


#include <linux/module.h> 
#include <linux/kernel.h>  

static int __init jesse(void) {
  printk("<1> Hello world 1.\n");
  return 0;
}

static void __exit we_need_to_cook(void) {
  printk(KERN_ALERT "Goodbye world 1.\n");
}

module_init(jesse);
module_exit(we_need_to_cook);

MODULE_LICENSE("GPL");
