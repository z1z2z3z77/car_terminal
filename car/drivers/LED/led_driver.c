#include "linux/printk.h"
#include <asm/uaccess.h>

#include <linux/init.h>
#include <linux/kern_levels.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
// 内核空间必须用这个ioctl.h
#include <linux/ioctl.h>
#include <linux/of_gpio.h>
//定义了led控制的专属命令，一个是LED_0N(编号L-0) LED_OFF(编号L-1)
#define LED_MAGIC 'L'

//内核的标准宏，生成一个唯一的32的整数命令码
//_IO表示不传输数据
#define LED_ON    _IO(LED_MAGIC, 0)
#define LED_OFF   _IO(LED_MAGIC, 1)

struct led_device
{
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *device_tree_node;
    int gpio_index;
    struct kobject *kobj;
};
struct platform_device *g_pdev;

void led_gpio_init(struct platform_device *pdev)
{
    struct led_device *led_device =platform_get_drvdata(pdev);

    led_device->device_tree_node = pdev->dev.of_node;
    if(led_device->device_tree_node == NULL)
    {
        printk("gpioed node not find\r\n");
        return ;
    }

    led_device->gpio_index = of_get_named_gpio(led_device->device_tree_node, "led-gpio", 0);

    if(led_device->gpio_index < 0)
    {
        printk("can't get led-gpio\n");
        return;
    }
    printk("led's GPIO index = %d\r\n", led_device->gpio_index);

    gpio_request(led_device->gpio_index, "led0");
    gpio_direction_output(led_device->gpio_index, 1);

}
static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct led_device *led = container_of(file->f_inode->i_cdev, struct led_device, cdev);
    switch(cmd)
    {
    case LED_ON:
        pr_info("led on\n");
        gpio_set_value(led->gpio_index, 0);
        break;
    case LED_OFF:
        pr_info("led off\n");
        gpio_set_value(led->gpio_index, 1);
        break;
    
    default:
        return -EINVAL;
    }
    return 0;
}
static struct file_operations fops ={
    .owner = THIS_MODULE,
    .unlocked_ioctl = led_ioctl,
};
static ssize_t led_ctl_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
    struct led_device *dev = platform_get_drvdata(g_pdev);
    pr_info("gpio_index = %d", dev->gpio_index);
    int val = gpio_get_value(dev->gpio_index) ? 0 : 1;
    return sprintf(buf, "%d\n", val);
}

static ssize_t led_ctl_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
    struct led_device *led = platform_get_drvdata(g_pdev);
    int val;

    if(kstrtoint(buf , 0 ,&val))
        return count;
    if(val == 1)
    {
        gpio_set_value(led->gpio_index, 1);
        pr_info("led off\n");
    }
    else
    {
        gpio_set_value(led->gpio_index, 0);
        pr_info("led on\n");

    }
    return count;
}

static struct kobj_attribute led_ctl_attribute = __ATTR(led_ctl, 0664, led_ctl_show, led_ctl_store);

static int led_probe(struct platform_device *pdev)
{
    struct led_device *led_device =devm_kzalloc(&pdev->dev, sizeof(struct led_device), GFP_KERNEL);

    platform_set_drvdata(pdev, led_device);

    led_gpio_init(pdev);

    if(led_device->major)
    {
        led_device ->devid = MKDEV(led_device->major, 0);
        register_chrdev_region(led_device->devid, 1, "gpio-led");

    }
    else
    {
        alloc_chrdev_region(&led_device->devid, 0, 1, "gpio-led");
        led_device->major = MAJOR(led_device->devid);
        led_device->minor = MINOR(led_device->devid);
    }
    printk("gpioled major = %d, minor = %d\r\n",led_device->major, led_device->minor);
    
    led_device->cdev.owner =THIS_MODULE;
    cdev_init(&led_device->cdev, &fops);
    int ret = cdev_add(&led_device->cdev, led_device->devid, 1);
    if(ret)
    {
        printk(KERN_NOTICE "Error %d adding apio_led", ret);
        unregister_chrdev_region(led_device->devid, 1);
        return ret;
    }

    led_device->class =class_create(THIS_MODULE, "gpio-led");
    if(IS_ERR(led_device->class))
    {
        return PTR_ERR(led_device->class);
    }
    led_device->device = device_create(led_device->class, NULL, led_device->devid, NULL, "led");
    if(IS_ERR(led_device->device))
    {
        return PTR_ERR(led_device->device);
    }
    led_device->kobj = kobject_create_and_add("led-ctl-status", NULL);
    if(!led_device->kobj)
    {
        return -ENOMEM;
    }
    ret = sysfs_create_file(led_device->kobj, &led_ctl_attribute.attr);
    if(ret)
    {
        kobject_put(led_device->kobj);
        return ret;
    }
    g_pdev = pdev;
    pr_info("gpio_led module loaded\n");
    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    struct led_device *led = platform_get_drvdata(pdev);

    sysfs_remove_file(led->kobj, &led_ctl_attribute.attr);

    kobject_put(led->kobj);

    device_destroy(led->class, led->devid);

    class_destroy(led->class);

    cdev_del(&led->cdev);

    unregister_chrdev_region(led->devid,1);
    printk(KERN_INFO "gpio_led module unloaded");
}



static const struct  of_device_id led_of_match [] =
{
    {.compatible = "gpio-led"},
    {}
};

static struct platform_driver led_driver ={
    .probe = led_probe,
    .remove = led_remove,
    .driver = 
    {
        .name = "imx6ll-led",
        .of_match_table = led_of_match,
    },
};

static int __init led_module_init(void)
{
    return platform_driver_register(&led_driver);
}

static void __exit led_module_exit(void)
{
    return platform_driver_unregister(&led_driver);
}

module_init(led_module_init);

module_exit(led_module_exit);

MODULE_LICENSE("GPL");
