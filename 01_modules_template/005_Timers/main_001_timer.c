#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/jiffies.h>
#include <linux/timer.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

//not available in rpi-4.19.y
#define TIMER_INIT_FLAGS  (TIMER_DEFERRABLE | TIMER_PINNED | TIMER_IRQSAFE)

struct timer_list my_timer;

unsigned long data = 10;

void cb_timer(struct timer_list *ptimer)
{
	if(data)
	{
		pr_info("calling mod_timer() again since data : %ld\n", data);
		if(mod_timer(&my_timer, jiffies + msecs_to_jiffies(500)))
		{
			pr_info("Error return from mod_timer()\n");
		}
		data--;
	}
}

static int __init init_load(void)
{
	pr_info("---Init - Top---\n");
	pr_info("calling timer_setup()\n");
	timer_setup(&my_timer, cb_timer, TIMER_INIT_FLAGS);
	
	pr_info("calling mod_timer()\n");
	if(mod_timer(&my_timer, jiffies + msecs_to_jiffies(500)))//mod_timer() starts the timer
	{
		pr_info("Error return from mod_timer()\n");
	}

	pr_info("---Init - End---\n");
	return 0;
}

static void __exit exit_load(void)
{
	pr_info("---Exit - Top---\n");
	pr_info("calling del_timer()\n");
	del_timer(&my_timer);
	pr_info("---Exit - End---\n");	
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
