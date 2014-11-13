#include <linux/init.h>
#include <linux/module.h>

int server_get_sum(int a, int b)
{
    printk("server_get_sum() done\n");
    return a+b;
}
EXPORT_SYMBOL_GPL(server_get_sum);


static int server_init(void)
{
    printk("server_init done\n");
    return 0;
}

static void server_exit(void)
{
    printk("server_exit done.\n");
}

module_init(server_init);
module_exit(server_exit);

MODULE_AUTHOR("Wu Pengfei");
MODULE_DESCRIPTION("A test for exporting symbol for clients");
MODULE_LICENSE("GPL");
