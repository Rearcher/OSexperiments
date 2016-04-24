#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#define DEVICE_NAME "helloworld"
#define CLASS_NAME "myhello"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("rahul <huanhuan7719@gmail.com>");
MODULE_DESCRIPTION("linux kernel module experiments");
MODULE_VERSION("0.1");

static int majorNumber;			 					// store device number, which is dynamically allocated
static char message[256] = {0};	 					// buffer to restore message sent from user
static short size_of_message;	 					// length of message
static struct class *helloworldClass = NULL;		// device-driver class struct pointer
static struct device *helloworldDevice = NULL;		// device-driver device struct pointer

static int hello_open(struct inode *, struct file *);
static void hello_release(struct inode *, struct file *);
static int hello_read(struct file *, char *, int, loff_t *);
static int hello_write(struct file *, char *, int, loff_t *);

struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.open = hello_open,
	.release = hello_release,
	.read = hello_read,
	.write = hello_write,
};

static int hello_open(struct inode *pinode, struct file *pfile) {
	printk("hello_open() is called\n");
	return 0;
}

static void hello_release(struct inode *pinode, struct file *pfile) {
	printk("hello_release() is called\n");
}

static int hello_read(struct file *pfile, char *buffer, int cnt, loff_t *offset) {
	int error_cnt = 0;
	error_cnt = copy_to_user(buffer, message, size_of_message);

	if (error_cnt == 0) {
		printk(KERN_INFO "Sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);
	} else {
		printk(KERN_INFO "Failed to send %d characters to the user\n", size_of_message);
		return -EFAULT;
	}
}

static int hello_write(struct file *pfile, char *buffer, int cnt, loff_t *offset) {
	sprintf(message, "%s(%d letters)", buffer, cnt);
	size_of_message = strlen(message);
	printk("Received %d characters from the user\n", cnt);
	return cnt;
}

static int hello_init(void) {
	// dynamically allocate a major number for the device
	majorNumber = register_chrdev(0, DEVICE_NAME, &hello_fops);
	if (majorNumber < 0) {
		printk("helloworld: failed to register a major number\n");
		return majorNumber;
	}
	
	// register the device class
	helloworldClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(helloworldClass)) {
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk("helloworld: failed to register device class\n");
		return PTR_ERR(helloworldClass);
	}
	
	// register the device driver
	helloworldDevice = device_create(helloworldClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(helloworldDevice)) {
		class_destroy(helloworldClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk("helloworld: Failed to create the device\n");
		return PTR_ERR(helloworldDevice);
	}

	printk("My kernel module: Hello World created correctly!\n");
	printk("With major number %d\n", majorNumber);
	return 0;
}

static void hello_exit(void) {
	device_destroy(helloworldClass, MKDEV(majorNumber, 0));		// remove device
	class_unregister(helloworldClass);							// unregister the device class
	class_destroy(helloworldClass);								// remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);				// unregister the major number
	printk("Hello World Goodbye!\n");
}

module_init(hello_init);
module_exit(hello_exit);
