##########################################################################################
struct wait_queue_head {
  spinlock_t    lock;
  struct list_head  head;
};

>> Notice here below we are creating a "WAIT_QUEUE_HEAD" and not the actual WAIT QUEUE
This WAIT_QUEUE_HEAD - has field "struct list_head head" - which will hold the linked
list of Task Lists - waiting on this WAIT_QUEUE_HEAD
>> So where is this "task_list" - When we call wait_event_interruptible() - it will
create a variable of type - "struct wait_queue_t"
struct __wait_queue
{
  unsigned int flags;
  void *private;
  wait_queue_func_t func;
  struct list_head task_list; --> This is the NODE which gets inserted into WAIT_QUEUE_HEAD
};

- So if we have a read() - and every process calls our read() - and if the IO data is not
avaiable yet - wait_event_interruptible(wait_queue_HEAD, condition) is called - Note that
HEAD is passed - in wait_event_interruptible() - it creates a "struct __wait_queue" and
adds this wait queue - member "struct list_head task_list;" into the HEAD
- So there is only 1 WAIT QUEUE HEAD - and for each process - struct wait_queue_t is
created and inserted into the HEAD of ours
##########################################################################################
------------------- STATIC ----------------------
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

void work_handler(struct work_struct *pwork_struct);

/*creates a variable of type : struct wait_queue_head*/
DECLARE_WAIT_QUEUE_HEAD(HEAD_wait_queue);

DECLARE_WORK(my_work_struct, work_handler);

int condition = 0;

void work_handler(struct work_struct *pwork_struct)
{
	condition = 1;
	wake_up_interruptible(&HEAD_wait_queue);
}

static int __init init_load(void)
{
  schedule_work(&my_work_struct);
	//will be blocked / wait - till the condition is TRUE and wake_up() is called
  wait_event_interruptible(HEAD_wait_queue, condition != 0);
  return 0;
}

##########################################################################################
------------------------DYNAMIC-----------------------

wait_queue_head_t wait_queue_etx;

int wait_queue_flag = 0;

init_waitqueue_head(&wait_queue_etx);

- then we can used wait_event() and wake_up() normally
##########################################################################################
Wait queue
----------
- Wait queues are essentially used to process blocked I/O, to wait for particular 
conditions to be true, and to sense data or resource availability. 
- To understand how they work, let's have a look at their structure in include/linux/wait.h:

struct __wait_queue 
{
	unsigned int flags;
	#define WQ_FLAG_EXCLUSIVE 0x01
	void *private;
	wait_queue_func_t func;
	struct list_head task_list;
};

- Let's pay attention to the "task_list" field. As you can see, it is a list. 
- Every process you want to put to sleep is queued in that list (hence the name wait queue)
and put into a sleep state until a condition becomes true. 

- The wait queue can be seen as nothing but a simple list of processes and a lock.

- The functions you will always face when dealing with wait queues are:

Static declaration:
DECLARE_WAIT_QUEUE_HEAD(name)

Dynamic declaration:
wait_queue_head_t my_wait_queue;
init_waitqueue_head(&my_wait_queue);

Blocking:
/*
* block the current task (process) in the wait queue if
* CONDITION is false
*/
int wait_event_interruptible(wait_queue_head_t q, CONDITION);

Unblocking:
/*
* wake up one process sleeping in the wait queue if
* CONDITION above has become true
*/
void wake_up_interruptible(wait_queue_head_t *q);

- "wait_event_interruptible()" does not continuously poll, but simply evaluates the
condition when it is called. If the condition is false, the process is put into a
TASK_INTERRUPTIBLE state and removed from the run queue. 

- The condition is then only rechecked each time you call "wake_up_interruptible()" in 
the wait queue. If the condition is true when "wake_up_interruptible()" runs, a process 
in the wait queue will be awakened, and its state set to TASK_RUNNING. 

- Processes are awakened in the order in which they are put to sleep. 
To awaken all processes waiting in the queue, you should use "wake_up_interruptible_all()".

- In fact, the main functions are wait_event, wake_up, and wake_up_all.
- They are used with processes in the queue in an exclusive (uninterruptible) wait, 
since they can't be interrupted by the signal. 
- They should be used only for critical tasks. 

