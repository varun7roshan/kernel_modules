#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/moduleparam.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

char *ptr = NULL;

char *addr = 0xe81dce6c;

module_param(addr, charp, S_IRUSR|S_IWUSR);

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	pr_info("----- Driver -1 - need addr parameter\n");
	if(addr)
	{
		pr_info("addr is 0x%x\n", addr);
		ptr = (char *)addr;
		pr_info("Printing addr : %s\n", ptr);
	}
	else
		pr_info("addr is NULL\n");

	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");
	pr_info("-- Driver 2 - Exiting ------\n");

	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
