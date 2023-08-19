1. Standard Timers - Jiffies
2. HR Timer - ktime
3. Atomic context delays
4. Non-Atomic contexts - sleep()
#########################################################################################
#include <linux/jiffies.h>
#include <linux/timer.h>
----------------------------------------------------------------------------------------
1. New kernel versions have FLAGS for timers : include/linux/timer.h
//not available in rpi-4.19.y
#define TIMER_INIT_FLAGS  (TIMER_DEFERRABLE | TIMER_PINNED | TIMER_IRQSAFE)
----------------------------------------------------------------------------------------
2. Declare a variable of type "struct time_list"

struct timer_list my_timer;

struct timer_list
{
  struct hlist_node entry;
  unsigned long   expires;
  void      (*function)(struct timer_list *);
  u32     flags;
};
----------------------------------------------------------------------------------------
3. Call "timer_setup()" to initialize the timer with callback function

#define timer_setup(timer, callback, flags)

Example:
timer_setup(&my_timer, cb_timer, TIMER_INIT_FLAGS); //used above MACRO
----------------------------------------------------------------------------------------
4. Call mod_timer() giving "expires" is terms of jiffies
>> you can convert ms to jiffies using : msecs_to_jiffies(500);
extern int mod_timer(struct timer_list *timer, unsigned long expires);
>> This starts the timer - by taking the expires value
>> Notice here that we have to pass "expires" value as ABSOLUTE value
>> RELATIVE would have been better, as we would not have to keep track of "jiffies"
>> HR-Timer has this feature

Example:
ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
if(ret)
	ERROR
----------------------------------------------------------------------------------------
5. In the callback function - we have to call mod_timer() again - if we want next
callback for the timer

Example:
void cb_timer(struct timer_list *ptimer)
{
	//need to call mod_timer again - if we need - as its single shot
	ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));
}
----------------------------------------------------------------------------------------
6. We need to delete the timer - when no loader needed - usually when module is unloaded

extern int del_timer(struct timer_list * timer);

int del_timer_sync(struct timer_list *timer);
>> This better as - it waits for the callback to finish execution - in-case locks are used

Example:
del_timer(&my_timer);
----------------------------------------------------------------------------------------
- OLD Kernel Versions:
struct timer_list
{
  struct list_head entry;
  unsigned long expires; //value in jiffies - when this Expries - callback is called
  struct tvec_t_base_s *base;
  void (*function)(unsigned long); // our callback function to be registered here
  unsigned long data; // data that needs to be passed in callback function

  int slack; //not sure what this is
};

void setup_timer( struct timer_list *timer, void (*function)(unsigned long),
                  unsigned long data);
###########################################################################################
-------- HR - Timer ----------
#include <linux/ktime.h>
#include <linux/hrtimer.h>

//Timer Variable
#define TIMEOUT_NSEC   ( 1000000000L )      //1 second in nano seconds
#define TIMEOUT_SEC    ( 4 )                //4 seconds

static struct hrtimer etx_hr_timer;

//Timer Callback function. This will be called when timer expires
enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
     /* do your timer stuff here */
    pr_info("Timer Callback function Called [%d]\n",count++);
    hrtimer_forward_now(timer,ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC));
    return HRTIMER_RESTART;
}