- Interruptible functions are just optional (but recommended). 
- Since they can be interrupted by signals, you should check their return value. 
- A nonzero value means your sleep has been interrupted by some sort of signal, and the 
driver should return
ERESTARTSYS.

- If something has called wake_up() or wake_up_interruptible() and the condition is still
FALSE, then nothing will happen. 
- Without wake_up (or wake_up_interuptible), processes will never be awakened

##########################################################################################
struct wait_queue_head {
  spinlock_t    lock;
  struct list_head  head;
};

>> Notice here below we are creating a "WAIT_QUEUE_HEAD" and not the actual WAIT QUEUE
This WAIT_QUEUE_HEAD - has field "struct list_head head" - which will hold the linked
list of Task Lists - waiting on this WAIT_QUEUE_HEAD
>> So where is this "task_list" - When we call wait_event_interruptible() - it will
create a variable of type - "struct wait_queue_t"
struct __wait_queue
{
  unsigned int flags;
  void *private;
  wait_queue_func_t func;
  struct list_head task_list; --> This is the NODE which gets inserted into WAIT_QUEUE_HEAD
};

- So if we have a read() - and every process calls our read() - and if the IO data is not
avaiable yet - wait_event_interruptible(wait_queue_HEAD, condition) is called - Note that
HEAD is passed - in wait_event_interruptible() - it creates a "struct __wait_queue" and
adds this wait queue - member "struct list_head task_list;" into the HEAD
- So there is only 1 WAIT QUEUE HEAD - and for each process - struct wait_queue_t is
created and inserted into the HEAD of ours

static DECLARE_WAIT_QUEUE_HEAD(my_wq);

static int condition = 0;

/* declare a work queue*/
static struct work_struct wrk;

static void work_handler(struct work_struct *work)
{
	msleep(5000);
	condition = 1;
	wake_up_interruptible(&my_wq);
}

static int __init my_init(void)
{
	printk("Wait queue example\n");
	INIT_WORK(&wrk, work_handler);
	schedule_work(&wrk);

	printk("Going to sleep %s\n", __FUNCTION__);
	/* Since we have started a work queue - it run from another Thread context
		 This Thread - process - which process - the "insmod" loadind process - will
		 be blocked due to call to wait_event_interruptible() - once the condition
		 is set to true and wake_up() is called - here it will be unblocked
	*/
	wait_event_interruptible(my_wq, condition != 0);
	pr_info("woken up by the work job\n");
	return 0;
}

##########################################################################################
#define DECLARE_WAIT_QUEUE_HEAD(name) \
  struct wait_queue_head name = __WAIT_QUEUE_HEAD_INITIALIZER(name)

struct wait_queue_head {
  spinlock_t    lock;
  struct list_head  head;
};


##########################################################################################

There are 3 important steps in Waitqueue.

Initializing Waitqueue
Queuing (Put the Task to sleep until the event comes)
Waking Up Queued Task

##########################################################################################
Initializing Waitqueue
Use this Header file for Waitqueue (include /linux/wait.h). There are two ways to initialize the waitqueue.

Static method
Dynamic method
------------------------------------------------------------------------------------------
- Static Method

DECLARE_WAIT_QUEUE_HEAD(wq);

- Where the “wq” is the name of the queue on which task will be put to sleep.
------------------------------------------------------------------------------------------
- Dynamic Method

wait_queue_head_t wq;
init_waitqueue_head (&wq);

- You can create a waitqueue using any one of the above methods. Other operations are common for both static and dynamic method except the way we create the waitqueue.
------------------------------------------------------------------------------------------
- Queuing
- Once the wait queue is declared and initialized, a process may use it to go to sleep. There are several macros are available for different uses. We will see each one by one.

wait_event
wait_event_timeout
wait_event_cmd
wait_event_interruptible
wait_event_interruptible_timeout
wait_event_killable

Note:
- Old kernel versions used the functions sleep_on() and interruptible_sleep_on(), but those two functions can introduce bad race conditions and should not be used.

Whenever we use the above one of the macro, it will add that task to the waitqueue which is created by us. Then it will wait for the event.
------------------------------------------------------------------------------------------
wait_event
sleep until a condition gets true.

wait_event(wq, condition);

wq – the waitqueue to wait on
condition – a C expression for the event to wait for

The process is put to sleep (TASK_UNINTERRUPTIBLE) until the condition evaluates to true. The condition is checked each time the waitqueue wq is woken up.
------------------------------------------------------------------------------------------
wait_event_timeout
sleep until a condition gets true or a timeout elapses

