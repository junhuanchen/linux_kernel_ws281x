#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/types.h>
//#include <linux/hrtimer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junhuanchen");
MODULE_DESCRIPTION("Kernel module for x3 using WS2812 LED");

#define DEV_NAME	"ws2812"
#define CLASS_NAME	"led"

// #define GPIO_SET_VALUE_DELTA	380

#define TRESET_US	60
// #define T0H_NS		(400 - GPIO_SET_VALUE_DELTA)
// #define T0L_NS		(850 - GPIO_SET_VALUE_DELTA)
// #define T1H_NS		(800 - GPIO_SET_VALUE_DELTA)
// #define T1L_NS		(450 - GPIO_SET_VALUE_DELTA)

// FIXME: Hardcoded for now. Will be changed in the future
#define GPIO_PIN 102
#define LED_COUNT 2

/* High precision nanodelay */
// #define hpr_set(timer, delay) \
// 	timer = ktime_get_ns();\
// 	timer += (delay);

// #define hpr_wait(timer) \
// 	while (ktime_get_ns() < timer);

static int major_num;
static struct class* class;
static struct device* dev;
static uint8_t usercount;
static uint8_t led_data[LED_COUNT * 3] = { 0 };

DEFINE_SPINLOCK(spin_lock_dls);
uint32_t spin_lock_flags;

static void led_sync(void) // GRB
{
	// led_data[0] = 0x00;
	// led_data[1] = 0xFF;
	// led_data[2] = 0x00;
	// led_data[3] = 0x00;
	// led_data[4] = 0xFF;
	// led_data[5] = 0x00;

	// led_data[0] = 0x00;
	// led_data[1] = 0x00;
	// led_data[2] = 0xFF;
	// led_data[3] = 0x00;
	// led_data[4] = 0x00;
	// led_data[5] = 0xFF;

	// led_data[0] = 0xFF;
	// led_data[1] = 0x00;
	// led_data[2] = 0x00;
	// led_data[3] = 0xFF;
	// led_data[4] = 0x00;
	// led_data[5] = 0x00;
	
	uint8_t mask = 0;
	// uint64_t hpr_timer;

	// for (int i = 0; i < sizeof(led_data); ++i) {
	// 	printk(KERN_INFO DEV_NAME " %d : %d\n", i, led_data[i]);
	// }

	/* GPIO speed workaround */
	spin_lock_irqsave(&spin_lock_dls, spin_lock_flags);
	for (int i = 0; i < 1000; ++i) {
		gpio_set_value(GPIO_PIN, 0);
	}
	spin_unlock_irqrestore(&spin_lock_dls, spin_lock_flags);

	/* Sending 3 bytes per LED */
	spin_lock_irqsave(&spin_lock_dls, spin_lock_flags);
	for (uint8_t i = 0; i != sizeof(led_data); ++i) {
		mask = 0x80;
		for (uint8_t j = 0; j != 8; ++j) {
			gpio_set_value(GPIO_PIN, 1);
			if (led_data[i] & mask) {
				// hpr_set(hpr_timer, T1H_NS);
				// gpio_set_value(GPIO_PIN, 1);
				// hpr_wait(hpr_timer);
				ndelay(380); // 850us - 900us

				// hpr_set(hpr_timer, T1L_NS);
				gpio_set_value(GPIO_PIN, 0);
				// hpr_wait(hpr_timer);
				// ndelay(50); // 400us - 500us
			} else {
				// hpr_set(hpr_timer, T0H_NS);
				// gpio_set_value(GPIO_PIN, 1);
				// hpr_wait(hpr_timer);
				// ndelay(50); // 400us - 500us

				// hpr_set(hpr_timer, T0L_NS);
				gpio_set_value(GPIO_PIN, 0);
				// hpr_wait(hpr_timer);
				ndelay(380); // 850us - 900us
			}
			mask >>= 1;
		}
	}
	spin_unlock_irqrestore(&spin_lock_dls, spin_lock_flags);

	/* Wait Treset time before sending new sequence */
	udelay(TRESET_US);
	

}

static int dev_open(struct inode *inode, struct file *file)
{
	// FIXME: Temp. To avoid working on synchronization
	if (usercount) {
		return -EFAULT;
	}

	++usercount;
	return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
// FIXME: Temp. For testing purposes.
	// led_sync();
// ENDFIX

	--usercount;
	return 0;
}

static ssize_t dev_read(struct file *file, char * buff, size_t len, loff_t *off)
{
	int ret;
	if (len < sizeof(led_data)) {
		return -EFAULT;
	}

	ret = copy_to_user(buff, led_data, sizeof(led_data));
	if (ret) {
		return -EFAULT;
	}

	return len;
}

static ssize_t dev_write(struct file *file, const char *buff, size_t len, loff_t *off)
{
	int ret;
	// if (*off >= sizeof(led_data)) {
	// 	return 0;
	// }
	
	// printk(KERN_INFO DEV_NAME ": %d\n", len);
	for (int i = 0; i < len; ++i) {
		led_data[i] = buff[i];
		// printk(KERN_INFO DEV_NAME ": %d\n", led_data[i]);
	}
	// ret = copy_from_user(led_data, sizeof(led_data), buff);
	// printk(KERN_INFO DEV_NAME ": %d\n", ret);
	led_sync();
	return sizeof(led_data);
}

static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init mod_init(void)
{
	int res;

	major_num = register_chrdev(0, DEV_NAME, &fops);
	if (major_num < 0) {
		printk(KERN_ALERT DEV_NAME ": chrdev register failed\n");
		return major_num;
	}

	class = class_create(THIS_MODULE, CLASS_NAME);
	if (!class) {
		unregister_chrdev(major_num, DEV_NAME);
		printk(KERN_ALERT DEV_NAME ": class create failed\n");
		return -EFAULT;
	}

	dev = device_create(class, NULL, MKDEV(major_num, 0), NULL, DEV_NAME);
	if (!dev) {
		class_destroy(class);
		unregister_chrdev(major_num, DEV_NAME);
		printk(KERN_ALERT DEV_NAME ": dev create failed\n");
		return -EFAULT;
	}

	res = gpio_request(GPIO_PIN, DEV_NAME);
	if (res) {
		return -EFAULT;
	}

	res = gpio_direction_output(GPIO_PIN, 0);
	if (res) {
		return -EFAULT;
	}

	led_sync();

	printk(KERN_INFO DEV_NAME ": init done\n");
	return 0;
}

static void __exit mod_exit(void)
{
	device_destroy(class, MKDEV(major_num, 0));
	class_unregister(class);
	class_destroy(class);
	unregister_chrdev(major_num, DEV_NAME);

	gpio_free(GPIO_PIN);
	printk(KERN_INFO DEV_NAME ": deinit done\n");
}

module_init(mod_init);
module_exit(mod_exit);
