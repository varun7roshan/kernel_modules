#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

struct work_struct *wq_malloc;

/*void (*work_func_t)(struct work_struct *work);*/
void my_work_handler(struct work_struct *work)
{
	pr_info("---Top---\n");
	pr_info("Going to sleep for 10s\n");
	msleep(30000);
	pr_info("sleep done\n");
	pr_info("---End---\n");	
}

static int __init init_load(void)
{
	pr_info("-- INIT - Top --\n");

	wq_malloc = kmalloc(sizeof(struct work_struct), GFP_KERNEL);	
	if(!wq_malloc)
		pr_info("ERROR:kmalloc error\n");

	INIT_WORK(wq_malloc, my_work_handler);
	//static inline bool schedule_work(struct work_struct *work)
	schedule_work(wq_malloc);

	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("-- EXIT - Top --\n");
	
#if 1
	pr_info("call to flush_scheduled_work()\n");
	/*Also flush_scheduled_work() gets blocked till wq is finished in rmmod*/
	//for gloabal wq to finish - better to call cancel_work_sync()- that will also block
	flush_scheduled_work();
	pr_info("return from flush_scheduled_work()\n");
#else
	/*so when sudo rmmod is invoked - rmmod only gets blocked - till wq finished*/
	pr_info("call to cancel_work_sync() - Block till wq is finished\n");
	cancel_work_sync(wq_malloc);
	pr_info("return from cancel_work_sync() - now we can kfree()\n");
#endif
	kfree(wq_malloc);
	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
