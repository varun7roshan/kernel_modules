##########################################################################################


##########################################################################################


##########################################################################################
							SysFS in Linux Kernel Tutorial

- Sysfs is a virtual filesystem exported by the kernel, similar to /proc. 
- The files in Sysfs contain information about devices and drivers. Some files in Sysfs 
are even writable, for configuration and control of devices attached to the system. 
Sysfs is always mounted on /sys.

- The directories in Sysfs contain the hierarchy of devices, as they are attached to the 
computer.

- Sysfs is the commonly used method to export system information from the kernel space to 
the user space for specific devices. 
- The sysfs is tied to the device driver model of the kernel. 
- The procfs is used to export the process-specific information and the debugfs is used to 
use for exporting the debug information by the developer.

- Before getting into the sysfs we should know about the Kernel Objects.
------------------------------------------------------------------------------------------
Kernel Objects

- The heart of the sysfs model is the kobject. 
- Kobject is the glue that binds the sysfs and the kernel, which is represented by 
struct kobject and defined in <linux/kobject.h>. 
- A struct kobject represents a kernel object, maybe a device or so, such as the things 
that show up as directory in the sysfs filesystem.

- Kobjects are usually embedded in other structures.

It is defined as,

#define KOBJ_NAME_LEN 20 

struct kobject 
{
 char *k_name;
 char name[KOBJ_NAME_LEN];
 struct kref kref;
 struct list_head entry;
 struct kobject *parent;
 struct kset *kset;
 struct kobj_type *ktype;
 struct dentry *dentry;
};

Some of the important fields are:

struct kobject
name: (Name of the kobject. Current kobject is created with this name in sysfs.)
parent: (This is kobject’s parent. When we create a directory in sysfs for the current 
kobject, it will create under this parent directory)
ktype: (the type associated with a kobject)
kset: (a group of kobjects all of which are embedded in structures of the same type)
sd: (points to a sysfs_dirent structure that represents this kobject in sysfs.)
kref: (provides reference counting)

- It is the glue that holds much of the device model and its sysfs interface together.

- So kobject is used to create kobject directory in /sys. This is enough. 
We will not go deep into the kobjects.
------------------------------------------------------------------------------------------
SysFS in Linux device driver

There are several steps to creating and using sysfs.

Create a directory in /sys
Create Sysfs file
Create a directory in /sys

- We can use this function (kobject_create_and_add) to create a directory.

struct kobject * kobject_create_and_add ( const char * name, struct kobject * parent);

name: the name for the kobject
parent: the parent kobject of this kobject, if any.

- If you pass kernel_kobj to the second argument, it will create the directory under 
/sys/kernel/. 
- If you pass firmware_kobj to the second argument, it will create the directory under 
/sys/firmware/. 
- If you pass fs_kobj to the second argument, it will create the directory under 
/sys/fs/. 
- If you pass NULL to the second argument, it will create the directory under /sys/.

- This function creates a kobject structure dynamically and registers it with sysfs. 
- If the kobject was not able to be created, NULL will be returned.

- When you are finished with this structure, call kobject_put and the structure will be 
dynamically freed when it is no longer being used.
------------------------------------------------------------------------------------------
Example
struct kobject *kobj_ref;

/*Creating a directory in /sys/kernel/ */
kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj); //sys/kernel/etx_sysfs

/*Freeing Kobj*/
kobject_put(kobj_ref);
------------------------------------------------------------------------------------------
Create Sysfs file

- Using the above function we will create a directory in /sys. 
- Now we need to create a sysfs file, which is used to interact user space with kernel 
space through sysfs. 
- So we can create the sysfs file using sysfs attributes.
------------------------------------------------------------------------------------------
- Attributes are represented as regular files in sysfs with one value per file. 
- There are loads of helper functions that can be used to create the kobject attributes. 
- They can be found in the header file sysfs.h
------------------------------------------------------------------------------------------
Create attribute

- Kobj_attribute is defined as,

struct kobj_attribute 
{
 struct attribute attr;
 ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
 ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
};

attr: the attribute representing the file to be created,
show:the pointer to the function that will be called when the file is read in sysfs,
store:the pointer to the function which will be called when the file is written in sysfs.
------------------------------------------------------------------------------------------
We can create an attribute using __ATTR macro.

__ATTR(name, permission, show_ptr, store_ptr);
------------------------------------------------------------------------------------------
Store and Show functions
- Then we need to write show and store functions.

ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);

ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

- Store function will be called whenever we are writing something to the sysfs attribute. 
- Show function will be called whenever we are reading the sysfs attribute.
------------------------------------------------------------------------------------------
Create sysfs file

- To create a single file attribute we are going to use ‘sysfs_create_file’.

int sysfs_create_file(struct kobject *  kobj, const struct attribute * attr);

kobj:object we’re creating for.
attr:attribute descriptor.
------------------------------------------------------------------------------------------
- One can use another function ‘ sysfs_create_group ‘ to create a group of attributes.

- Once you have done with the sysfs file, you should delete this file using 
sysfs_remove_file

void sysfs_remove_file ( struct kobject *  kobj, const struct attribute * attr);

kobj:object we’re creating for.
attr:attribute descriptor.
------------------------------------------------------------------------------------------
Example
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d", etx_value);
}

static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        sscanf(buf,"%d",&etx_value);
        return count;
}

//This Function will be called from Init function
/*Creating a directory in /sys/kernel/ */
kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);
 
/*Creating sysfs file for etx_value*/
if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
    printk(KERN_INFO"Cannot create sysfs file......\n");
    goto r_sysfs;
}
//This should be called from exit function
kobject_put(kobj_ref); 
sysfs_remove_file(kernel_kobj, &etx_attr.attr);

##########################################################################################

