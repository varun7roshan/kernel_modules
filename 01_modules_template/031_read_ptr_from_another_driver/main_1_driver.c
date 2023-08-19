#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/slab.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

//#define STR_DRV_1 "This String is part of Driver-1 - and this string is in MACRO"

char *str_of_driver_1 = "DRIVER-1";

char *ptr;

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	pr_info("Address of str_of_driver_1:%p\n", str_of_driver_1);
	pr_info("This is Print from Driver-1: %s\n", str_of_driver_1);

	ptr = kzalloc(10, GFP_KERNEL);
	memcpy(ptr, str_of_driver_1, 9);

	pr_info("Dynamically allocated memory : %p\n", ptr);
	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");
	pr_info("-- DRIVER-1 - EXITING\n");

	kzfree(ptr);
	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