wait_event_timeout(wq, condition, timeout);

wq –  the waitqueue to wait on
condtion – a C expression for the event to wait for
timeout –  timeout, in jiffies

The process is put to sleep (TASK_UNINTERRUPTIBLE) until the condition evaluates to true or timeout elapses. The condition is checked each time the waitqueue wq is woken up.

It returns 0 if the condition evaluated to false after the timeout elapsed, 1 if the condition evaluated to true after the timeout elapsed, or the remaining jiffies (at least 1) if the condition evaluated to true before the timeout elapsed.
------------------------------------------------------------------------------------------
wait_event_cmd
sleep until a condition gets true

wait_event_cmd(wq, condition, cmd1, cmd2);

wq –  the waitqueue to wait on
condtion – a C expression for the event to wait for
cmd1 – the command will be executed before sleep
cmd2 – the command will be executed after sleep

The process is put to sleep (TASK_UNINTERRUPTIBLE) until the condition evaluates to true. The condition is checked each time the waitqueue wq is woken up.
------------------------------------------------------------------------------------------
wait_event_interruptible
sleep until a condition gets true

wait_event_interruptible(wq, condition);

wq –  the waitqueue to wait on
condtion – a C expression for the event to wait for

The process is put to sleep (TASK_INTERRUPTIBLE) until the condition evaluates to true or a signal is received. The condition is checked each time the waitqueue wq is woken up.

The function will return -ERESTARTSYS if it was interrupted by a signal and 0 if condition evaluated to true.
------------------------------------------------------------------------------------------
wait_event_interruptible_timeout
sleep until a condition gets true or a timeout elapses

wait_event_interruptible_timeout(wq, condition, timeout);

wq –  the waitqueue to wait on
condtion – a C expression for the event to wait for
timeout –  timeout, in jiffies

The process is put to sleep (TASK_INTERRUPTIBLE) until the condition evaluates to true or a signal is received or timeout elapsed. The condition is checked each time the waitqueue wq is woken up.

It returns, 0 if the condition evaluated to false after the timeout elapsed, 1 if the condition evaluated to true after the timeout elapsed, the remaining jiffies (at least 1) if the condition evaluated to true before the timeout elapsed, or -ERESTARTSYS if it was interrupted by a signal.
------------------------------------------------------------------------------------------
wait_event_killable
sleep until a condition gets true

wait_event_killable(wq, condition);
wq –  the waitqueue to wait on
condtion – a C expression for the event to wait for

The process is put to sleep (TASK_KILLABLE) until the condition evaluates to true or a signal is received. The condition is checked each time the waitqueue wq is woken up.

The function will return -ERESTARTSYS if it was interrupted by a signal and 0 if condition evaluated to true.
------------------------------------------------------------------------------------------
Waking Up Queued Task
When some Tasks are in sleep mode because of the waitqueue, then we can use the below function to wake up those tasks.

wake_up
wake_up_all
wake_up_interruptible
wake_up_sync and wake_up_interruptible_sync
wake_up
wakes up only one process from the wait queue which is in non-interruptible sleep.
------------------------------------------------------------------------------------------
wake_up(&wq);

wq – the waitqueue to wake up
------------------------------------------------------------------------------------------
wake_up_all
wakes up all the processes on the wait queue

wake_up_all(&wq);

wq – the waitqueue to wake up
------------------------------------------------------------------------------------------
wake_up_interruptible
wakes up only one process from the wait queue that is in interruptible sleep

wake_up_interruptible(&wq);

wq – the waitqueue to wake up
------------------------------------------------------------------------------------------
wake_up_sync and wake_up_interruptible_sync
wake_up_sync(&wq);
wake_up_interruptible_sync(&wq);
------------------------------------------------------------------------------------------
- Normally, a "wake_up" call can cause an "immediate reschedule" to happen, meaning that 
other processes might run before wake_up returns. 
- The "synchronous" variants instead make any awakened processes runnable but do not 
reschedule the CPU. 
- This is used to avoid rescheduling when the current process is known to be going to 
sleep, thus forcing a reschedule anyway. 
- Note that awakened processes could run immediately on a different processor, so these 
functions should not be expected to provide mutual exclusion.
##########################################################################################

