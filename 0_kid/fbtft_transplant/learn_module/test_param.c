#include <linux/init.h>
#include <linux/module.h>

static char *name = "Wu Pengfei";
module_param(name, charp, 0);
MODULE_PARM_DESC(name, "Name of Author(default:Wu Pengfei)");

static uint age;
module_param(age, uint, 0);
MODULE_PARM_DESC(age, "Age of Author");

static int salary;
module_param(salary, int, 0);
MODULE_PARM_DESC(salary, "Salary of Author");

static int test_param_init(void)
{
    printk("name = %s\n",name);
    printk("age = %u\n",age);
    printk("salary = %d\n",salary);
    printk("test_param_init done\n");
    return 0;
}

static void test_param_exit(void)
{
    printk("test_param_exit done.\n");
}

module_init(test_param_init);
module_exit(test_param_exit);

MODULE_AUTHOR("Wu Pengfei");
MODULE_DESCRIPTION("A test for passing module paramter");
MODULE_LICENSE("GPL");
