#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/moduleparam.h>

#undef pr_fmt
#define pr_fmt(fmt) "In %s():%d:" fmt, __func__, __LINE__

int user_has_set_write_it_now(const char *val, const struct kernel_param *kp);
/*
struct kernel_param_ops
{
 int (*set)(const char *val, const struct kernel_param *kp);
 int (*get)(char *buffer, const struct kernel_param *kp);
 void (*free)(void *arg);
};
*/

const struct kernel_param_ops notfiy_ops = 
{
	.set = user_has_set_write_it_now, //our setter and nofifier
	.get = param_get_int, //default getter from kernel helper functions
};

int my_int = 1;
char *my_string = "This is Org String";
int my_array[3] = {3, 2, 1};
int my_notify_int = 99;

module_param(my_int, int, S_IRUSR|S_IWUSR);

/*for string to be allowed to changed - we need to mentiod "charp" - char pointer*/
module_param(my_string, charp, S_IRUSR|S_IWUSR);

module_param_array(my_array, int, NULL, S_IRUSR|S_IWUSR);

/* #define module_param_cb(name, ops, arg, perm)
	module_paam_cb(variable_name, struct kernel_param_ops *, &address_of_var, S_IRUSR|S_IWUSR)
*/
module_param_cb(my_notify_int, &notfiy_ops, &my_notify_int, S_IRUSR|S_IWUSR);

int user_has_set_write_it_now(const char *val, const struct kernel_param *kp)
{
	int res = 0;
	int i = 0;

	pr_info("--Top--\n");
	res = param_set_int(val, kp); //using helper to update the variable value
	if(!res)
	{
		pr_info("User has set new value to my_notify_int:%d\n", my_notify_int);

		pr_info("--Printing all variables--\n");
		pr_info("my_int : %d\n", my_int);	
		pr_info("my_string : %s\n", my_string);
		pr_info("my_notify_int : %d\n", my_notify_int);
		for(i = 0; i < (sizeof(my_array)/sizeof(int)); i++)
			pr_info("my_array[%d]:%d\n", i, my_array[i]);

		pr_info("--End--\n");
		return 0;
	}
	pr_info("Error:return from param_set_int()\n");
	pr_info("--End--\n");
	return -1;
}

static int __init init_load(void)
{
	int i = 0;
	pr_info("-- INIT - Top --\n");
	
	pr_info("my_int : %d\n", my_int);	
	pr_info("my_string : %s\n", my_string);
	pr_info("my_notify_int : %d\n", my_notify_int);
	for(i = 0; i < (sizeof(my_array)/sizeof(int)); i++)
		pr_info("my_array[%d]:%d\n", i, my_array[i]);

	pr_info("-- INIT - End --\n");
	return 0;
}

static void __exit exit_load(void)
{
	int i = 0;
	pr_info("-- EXIT - Top --\n");

	pr_info("my_int : %d\n", my_int);	
	pr_info("my_string : %s\n", my_string);
	pr_info("my_notify_int : %d\n", my_notify_int);
	for(i = 0; i < (sizeof(my_array)/sizeof(int)); i++)
		pr_info("my_array[%d]:%d\n", i, my_array[i]);


	pr_info("-- EXIT - Top --\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Template Kernel Module");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
//EOF
