##########################################################################################
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/completion.h>

struct task_struct *thread_1;
const int kt_num = 1;

/*DECLARE_COMPLETION(var_name) 
	struct completion work = COMPLETION_INITIALIZER(work)
*/
DECLARE_COMPLETION(my_complete); //-----------------(1)

/*int (* threadfn(void *data)*/
int my_thread(void *data)
{
  int count = 5;
  while(!kthread_should_stop())
  {
    msleep(5000);
    if(!count)
    {
      //after 25 seconds - complete() is signaled
      /*void complete (struct completion * x);*/
      complete(&my_complete); //-------------------------------(3)
    }
    count--;
  }
}

static int __init init_load(void)
{
  /*kthread_run (threadfn, data, namefmt, ...);*/
  thread_1 = kthread_run(my_thread, NULL, "varun-com-kthread:%d", kt_num);
  if(!thread_1)
    pr_info("ERROR:return from kthread_run()\n");
  
	/*void wait_for_completion (struct completion * x);*/
  wait_for_completion(&my_complete); //---------------------(2)
  return 0;
}

static void __exit exit_load(void)
{
  kthread_stop(thread_1);
}

##########################################################################################


##########################################################################################


##########################################################################################
																Completion
>> Implemented using WAIT QUEUES
- Completion, the name itself says. When we want to notify or wake up some thread or 
something when we finished some work, then we can use completion. 
- We’ll take one situation. We want to wait for one thread for something to run. 
- Until that time that thread has to sleep. Once that process finished then we need to 
wake up that thread that is sleeping. 
- We can do this by using completion without race conditions.

- These completions are a synchronization mechanism which is a good method in the above 
situation mentioned rather than using improper locks/semaphores and busy-loops.

Completion in Linux Device Driver
- In the Linux kernel, "Completions are developed by using waitqueue".

- The advantage of using completion in Linux is that they have a well defined, 
focused purpose which makes it very easy to see the intent of the code, but they also 
result in more efficient code as all threads can continue execution until the result is 
actually needed, and both the waiting and the signaling is highly efficient using 
low-level scheduler sleep/wakeup facilities.

- There are 5 important steps in Completions.

1. Initializing Completion in Linux
2. Re-Initializing Completion in Linux
3. Waiting for completion (The code is waiting and sleeping for something to finish)
4. Waking Up Task (Sending a signal to sleeping part)
5. Check the status
-------------------------------------------------------------------------------------------
Initialize Completion

- We have to include "<linux/completion.h>" and creating a variable of type 
struct completion,which has only two fields:

struct completion { 
    unsigned int done; 
    wait_queue_head_t wait; 
};

- Where, wait is the waitqueue to place tasks on for waiting (if any). 
done is the completion flag for indicating whether it’s completed or not.
-------------------------------------------------------------------------------------------
- We can create the struct variable in two ways.

1. Static Method
2. Dynamic Method
-------------------------------------------------------------------------------------------
Static Method
DECLARE_COMPLETION(data_read_done);

Where the “data_read_done” is the name of the struct which is going to create statically.
-------------------------------------------------------------------------------------------
Dynamic Method
init_completion (struct completion * x);

Where, x – completion structure that is to be initialized

Example:
struct completion data_read_done;

init_completion(&data_read_done);
- In this init_completion call we initialize the waitqueue and set done to 0, 
i.e. “not completed” or “not done”.
-------------------------------------------------------------------------------------------
Re-Initializing Completion
reinit_completion (struct completion * x);

Where, x – completion structure that is to be reinitialized

Example:
reinit_completion(&data_read_done);

- This function should be used to reinitialize a completion structure so it can be reused. 
- This is especially important after complete_all() is used. 
- This simply resets the ->done field to 0 (“not done”), without touching the waitqueue. 
- Callers of this function must make sure that there are no racy "wait_for_completion()" 
calls going on in parallel.
-------------------------------------------------------------------------------------------
Waiting for completion
- For a thread to wait for some concurrent activity to finish, it calls anyone of the 
function based on the use case.

wait_for_completion
This is used to make the function waits for the completion of a task.

void wait_for_completion (struct completion * x);

Where, x – holds the state of this particular completion

- This waits to be signaled for completion of a specific task. It is NOT interruptible 
and there is no timeout.

