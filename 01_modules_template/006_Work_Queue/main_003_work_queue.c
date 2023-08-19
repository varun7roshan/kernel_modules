#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

void my_work_handler(struct work_struct *work);

struct workqueue_struct *my_workqueue;

DECLARE_WORK(static_work_struct, my_work_handler);

/*void (*work_func_t)(struct work_struct *work);*/
void my_work_handler(struct work_struct *work)
{
	pr_info("---Top---\n");
	pr_info("Going to sleep for 10s\n");
	msleep(40000);
	pr_info("sleep done\n");
	pr_info("---End---\n");	
}

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	pr_info("calling create_workqueue()\n");
	my_workqueue = create_workqueue("OWN_WQ");

	pr_info("calling queue_work()\n");
	//int queue_work( struct workqueue_struct *wq, struct work_struct *work );
	queue_work(my_workqueue, &static_work_struct);

	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");
	pr_info("calling destroy_workqueue()\n");
	destroy_workqueue(my_workqueue);
	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
