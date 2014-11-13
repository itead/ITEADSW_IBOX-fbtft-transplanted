#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk(KERN_WARNING"hello_init done.\n");
    return 0;
}

static void __init hello_exit(void)
{
    printk(KERN_WARNING"hello_exit done.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Wu Pengfei");
MODULE_DESCRIPTION("A test for learning module design.");
MODULE_LICENSE("GPL");
