#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/completion.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

struct task_struct *thread_1;
const int kt_num = 1;

/*DECLARE_COMPLETION(var_name) 
	struct completion work = COMPLETION_INITIALIZER(work)
*/
DECLARE_COMPLETION(my_complete);

/*int (* threadfn(void *data)*/
int my_thread(void *data)
{
	int count = 5;
	
	pr_info("--Top--\n");

	while(!kthread_should_stop())
	{
		pr_info("Going to sleep 5-sec: Time elapsed:%d\n", (count-5) * 5);
		msleep(5000);
		if(!count)
		{
			//after 25 seconds - complete() is signaled
			/*void complete (struct completion * x);*/
			complete(&my_complete);
		}
		count--;
	}

	pr_info("--End--\n");
}

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	/*kthread_run (threadfn, data, namefmt, ...);*/
	thread_1 = kthread_run(my_thread, NULL, "varun-com-kthread:%d", kt_num);
	if(!thread_1)
		pr_info("ERROR:return from kthread_run()\n");


	pr_info("kthread started - call to wait_for_completion()\n");
	/*void wait_for_completion (struct completion * x);*/
	wait_for_completion(&my_complete);

	pr_info("return from wait_for_completion()\n");
	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");
	
	pr_info("call to kthread_stop()\n");
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
