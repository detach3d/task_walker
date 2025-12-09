#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include "task_walker.h"

#define BUF_LEN 50

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rasul Mammadov");
MODULE_DESCRIPTION("This driver walks through the task list of the kernel.");

static int device_open(struct inode *inode, 
						struct file *file);

static int device_release(struct inode *inode, 
							struct file *file);

static ssize_t task_walker_write(struct file *file,
			   char __user * buffer,
			   size_t length,
			   loff_t * offset);

static long device_ioctl(struct file *file, 
						unsigned int ioctl_num, 
						unsigned long ioctl_param);

static dev_t g_dev;
static struct cdev g_cdev;

static char *Message_Ptr;
static char Message_Buf[4096];

static struct file_operations g_fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_release,
	.unlocked_ioctl = device_ioctl
};

static int __init task_walker_init(void)
{
	int result;

	printk(KERN_INFO "task_walker module initialization...\n");

	if ((result = alloc_chrdev_region(&g_dev, 0, 1, "task_walker")) < 0) {
		printk(KERN_INFO "cannot alloc char driver!...\n");
		return result;
	}
	cdev_init(&g_cdev, &g_fops);
	if ((result = cdev_add(&g_cdev, g_dev, 1)) < 0) {
		unregister_chrdev_region(g_dev, 1);
		printk(KERN_ERR "cannot add device!...\n");
		return result;
	}
    printk(KERN_INFO "task_walker module loaded. Major: %d Minor: %d\n", MAJOR(g_dev), MINOR(g_dev));
	
    // task_walker_write(NULL, NULL, 0, NULL);
    
    return 0;
}

static int device_open(struct inode *inode, 
					struct file *file)
{
	printk(KERN_INFO "device_open()\n");
	return 0;
}

static int device_release(struct inode *inode, 
						struct file *file)
{
	printk(KERN_INFO "device_release()\n");
	return 0;
}

static long device_ioctl(struct file *file, 
						unsigned int ioctl_num, 
						unsigned long ioctl_param)
{
	switch (ioctl_num) {
	case IOC_WALK_TASKS:
		printk(KERN_INFO "ioctl: Walking tasks...\n");
		task_walker_write(file, NULL, 0, 0);
		
		/* Copy the message from kernel space to user space */
		if (copy_to_user((char __user *)ioctl_param, Message_Ptr, strlen(Message_Ptr) + 1)) {
			printk(KERN_ERR "Failed to copy data to user space\n");
			return -EFAULT;
		}
		break;

	default:
		printk(KERN_INFO "ioctl: Unknown command\n");
		return -ENOTTY;
	}
	return 0;
}

static void __exit task_walker_exit(void)
{
	cdev_del(&g_cdev);
	unregister_chrdev_region(g_dev, 1);


	printk(KERN_INFO "task_walker module exit...\n");
}

static ssize_t task_walker_write(struct file *file,
			   char __user * buffer,
			   size_t length,
			   loff_t * offset)
{
    struct task_struct *task;
    int offset_buf = 0;
    int remaining = sizeof(Message_Buf) - 1;

    printk(KERN_INFO "Walking through the task list...\n");
    
    rcu_read_lock();
    for_each_process(task) {
        int bytes = snprintf(Message_Buf + offset_buf, remaining,
                            "PID: %d | Comm: %s\n",
                            task->pid, task->comm);
        
        if (bytes < remaining) {
            offset_buf += bytes;
            remaining -= bytes;
            pr_info("%s", Message_Buf + offset_buf - bytes);
        } else {
            break;
        }
    }
    rcu_read_unlock();
    
    Message_Buf[offset_buf] = '\0';
    Message_Ptr = Message_Buf;
    return offset_buf;
}

module_init(task_walker_init);
module_exit(task_walker_exit);