#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/delay.h>
#include <linux/kthread.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

struct task_struct *thread_1;
const int kt_num = 1;
/* 
	int (* threadfn(void *data)
*/
int my_thread(void *data)
{
	int count = 0;
	pr_info("--Top--\n");
	while(!kthread_should_stop())
	{
		pr_info("Going to sleep ...5s:Time elapsed : %d seconds\n", count*5);
		msleep(5000);
		count++;
	}
	
	pr_info("kthread_should_stop() returned TRUE : 1 - hence exiting\n");
	return 0; //or do_exit()
}


static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	/*
		struct task_struct * kthread_create (int (* threadfn(void *data), void *data, 
																				const char namefmt[], ...);
	*/
	thread_1 = kthread_create(my_thread, NULL, "vaurn-kthread:%d", kt_num);
	if(thread_1)
	{
		pr_info("kthread_create() was successful - now run it / wake it up\n");
		/*int wake_up_process (struct task_struct * p);*/
		wake_up_process(thread_1);
	}
	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");

	pr_info("call to kthread_stop(thread_1);\n");
	/*int kthread_stop ( struct task_struct *k);*/
	kthread_stop(thread_1);
	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
