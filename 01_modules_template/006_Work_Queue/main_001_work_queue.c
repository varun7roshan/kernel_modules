#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/delay.h>
#include <linux/workqueue.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

void my_work_handler(struct work_struct *work);

/*static method: DECLARE_WORK(variable_name, callback_function);*/
DECLARE_WORK(my_work_struct, my_work_handler);

/*void (*work_func_t)(struct work_struct *work);*/
void my_work_handler(struct work_struct *work)
{
	pr_info("---Top---\n");
	pr_info("Going to sleep for 10s\n");
	msleep(10000);
	pr_info("sleep done\n");
	pr_info("---End---\n");	
}

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");
	
	//static inline bool schedule_work(struct work_struct *work)
	schedule_work(&my_work_struct);

	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");

	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
