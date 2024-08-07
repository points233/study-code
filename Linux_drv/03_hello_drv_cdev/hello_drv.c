#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

/* 1.确定主设备号 */
static char hello_buf[1024];
static struct class *hello_class;
static struct cdev hello_cdev;
static dev_t dev;

/* 3.实现对应的open/read/write等函数，填入file_operations结构体 */
static int hello_open (struct inode *node, struct file *file)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}

static ssize_t hello_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    int err;
    unsigned long len = size > 100 ? 100 : size;
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    err = copy_to_user(buf, hello_buf, len);
    return len;
}

static ssize_t hello_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    int err;
    unsigned long len = size > 100 ? 100 : size;
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    err = copy_from_user(hello_buf, buf, len);
    return len;
}

static int hello_close (struct inode *node, struct file *file)
{
    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}


/* 2.定义自己的file_operations结构体 */
static struct file_operations hello_drv = {
	.owner		= THIS_MODULE,
    .open       = hello_open,
    .read       = hello_read,
    .write      = hello_write,
    .release    = hello_close,

};

/* 4.把的file_operations结构体告诉内核：注册驱动程序 */


/* 5.谁来注册驱动程序？得有一个入口函数：安装驱动程序时，就回去调用这个入口函数 */
static int __init hello_init(void)
{
    int ret;
    

    ret = alloc_chrdev_region(&dev, 0, 1, "hello");
	if (ret < 0)
    {
		printk(KERN_ERR "alloc_chrdev_region() failed for hello\n");
		return -EINVAL;
	}

    cdev_init(&hello_cdev, &hello_drv);

    ret = cdev_add(&hello_cdev, dev, 1);
    if(ret)
    {
		printk(KERN_ERR "cdev_add() failed for hello\n");
		return -EINVAL;
	}

	hello_class = class_create(THIS_MODULE, "hello");
	ret = PTR_ERR(hello_class);
	if (IS_ERR(hello_class))
    {
        printk("failed to allocate class\n");
        return -1;
    }

    device_create(hello_class, NULL, dev, NULL, "hello");

    return 0;
}

/* 6.有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数 */
static void __exit hello_exit(void)
{
    device_destroy(hello_class, dev);
    class_destroy(hello_class);
    // unregister_chrdev(major, "hello");
    cdev_del(&hello_cdev);
    unregister_chrdev_region(dev, 1);
}


/* 7.其他完善：提供设备信息，自动创建设备节点 */
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");