ktime = ktime_set(TIMEOUT_SEC, TIMEOUT_NSEC);
hrtimer_init(&etx_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
etx_hr_timer.function = &timer_callback;
hrtimer_start( &etx_hr_timer, ktime, HRTIMER_MODE_REL);

hrtimer_cancel(&etx_hr_timer);

##########################################################################################
- How to use ABS

hrtimer_start(&dev->poll_hrt, last_timeout + ms_to_ktime(5), HRTIMER_MODE_ABS);
>> Note here we have to pass the absolute value of time when to expire - in ABS mode

hrtimer_start(&dev->poll_hrt, ms_to_ktime(5), HRTIMER_MODE_REL);
>> Where as here, we are telling the timer should expire relative to current time - and
we can give the delay/expire value directly

##########################################################################################

struct hrtimer my_hr_timer;
unsigned long ms = 1000;
int count = 60;

enum hrtimer_restart cb_hrtimer(struct hrtimer *ptimer)
{
	hrtimer_forward_now(ptimer, ms_to_ktime(ms)); //this is REL value of time
	return HRTIMER_RESTART;

	return HRTIMER_NORESTART;
}

hrtimer_init(&my_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL); //relative to now
hrtimer_start(&my_hr_timer, ms_to_ktime(ms), HRTIMER_MODE_REL_PINNED); //relative to now

ret = hrtimer_try_to_cancel(&my_hr_timer);
if(ret)
	//timer is active - can call force cancel - hrtimer_cancel() - just wait for cb to finish

hrtimer_cancel(&my_hr_timer);

#########################################################################################
#include <linux/delay.h>

udelay(unsigned long usecs)

>> When we need delay in Atomic contexts - like in ISR

#########################################################################################
- There are two categories of time. 
- The kernel uses "absolute time" to know "what time it is", that is, the "date and time" 
of "the day".
- whereas "relative time" is used by, for example, the kernel scheduler. 

- For "absolute time", there is a "hardware chip" called the "real-time clock (RTC)". 
- To handle "relative time", the kernel relies on a "CPU feature (peripheral)" called a 
timer, which, from the "kernel's point of view", is called a "kernel timer". 

-Kernel timers are classified into two different parts:
1. Standard timers, or system timers
2. High-resolution timers

- Timer IRQ is generated by an External Chip : The APIC provides the timer interrupt
##########################################################################################
- Standard timers are kernel timers operating on the granularity of "jiffies".

Jiffies and HZ
--------------
- A "jiffy" is a "kernel unit of time" declared in "<linux/jiffies.h>". 

- To understand jiffies, we need to introduce a new constant, "HZ", which is the number 
of times jiffies is incremented in one second. 

- Each increment is called a tick. 

- In other words, "HZ represents the size of a jiffy". 
>> Meaning,
Jiffies is incremented for each tick – that is the frequency
If HZ is 1 – then Time Period is 1 sec – and jiffies is incremented every tick - 1 second
If HZ = 1000 – that means, jiffies is incremented at this frequency
Time Period is 1/1000(HZ) = 1ms –  so the jiffies is incremented every 1ms
and after 1 second, the jiffies value will be 1000 - meaning 1000 ms has elapsed, as for
HZ = 1000, the time period is 1ms, so 1000 x 1ms = 1000 ms = 1 second 

- "You can treat HZ as TCNT"

- When they ask at what rate the jiffies is incremented "?"
>> Its based on the HZ value, either the clock value – that is
>> Time Period = 1/(HZ) -> At this rate the jiffies is incremented

- HZ depends on the hardware and on the kernel version, and also determines how 
frequently the clock interrupt fires. That is if HZ = 1000, then time period is 1ms and
timer interrupt is fired every 1ms and jiffies will be incremented every 1ms

- This is configurable on some architectures, fixed on other ones.

- What it means is that jiffies is incremented HZ times every second. 
- Once defined, the "programmable interrupt timer (PIT)", which is a "hardware component", 
is programmed with that value in order to increment jiffies when the PIT interrupt comes in.

- Depending on the platform, jiffies can lead to overflow. 
- On a 32-bit system, HZ = 1,000 will result in about 50 days, duration only, 
whereas the duration is about 600 million years on a 64-bit system. 

- By storing jiffies in a 64-bit variable, the problem is solved. 

- A second variable has then been introduced and defined in <linux/jiffies.h>:
extern u64 jiffies_64;

- In this manner, on 32-bit systems jiffies will point to low-order 32 bits, and 
jiffies_64 will point to high-order bits. 
- On 64-bit platforms, jiffies = jiffies_64.

So generally we can use "jiffies" directly - on 64-bit and 32-bit systems, but on 32-bit
system we need to be careful about overflow - thats it

##########################################################################################
The timer API

- A timer is represented in the kernel as an instance of timer_list:

#include <linux/timer.h>

struct timer_list 
{
	struct list_head entry;
	unsigned long expires; //value in jiffies - when this Expries - callback is called
	struct tvec_t_base_s *base;
	void (*function)(unsigned long); // our callback function to be registered here
	unsigned long data; // data that needs to be passed in callback function

	int slack; //not sure what this is
};

- "expires" is an absolute value in jiffies. 
- "entry" is a doubly linked list
- "data" is optional,passed to the callback "function"

struct timer_list 
{
	struct hlist_node	entry;
	unsigned long		expires;
	void			(*function)(struct timer_list *);
	u32			flags;
};

#define TIMER_CPUMASK		0x0003FFFF
#define TIMER_MIGRATING		0x00040000
#define TIMER_BASEMASK		(TIMER_CPUMASK | TIMER_MIGRATING)
#define TIMER_DEFERRABLE	0x00080000
#define TIMER_PINNED		0x00100000
#define TIMER_IRQSAFE		0x00200000
#define TIMER_INIT_FLAGS	(TIMER_DEFERRABLE | TIMER_PINNED | TIMER_IRQSAFE)
#define TIMER_ARRAYSHIFT	22
#define TIMER_ARRAYMASK		0xFFC00000
##########################################################################################
Timer setup initialization
--------------------------
1. Setting up the timer: 
- Set up the timer, feeding the user-defined callback and data:
void setup_timer( struct timer_list *timer, void (*function)(unsigned long), 
									unsigned long data);

You can also use this:
void init_timer(struct timer_list *timer);
setup_timer is a wrapper around init_timer.

>> Better to use "setup_timer()" as the "expires" will be set in another call to:
mod_timer() - internally mod_timer() sets expires correctly for us

2. Setting the expiration time: 
- When the timer is initialized, we need to set its expiration before the callback 
gets fired:
int mod_timer(struct timer_list *timer, unsigned long expires);

3. Releasing the timer: When you are done with the timer, it needs to be released:
void del_timer(struct timer_list *timer);
int del_timer_sync(struct timer_list *timer);

- "del_timer()" returns void whether it has deactivated a pending timer or not. 
Its return value is 0 on an inactive timer, or 1 on an active one. 

- The last, "del_timer_sync()", waits for the handler to finish its execution, 
even if that happens on another CPU. 

- "You should not hold a lock preventing the handler's completion", otherwise it will 
result in a deadlock. 

- You should release the timer in the module cleanup routine. 
You can independently check whether the timer is running or not:
int timer_pending( const struct timer_list *timer);
>> This function checks whether there are any fired timer callbacks pending.

##########################################################################################

High-resolution timers (HRTs)
-----------------------------
- Standard timers are less accurate and do not suit real-time applications. 

- HRTs, introduced in kernel v2.6.16 (and enabled by the CONFIG_HIGH_RES_TIMERS option 
in the kernel configuration) 
- have a resolution in "microseconds" (up to "nanoseconds", depending on the platform), 
"compared to milliseconds on standard timers". 

- The standard timer depends on HZ (since they rely on jiffies), 
- whereas the "HRT implementation" is based on "ktime".
typedef s64 ktime_t;

- The kernel and hardware must support an HRT before being used on your system. 

- In other words, there must be "architecture-dependent code" implemented to access your 
hardware HRTs.
TIMER_NORESTART###########################################################################
#include <linux/hrtimer.h>

struct hrtimer 
{
  struct timerqueue_node    node;
  ktime_t       _softexpires;
  enum hrtimer_restart    (*function)(struct hrtimer *);
  struct hrtimer_clock_base *base;
  u8        state;
  u8        is_rel;
  u8        is_soft;
};

/*
 * Return values for the callback function
 */
enum hrtimer_restart {
  HRTIMER_NORESTART,  /* Timer is not restarted */
  HRTIMER_RESTART,  /* Timer must be restarted */
};

##########################################################################################

1. Initializing the hrtimer: 
- Before hrtimer initialization, you need to set up a "ktime", which represents time 
duration. We will see how to achieve that in the following example:
void hrtimer_init( struct hrtimer *time, clockid_t which_clock, enum hrtimer_mode mode);

for : clockid_t which_clock
/*
 * The IDs of the various system clocks (for POSIX.1b interval timers):
 */
#define CLOCK_REALTIME      0 // Wall Clock
#define CLOCK_MONOTONIC     1 // TCNT - clock, does not depend on Wall clock tick
>> So for timers - better to use : "CLOCK_MONOTONIC"
CLOCK_MONOTONIC: a clock that is guaranteed always to move forward in time, but which does not reflect “wall clock time” in any specific way. In the current implementation, CLOCK_MONOTONIC resembles the jiffies tick count in that it starts at zero when the system boots and increases monotonically from there.
CLOCK_REALTIME: which matches the current real-world time.

- Notice in below example, in case of ABS - we have keep track of Last Timeout and then
incement to the Absolute Timer - This is a headache - so better to use REL - relative to
now
Example:
hrtimer_start(&dev->poll_hrt, ms_to_ktime(5), HRTIMER_MODE_REL);
hrtimer_start(&dev->poll_hrt, last_timeout + ms_to_ktime(5), HRTIMER_MODE_ABS);
/*
 * Mode arguments of xxx_hrtimer functions:
 *
 * HRTIMER_MODE_ABS   - Time value is absolute
 * HRTIMER_MODE_REL   - Time value is relative to now
 * HRTIMER_MODE_PINNED    - Timer is bound to CPU (is only considered
 *          when starting the timer)
 * HRTIMER_MODE_SOFT    - Timer callback function will be executed in
 *          soft irq context
 */
enum hrtimer_mode {
  HRTIMER_MODE_ABS  = 0x00,
  HRTIMER_MODE_REL  = 0x01,
  HRTIMER_MODE_PINNED = 0x02,
  HRTIMER_MODE_SOFT = 0x04,

  HRTIMER_MODE_ABS_PINNED = HRTIMER_MODE_ABS | HRTIMER_MODE_PINNED,
  HRTIMER_MODE_REL_PINNED = HRTIMER_MODE_REL | HRTIMER_MODE_PINNED,

  HRTIMER_MODE_ABS_SOFT = HRTIMER_MODE_ABS | HRTIMER_MODE_SOFT,
  HRTIMER_MODE_REL_SOFT = HRTIMER_MODE_REL | HRTIMER_MODE_SOFT,

  HRTIMER_MODE_ABS_PINNED_SOFT = HRTIMER_MODE_ABS_PINNED | HRTIMER_MODE_SOFT,
  HRTIMER_MODE_REL_PINNED_SOFT = HRTIMER_MODE_REL_PINNED | HRTIMER_MODE_SOFT,

};

Example : arch/powerpc/kernel/watchdog.c
hrtimer_init(hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
hrtimer->function = watchdog_timer_fn;
hrtimer_start(hrtimer, ms_to_ktime(wd_timer_period_ms),HRTIMER_MODE_REL_PINNED);

static enum hrtimer_restart watchdog_timer_fn(struct hrtimer *hrtimer)
{
 /* hrtimer_forward_now - forward the timer expiry so it expires after now
 * @timer:  hrtimer to forward
 * @interval: the interval to forward
*/

	hrtimer_forward_now(hrtimer, ms_to_ktime(wd_timer_period_ms));
	return HRTIMER_NORESTART;
	return HRTIMER_RESTART;

}


##########################################################################################
2. ktime_set:
- There is a new type, "ktime_t", which is used to store a "time value in nanoseconds". 
- On 64-bit systems, a ktime_t is really just a 64-bit integer value in nanoseconds. 
- On 32-bit machines, however, it is a two-field structure: one 32-bit value holds the 
number of seconds, and the other holds nanoseconds. 
The below function is used to get the ktime_t from seconds and nanoseconds.

ktime_set(long secs, long nanosecs);
>> So To keep the Driver Code generic across paltforms - 32-bit/64-bit
- if we want 5-sec timer
ktime_set(4, 1000000000);
>> On 32-bit, 4,100000000 will be used on 2 seperate variables
>> On64-bit, it will be combined and saved in a single 64-bit variable


##########################################################################################
3. Starting hrtimer: 
hrtimer can be started as shown in the following example:
int hrtimer_start( struct hrtimer *timer, ktime_t time, const enum hrtimer_mode mode);
"mode" represents the "expiry mode". It should be "HRTIMER_MODE_ABS" for an
absolute time value, or "HRTIMER_MODE_REL" for a time value "relative to now".
typedef s64 ktime_t;


##########################################################################################
4. hrtimer cancellation: 
You can either cancel the timer or see whether it is possible to cancel it:
int hrtimer_cancel( struct hrtimer *timer);
int hrtimer_try_to_cancel(struct hrtimer *timer);
- Both return 0 when the timer is not active and 1 when the timer is active. 
- The difference between these two functions is that "hrtimer_try_to_cancel()" fails if
the timer is active or its callback is running, returning -1, whereas
"hrtimer_cancel()" will wait until the callback finishes.

- We can independently check whether the hrtimer's callback is still running with the
following:
int hrtimer_callback_running(struct hrtimer *timer);

- Remember, hrtimer_try_to_cancel() internally calls hrtimer_callback_running().

- In order to prevent the timer from automatically restarting, the hrtimer
callback function must return "HRTIMER_NORESTART".
>> So HR Timer - restarts automatically - so this can run like a scheduled Thread
- and we can stop the thread, but just returnning "HRTIMER_NORESTART"

- With HRTs enabled on your system, the "accuracy of sleep()" and "timer system calls" 
"does not depend on jiffies anymore", but they are still as accurate as HRTs are. 
It is the reason why some systems do not support "nanosleep()", for example.


##########################################################################################
5. Changing the High Resolution Timer’s Timeout
- If we are using this High Resolution Timer (hrtimer) as a periodic timer, then the 
callback must set a new expiration time before returning. 
- Usually, restarting timers are used by kernel subsystems that need a callback at a 
regular interval.

hrtimer_forward:
u64 hrtimer_forward(struct hrtimer * timer, ktime_t now, ktime_t interval);

- This will forward the timer expiry so it will expire in the future by the given interval.
Arguments:
timer – hrtimer to forward
now – forward past this time
interval – the interval to forward

Return:
Returns the number of overruns.
##########################################################################################
6. hrtimer_forward_now
u64 hrtimer_forward_now(struct hrtimer *timer, ktime_t interval);

This will forward the timer expiry so it will expire in the future from now by the given 
interval.

Arguments:
timer – hrtimer to forward
interval – the interval to forward

Returns:
Returns the number of overruns.
##########################################################################################
7. Check High Resolution Timer’s status
The below-explained functions are used to get the status and timings.

hrtimer_get_remaining
ktime_t hrtimer_get_remaining(const struct hrtimer * timer);
This is used to get the remaining time for the timer.

Arguments:
timer – hrtimer to get the remaining time
Returns:
Returns the remaining time.
##########################################################################################
8.hrtimer_callback_running
int hrtimer_callback_running(struct hrtimer *timer);

This is the helper function to check, whether the timer is running the callback function.
Arguments:
timer – hrtimer to check 
Returns:
0 when the timer’s callback function is not running
1 when the timer’s callback function is running
##########################################################################################
9. hrtimer_cb_get_time
ktime_t hrtimer_cb_get_time(struct hrtimer *timer);

This function used to get the current time of the given timer.
Arguments:
timer – hrtimer to get the time
Returns:
Returns the time.

##########################################################################################
Using High Resolution Timer In Linux Device Driver
In this example, we took the basic driver source code from this tutorial. On top of that code, we have added the high resolution timer. The steps are mentioned below.

Initialize and start the timer in the init function
After the timeout, a registered timer callback will be called.
In the timer callback function again we are forwarding the time period and return HRTIMER_RESTART. We have to do this step if we want a periodic timer. Otherwise, we can ignore that time forwarding and return HRTIMER_NORESTART.
Once we are done, we can disable the timer.
In this example, We are setting 5 seconds timeout using ktime_set(4, 1000000000). That 4 for 4 seconds and 1000000000 (nano sec) for 1 second.
##########################################################################################

Dynamic tick/tickless kernel
----------------------------
>> "Diabling/Changing Periodic Tick Value"
>> Either, when in idle thread running - it will set the Timer value in PIC - with a large
value - So the Timer Interrupt occurs later - so no CPU processing of timer interrupt
>> And when the timer interrupt occurs - it will check if tasks are present in the 
run queue - and enables the "periodic tick - programs the timer" and invokes the
scheduler - if no tasks are there - it will not enable the "periodic tick"

- With the previous HZ options, the kernel is interrupted HZ times per second in order to
reschedule tasks, even in an idle state. 

- If HZ is set to 1,000, there will be 1,000 kernel interruptions per second, preventing 
the CPU from being idle for a long time, thus affecting CPU power consumption.

- Now, let's look at a kernel with no fixed or predefined ticks, where the ticks are 
disabled until some task needs to be performed. 

- "We call such a kernel a tickless kernel". 

- In fact, "tick activation is scheduled", "based on the next action". 
- The right name should be dynamic tick kernel. 

- The kernel is responsible for task scheduling, and maintains a list of runnable tasks
(the run queue) in the system. 

- When there is no task to schedule, the scheduler switches to the idle thread, which 
"enables dynamic tick" by "disabling periodic tick" until the "next timer expires"
(a new task is queued for processing).

- Under the hood, the kernel also maintains a list of the task timeouts 
(it then knows when and how long it has to sleep). 

- In an idle state, if the next tick is further away than the lowest timeout in the tasks 
list timeout, the "kernel programs the timer" with that "timeout value".

- When the "timer expires", the "kernel re-enables periodic ticks" and invokes the 
scheduler, which then schedules the task associated with the timeout. 

- This is how the tickless kernel removes periodic ticks and saves power when idle.
##########################################################################################
Delays and sleep in the kernel
------------------------------
- Without going into the details, there are "two types" of "delays", depending on the 
context your code runs in: "atomic or nonatomic". The mandatory header to handle 
delays in the kernel is :
#include <linux/delay>.

Atomic context
--------------
- Tasks in the atomic context (such as ISR) can't sleep, and can't be scheduled; 
it is the reason why busy-wait loops are used for delaying purposes in an atomic context. 
- The kernel exposes the "Xdelay()" family of functions that will spend time in a busy 
loop, long enough(based on jiffies) to achieve the desired delay:

ndelay(unsigned long nsecs)
udelay(unsigned long usecs) --> Always use this
mdelay(unsigned long msecs)

- You should always use "udelay()" since "ndelay()" precision depends on how accurate your
hardware timer is (not always the case on an embedded SOC). 
- Use of mdelay() is also discouraged.
- Timer handlers (callbacks) are executed in an atomic context, meaning that sleeping is 
not allowed at all. By sleeping, I mean any function that may result in sending the 
caller to sleep, such as allocating memory, locking a mutex, an explicit call to the 
sleep() function, and so on.

Nonatomic context
-----------------
- In a nonatomic context, the kernel provides the sleep[_range] family of functions 
and which function to use depends on how long you need to delay by:

- udelay(unsigned long usecs): Busy-wait loop-based. You should use this function if you 
need to sleep for a few μsecs ( < ~10 us ).

- usleep_range(unsigned long min, unsigned long max): Relies on hrtimers, and it is 
recommended to let this sleep for a few ~μsecs or small msecs (10 us - 20 ms), avoiding 
the busy-wait loop of udelay().

- msleep(unsigned long msecs): Backed by jiffies/legacy_timers. You should use this for 
larger, msecs sleep (10 ms+).

Sleep and delay topics are well explained in
Documentation/timers/timers-howto.txt in the kernel source.
##########################################################################################
			Understanding Linux Kernel - Chanpter-6 : Timing Measurements
##########################################################################################

Real Time Clock (RTC)
---------------------
- All "PCs" include a "clock"/wall-clock called "Real Time Clock (RTC)", which is 
independent of the CPU and all other chips.

- The "RTC" continues to tick even when the "PC is switched off", because it is 
energized by a small battery. 

- The CMOS RAM and RTC are integrated in a single chip (the Motorola 146818).

- The "RTC is capable of issuing periodic interrupts" on "IRQ8" at frequencies ranging
between "2 Hz" and "8,192 Hz". 

- It can also be programmed to activate the "IRQ8" line when the RTC reaches a specific 
value, thus working as an "alarm clock".

- "Linux uses the RTC only to derive the time and date;" 

- however, it allows processes "to program the RTC" by acting on the "/dev/rtc" 
device file (see Chapter 13). 

- The "kernel accesses the RTC" through the "0x70 and 0x71 I/O ports". 
- The system administrator can read and write the RTC by executing the clock Unix system 
program that acts directly on these two I/O ports.
##########################################################################################
Time Stamp Counter (TSC) - This is different from Timer Interrupt Externally
------------------------
- All "80×86 microprocessors" include a "CLK input pin", which receives the "clock signal"
of an external oscillator. 
>> This is from a External Crystal Oscillator - we have seen this in Micro-controllers as
well - So the uC or uP can take the external Crystal as Clock source - or it can have
an internal oscillator

- Starting with the Pentium, 80×86 microprocessors sport a counter(TCNT) that is increased 
at each clock signal. 

- The counter is accessible through the 64-bit Time Stamp Counter (TSC/TCNT) register, 
which can be read by means of the "rdtsc" assembly language instruction. 
>> Like the TCNT in Atmega 

- When using this register, the kernel has to take into consideration the frequency of 
the clock signal: if, for instance, the clock ticks at 1 GHz, the Time Stamp Counter is 
increased once every nanosecond.

- Linux may take advantage of this register to get much more accurate time measurements
than those delivered by the "Programmable Interval Timer". 

- To do this, "Linux must determine the clock signal frequency" while initializing the 
system
- In fact, because this frequency is not declared when compiling the kernel, the same 
kernel image may run on CPUs whose clocks may tick at any frequency.

- The "task of figuring out" the "actual frequency of a CPU" is accomplished during the
system’s boot. 
- The "calibrate_tsc()" function computes the frequency by counting the number of clock 
signals that occur in a time interval of approximately 5 milliseconds.

- This time constant is produced by properly setting up one of the channels of the 
Programmable Interval Timer (see the next section).*

>> So we are sure PIT - will give an interrupt every 1-ms - and we read the "TSC" register
after 1-ms - to check the numbe of clock signal, which incremented the TSC register
>> So in 1-ms if the TSC value was - 1, that means the Clock Frequency is :
1 count --> 1 ms
  ?     <-- 1000 ms
-------------------- > (1000 ms x 1)/1 ms = 1000 Hz is the Clock Frequency
>> This is how we can calibrate - and find out the CLOCK frequency from external crystal
##########################################################################################
Programmable Interval Timer (PIT)
---------------------------------
- Besides the "Real Time Clock" and the "Time Stamp Counter" TSC/TCNT, IBM-compatible PCs
include another type of time-measuring device called "Programmable Interval Timer" (PIT ). 

- The role of a "PIT" is similar to the "alarm clock" of a microwave oven: 
- it makes the user aware that the cooking time interval has elapsed. 
- Instead of ringing a bell, this device issues a "special interrupt" called 
"timer interrupt", which notifies the kernel that one more time interval has elapsed.

- Another difference from the alarm clock is that the PIT goes on issuing "interrupts" 
forever at some fixed frequency established by the kernel. 
>> So we program the PIT - at what rate it has to give us interrupt

- Each IBM-compatible PC includes at least one PIT, which is usually implemented by an 
8254 CMOS chip using the 0x40–0x43 I/O ports.

- As we’ll see in detail in the next paragraphs, Linux programs the PIT of IBM-compatible
PCs to issue timer interrupts on the "IRQ0" at a (roughly) 1000-Hz frequency— that is, 
once every 1 millisecond. 
>> So this PIT chip is connected to IRQ-0 line
>> We saw that RTC is on IRQ-8 line
>> And TSC is an internal register - where we can read the value - this can also be
an interrupt

- This time interval is called a tick, and its length in nanoseconds is stored in the 
tick_nsec variable. 
- On a PC, tick_nsec is initialized to 999,848 nanoseconds (yielding a clock signal 
frequency of about 1000.15 Hz), but its value may be automatically adjusted by the 
kernel if the computer is synchronized with an external clock (see the later section 
“The adjtimex( ) System Call”). 

- The ticks beat time for all activities in the system; in some sense, they are like the 
ticks sounded by a metronome while a musician is rehearsing.
>> So this is main Tick - clock signal used for processing like - scheduling
>> RTC and TSC timers have different role

- Generally speaking, "shorter ticks" result in "higher resolution timers", which help 
with smoother multimedia playback and faster response time when performing synchronous
I/O multiplexing (poll() and select() system calls). 

- This is a trade-off however: "shorter ticks" require the "CPU" to spend a larger 
fraction of its time in "Kernel Mode"—that is, a "smaller fraction of time" in "User Mode"
- As a consequence, user programs run slower.

- The frequency of timer interrupts depends on the hardware architecture. 
- The slower machines have a tick of roughly 10 milliseconds (100 timer interrupts per 
second), while the faster ones have a tick of roughly 1 millisecond (1000 or 1024 
timer interrupts per second).

- A few macros in the Linux code yield some constants that determine the frequency of
timer interrupts. These are discussed in the following list.

• "HZ" yields the approximate "number of timer interrupts per second"—that is, their
frequency. This value is set to 1000 for IBM PCs.

• "CLOCK_TICK_RATE" yields the value 1,193,182, which is the 8254 chip’s internal
oscillator frequency.
>> 8254 is the PIT Chipset - which is connected to IRQ-0 line - Timer Interrupt

• "LATCH" yields the ratio between CLOCK_TICK_RATE and HZ, rounded to the nearest integer. 
It is used to program the PIT.

- The "PIT" is initialized by "setup_pit_timer()" as follows:

spin_lock_irqsave(&i8253_lock, flags);
outb_p(0x34,0x43); //outb_p(value, IO_PORT); _p is for pause - NOP instruction
udelay(10);
outb_p(LATCH & 0xff, 0x40);
udelay(10);
outb(LATCH >> 8, 0x40);
spin_unlock_irqrestore(&i8253_lock, flags);

The outb( ) C function is equivalent to the outb assembly language instruction: 
- it copies the first operand into the I/O port specified as the second operand. 
- The outb_p( ) function is similar to outb( ), except that it introduces a "pause" 
by executing a "no-op" instruction to keep the hardware from getting confused. 

- The udelay() macro introduces a further small delay (see the later section 
“Delay Functions”). 

- The first outb_p( ) invocation is a command to the PIT to "issue interrupts" 
at a "new rate". 

- The next two outb_p( ) and outb( ) invocations supply the "new interrupt rate" to the 
device. 

- The 16-bit LATCH constant is sent to the 8-bit 0x40 I/O port of the device as 
two consecutive bytes. 

- As a result, the PIT issues timer interrupts at a (roughly) 1000-Hz frequency 
(that is, once every 1 ms).
##########################################################################################
CPU Local Timer
---------------

- The local "APIC" present in recent 80 × 86 microprocessors 
(see the section “Interrupts and Exceptions” in Chapter 4) provides yet another 
time-measuring device: "the CPU local timer".

- The CPU local timer is a "device" similar to the "Programmable Interval Timer" 
just described that can issue one-shot or periodic interrupts. 
- There are, however, a few differences:

• The "APIC’s timer counter" is 32 bits long, while the PIT’s timer counter is 16 bits long
therefore, the local timer can be programmed to issue interrupts at very low frequencies 
(the counter stores the number of ticks that must elapse before the interrupt is issued).

• The "local APIC timer sends an interrupt only to its processor", while the PIT
raises a global interrupt, which may be handled by any CPU in the system.
>> This is important Point - There will be an APIC - Advanced Programmable Interrupt 
Controller Chip attached to Every CPU Core - So APIC sends interrupt only to its CPU
Where as the PIT Chip Sends IRQ-0 - which is a Global Interrupt
>> So this becomes another Clock Source

• The "APIC’s timer" is based on the "bus clock signal" (or the APIC bus signal). 
It can be programmed in such a way to decrease the timer counter every 
1, 2, 4, 8, 16, 32, 64, or 128 bus clock signals. 
- Conversely, the PIT, which makes use of its own clock signals, can be programmed in a 
more flexible way.

##########################################################################################
Aso at this Point we have Clock Souces:
1. RTC - External Chip - Date and Time - on IRQ-8
2. TSC - Like TCNT - internal register - which is clocked using CPU CLOCK_IN pin
3. PIT - External Chip - Connected to IRQ-0 line - which is the timer Interrupt - HZ
4. APIC - Chip attached to every CPU - sends local interrupt for every CPU

Next we have :
High Precision Event Timer (HPET) - This is also an External Chip
---------------------------------
- The "High Precision Event Timer (HPET)" is a "new timer chip" developed jointly by
Intel and Microsoft. 
- Although HPETs are not yet very common in end-user machines, Linux 2.6 already supports 
them, so we’ll spend a few words describing their characteristics.

- The "HPET" provides a "number of hardware timers" that can be exploited by the kernel.

- Basically, the "chip" includes up to "eight 32-bit or 64-bit independent counters".
>> So a HPET Chip - has 8 registers - each having its own clock source - the clock source
can be from a divider - Liker have "Counters" in micro-controllers

- Each counter is driven by its own clock signal, whose frequency must be at least 10 MHz; 
therefore, the counter is increased at least once in 100 nanoseconds. 

- Any counter is associated with at most 32 timers, each of which is composed by a 
comparator and a match register. The comparator is a circuit that checks the value in the
counter against the value in the match register, and raises a hardware interrupt if a
match is found. Some of the timers can be enabled to generate a periodic interrupt.

- The HPET chip can be programmed through registers mapped into memory space
(much like the I/O APIC). The BIOS establishes the mapping during the bootstrapping
phase and reports to the operating system kernel its initial memory address. 

- The HPET registers allow the kernel to read and write the values of the counters and of
the match registers, to program one-shot interrupts, and to enable or disable periodic
interrupts on the timers that support them.

- The next generation of motherboards will likely support both the HPET and the
8254 PIT; in some future time, however, the HPET is expected to completely replace
the PIT.
##########################################################################################

ACPI Power Management Timer - This better that TSC - as TSC is internal to CPU
---------------------------
- The ACPI Power Management Timer (or ACPI PMT) is yet another clock device
included in almost all ACPI-based motherboards. 

- Its clock signal has a fixed frequency of roughly 3.58 MHz. 

- The device is actually a simple counter increased at each clock tick; to read the 
current value of the counter, the kernel accesses an I/O port whose address is determined 
by the BIOS during the initialization phase (see Appendix A).

- The "ACPI Power Management Timer is preferable to the TSC" if the operating system
or the BIOS may dynamically lower the frequency or voltage of the CPU to save
battery power. 
- "When this happens, the frequency of the TSC changes—thus causing time warps" and others 
unpleasant effects—while the frequency of the ACPI PMT does not. 

- On the other hand, the high-frequency of the TSC counter is quite handy for measuring 
very small time intervals.
- However, if an HPET device is present, it should always be preferred to the other 
circuits because of its richer architecture. 

- Table 6-2 later in this chapter illustrates how
Linux takes advantage of the available timing circuits.

- Now that we understand what the hardware timers are, we may discuss how the
Linux kernel exploits them to conduct all activities of the system.
##########################################################################################