Example:
wait_for_completion (&data_read_done);

- Note that wait_for_completion() is calling spin_lock_irq()/spin_unlock_irq(), 
so it can only be called safely when you know that interrupts are enabled. 
- Calling it from IRQs-off atomic contexts will result in hard-to-detect spurious 
enabling of interrupts.

">> So do not disable the irqs when calling wait_for_completion()"
-------------------------------------------------------------------------------------------
wait_for_completion_timeout
- This is used to make the function waits for the completion of a task with a timeout. 
- Timeouts are preferably calculated with msecs_to_jiffies() or usecs_to_jiffies(), 
to make the code largely HZ-invariant.

unsigned long wait_for_completion_timeout (struct completion * x, unsigned long timeout);

x – holds the state of this particular completion
timeout – timeout value in jiffies

- This waits for either completion of a specific task to be signaled or for a specified 
timeout to expire. The timeout is in jiffies. It is not interruptible.

- It returns 0 if timed out, and positive (at least 1, or the number of jiffies left 
till timeout) if completed.

Example:
wait_for_completion_timeout (&data_read_done);
-------------------------------------------------------------------------------------------
wait_for_completion_interruptible

- This waits for the completion of a specific task to be signaled. It is interruptible.

int wait_for_completion_interruptible (struct completion * x);
x – holds the state of this particular completion

It return -ERESTARTSYS if interrupted, 0 if completed.
-------------------------------------------------------------------------------------------
wait_for_completion_interruptible_timeout

- This waits for either completion of a specific task to be signaled or for a specified 
timeout to expire. It is interruptible. The timeout is in jiffies. 
- Timeouts are preferably calculated with msecs_to_jiffies() or usecs_to_jiffies(), to make the code largely HZ-invariant.

long wait_for_completion_interruptible_timeout (struct completion * x, unsigned long timeout);

x – holds the state of this particular completion
timeout – timeout value in jiffies

- It returns -ERESTARTSYS if interrupted, 0 if timed out, positive (at least 1, or a 
number of jiffies left till timeout) if completed.
-------------------------------------------------------------------------------------------
wait_for_completion_killable
- This waits to be signaled for completion of a specific task. It can be interrupted by a 
kill signal.

int wait_for_completion_killable (struct completion * x);

x – holds the state of this particular completion

It returns -ERESTARTSYS if interrupted, 0 if completed.
-------------------------------------------------------------------------------------------
wait_for_completion_killable_timeout
- This waits for either completion of a specific task to be signaled or for a specified 
timeout to expire. 
- It can be interrupted by a kill signal. The timeout is in jiffies. 
- Timeouts are preferably calculated with msecs_to_jiffies() or usecs_to_jiffies(), 
to make the code largely HZ-invariant.

long wait_for_completion_killable_timeout (struct completion * x, unsigned long timeout);

x – holds the state of this particular completion
timeout – timeout value in jiffies

- It returns -ERESTARTSYS if interrupted, 0 if timed out, positive (at least 1, or a 
number of jiffies left till timeout) if completed.
-------------------------------------------------------------------------------------------
try_wait_for_completion

- This function will not put the thread on the wait queue but rather returns false if it 
would need to en queue (block) the thread, else it consumes one posted completion and 
returns true.

bool try_wait_for_completion (struct completion * x);

x-holds the state of this particular completion

It returns 0 if completion is not available 1 if a got it succeeded.

This try_wait_for_completion() is safe to be called in IRQ or atomic context.
-------------------------------------------------------------------------------------------
Waking Up Task

complete
- This will wake up a single thread waiting on this completion. 
- Threads will be awakened in the same order in which they were queued.

void complete (struct completion * x);

x – holds the state of this particular completion

Example:
complete(&data_read_done);
-------------------------------------------------------------------------------------------
complete_all

- This will wake up all threads waiting on this particular completion event.

void complete_all (struct completion * x);

x – holds the state of this particular completion
-------------------------------------------------------------------------------------------
Check the status

completion_done
- This is the test to see if completion has any waiters.

bool completion_done (struct completion * x);

x – holds the state of this particular completion

- It returns 0 if there are waiters (wait_for_completion in progress) 1 if there are no 
waiters.

This completion_done() is safe to be called in IRQ or atomic context.

##########################################################################################

