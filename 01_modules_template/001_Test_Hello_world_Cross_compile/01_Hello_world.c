#include <linux/module.h>

static int __init hello_init(void)
{
	pr_info("Starting with Pi Again - from kernel Helper Functions\n");
	pr_info("Hello Raspberry Pi - From Cross Compiled\n");
	printk("From printk - Hello Raspberry Pi - From Cross Compiled\n");

	pr_info("Now we can start coding - but need to figure out the Device Tree Overlays\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("BYEEEE from Cross Compiled\n");
	printk("printk - BYEEEE from Cross Compiled\n");

	pr_info("BYEE for now - we can start coding - but need to figure out the Device Tree Overlays\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Testing Hello World - Cross Compile");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");

