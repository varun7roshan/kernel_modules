#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/jiffies.h>
#include <linux/timer.h>

#include <linux/ktime.h>
#include <linux/hrtimer.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

struct hrtimer my_hr_timer;

unsigned long ms = 1000;
int count = 60;

enum hrtimer_restart cb_hrtimer(struct hrtimer *ptimer)
{
	pr_info("Top - count : %d\n", count);
	count--;
	if(count)
	{
		pr_info("calling hrtimer_forward(ptimer, ms_to_ktime(ms))\n");
		hrtimer_forward_now(ptimer, ms_to_ktime(ms)); //this is need to be set, else worng time cb
		pr_info("returning : HRTIMER_RESTART\n");
		return HRTIMER_RESTART;
	}
	else
	{
		pr_info("returning : HRTIMER_NORESTART\n");
		return HRTIMER_NORESTART;
	}
}

static int __init init_load(void)
{
	pr_info("---Init - Top---\n");

#if 1
	/* HRTIMER_MODE_REL - did not give 500ms with count = 20, 10 seconds 
	*/
	pr_info("calling hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL)\n");	
	hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL); //relative to now
#else //ABS
	/*
		The correct way to use ABS:
		hrtimer_start(&dev->poll_hrt, last_timeout + ms_to_ktime(5), HRTIMER_MODE_ABS);
		>> Note here we have to pass the absolute value of time when to expire - in ABS mode
	*/
	pr_info("calling hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS)\n");	
	hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS); //Absolute Time Value
#endif

	my_hr_timer.function = cb_hrtimer;
#if 1
	pr_info("hrtimer_start(&my_hr_timer, ms_to_ktime(ms), HRTIMER_MODE_REL_PINNED)\n");
	hrtimer_start(&my_hr_timer, ms_to_ktime(ms), HRTIMER_MODE_REL_PINNED); //relative to now
#else
	pr_info("hrtimer_start(&my_hr_timer, ms_to_ktime(ms), HRTIMER_MODE_ABS_PINNED)\n");
	hrtimer_start(&my_hr_timer, ms_to_ktime(ms), HRTIMER_MODE_ABS_PINNED); //relative to now
#endif	

	pr_info("---Init - End---\n");
	return 0;
}

static void __exit exit_load(void)
{
	int ret = 0;
	pr_info("---Exit - Top---\n");
	pr_info("calling hrtimer_try_to_cancel()\n");

	ret = hrtimer_try_to_cancel(&my_hr_timer);
	if(ret)
	{
		pr_info("HR-Timer is active - Failed to cancel\n");
		pr_info("Force cancel the HR-Timer - calling hrtimer_cancle()\n");
		hrtimer_cancel(&my_hr_timer); //will just wait for the callback for finish execution
	}
	else
	{
		pr_info("HR-Timer was in-active - and is cancelled by hrtimer_try_to_cancel()\n");
	}
	pr_info("---Exit - End---\n");	
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
