#include <linux/init.h>
#include <linux/module.h>

extern int server_get_sum(int a, int b);

static int client_init(void)
{
    printk("calling server_get_sum(1, 23)\n");
    printk("get sum= %d\n",server_get_sum(1, 23));
    printk("client_init done\n");
    return 0;
}

static void client_exit(void)
{
    printk("client_exit done.\n");
}

module_init(client_init);
module_exit(client_exit);

MODULE_AUTHOR("Wu Pengfei");
MODULE_DESCRIPTION("A test for using symbol exported by server.ko");
MODULE_LICENSE("GPL");
