#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

void work_handler(struct work_struct *pwork_struct);

/*creates a variable of type : struct wait_queue_head*/
DECLARE_WAIT_QUEUE_HEAD(HEAD_wait_queue);

DECLARE_WORK(my_work_struct, work_handler);

int condition = 0;

void work_handler(struct work_struct *pwork_struct)
{
	pr_info("--Top--\n");
	pr_info("Sleep for 10s\n");
	msleep(10000);

#if 1 
	condition = 1; //condition has to become true
#else
	/*Tested : though wake_up_interruptible() is called - the blocked thread on
		wait_event_interruptible() is not returned - as the condition is FALSE
	*/
	condition = 0; //Testing
#endif
	pr_info("call to wake_up_interruptible() and condition is set to :%d\n", condition);
	//wake_up_interruptible(struct wait_queue_head *wait_queue_head)
#if 1
	wake_up_interruptible(&HEAD_wait_queue);
#else
	/*wake_up_interruptible() has to be called - just setting the condition to TRUE is not
		sufficient - as the wait is on both (1) wait queue and (2) condition
	*/
	pr_info("wake_up_interruptible() is not called\n");
#endif
	pr_info("--End--\n");	
}

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	pr_info("call to schedule_work()\n");
	schedule_work(&my_work_struct);

	pr_info("calling wait_event_interruptible()\n");
	/*Note here - we need to pass the variable name : HEAD_wait_queue - internally in the
		MACRO - & is used to get the address - This way of coding it not good*/
	wait_event_interruptible(HEAD_wait_queue, condition != 0);
	pr_info("return from wait_event_interruptible(), condition : %d\n", condition);
	
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
