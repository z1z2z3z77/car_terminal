#include <asm/uaccess.h>

#include <linux/init.h>
#include <linux/kern_levels.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>

#include "ap3216c.h"
#include "linux/device.h"
#include "linux/of_gpio.h"
#include "linux/printk.h"


typedef struct{

	dev_t dev_id ;
	int major;
	int minor;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	struct i2c_client *client;
	int irq_index;
	int int_gpio_index;
	struct work_struct work;
	struct mutex mtx;

	uint16_t ps_data;
	uint16_t als_data;
	uint16_t ir_data;
	
}ap3216c_dev;

static int ap3216c_read_regs(ap3216c_dev *ap3216c_device, u8 reg, void *val, int len)
{
	int ret;
	struct i2c_msg msg[2];
	struct i2c_client *client = ap3216c_device->client;

	msg[0].addr = client->addr ;
	msg[0].flags = 0 ;
	msg[0].buf = &reg ;
	msg[0].len = 1;

	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD ;
	msg[1].buf = val ;
	msg[1].len = len ;
     ret = i2c_transfer(client->adapter, msg, 2);
	 if(ret == 2)
	 {
		ret = 0;
	 }
	 else
	 {
		dev_err(&(ap3216c_device->client->dev),"i2c read failed=%d reg =%06x len= %d \n", ret ,reg ,len);
		ret = -EREMOTEIO;
	 }
	 return ret;
}
static int ap3216c_write_regs(ap3216c_dev *ap3216c_device, u8 reg, u8 *buf, u8 len)
{
	u8 b[256];
	struct i2c_msg msg;
	struct i2c_client *client = ap3216c_device->client;
	if(client == NULL)
	{
		pr_err("ap3216c: i2c client is NULL\n");
		return 0;
	}
	b[0] = reg;
	memcpy(&b[1], buf, len);

	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = b;
	msg.len =len + 1;
	
	return i2c_transfer(client->adapter, &msg ,1);

}
static int ap3216c_write_reg(ap3216c_dev *ap3216c_device, u8 reg, u8 data)
{
	u8 buf = 0;
	buf = data ;
	ap3216c_write_regs(ap3216c_device, reg, &buf, 1);
}
static int ap3216c_read_reg(ap3216c_dev *ap3216c_device, u8 reg)
{
   u8 data = 0;
   ap3216c_read_regs(ap3216c_device, reg, &data, 1);
   return data;
}

void ap3216c_read_data( ap3216c_dev *ap3216c_device)
{
	unsigned char i = 0;
	unsigned char buf[6];
	for(i = 0; i < 6; i++)
	{
		buf[i] = ap3216c_read_reg(ap3216c_device, AP3216C_IRDATA_L + i);
	}
	mutex_lock(&ap3216c_device->mtx);
	if(buf[0] & 0x80)
	{
		ap3216c_device->ir_data = 65535;
		dev_info(&ap3216c_device->client->dev, "IR is invalid.");

	}
	else
	ap3216c_device->ir_data = ((unsigned short) buf[1]<<2| buf[0] & 0x03);

	ap3216c_device->als_data = ((unsigned short)buf[3]<<8 | buf[2]);
	if(buf[4] & 0x40)
	{
		ap3216c_device->ps_data = 65535;
		dev_info(&ap3216c_device->client->dev, "ps data is invalid");
	}
	else
	{
		ap3216c_device->ps_data = ((unsigned short)(buf[5] & 0x3F)<<4 )| (buf[4] & 0x0F );
	}
	mutex_unlock(&ap3216c_device->mtx);

}
static irqreturn_t interrupt_data(int irq, void* dev_id)
{
	ap3216c_dev *ap3216c_device = (ap3216c_dev *)dev_id;
	schedule_work(&(ap3216c_device->work));
	dev_info(&(ap3216c_device->client->dev),"interrupt occured\n");
	return IRQ_HANDLED;
}
static void work_callback(struct work_struct *work)
{
	ap3216c_dev *ap3216c_device = container_of(work, ap3216c_dev, work);
	ap3216c_read_data(ap3216c_device);
}

static int ap3216c_open(struct inode *inode, struct file *file)
{
	file->private_data = container_of(inode->i_cdev, ap3216c_dev, cdev);
	ap3216c_write_reg(file->private_data, AP3216C_SYSEMCONG, 0X04);
	mdelay(50);
	ap3216c_write_reg(file->private_data, AP3216C_SYSEMCONG, 0X03);
	ap3216c_write_reg(file->private_data, AP3216C_INTCLEAR, 0X00);
	ap3216c_dev *p = file ->private_data;
	if(p && p->irq_index >=0)
	{
		enable_irq(p->irq_index);
		dev_info(&p->client->dev, "irq %d enabled\n",p->irq_index);
	}
	dev_info(&p->client->dev,"device opened !");
	return 0;
	
}
static ssize_t ap3216c_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	short data[3];
	ap3216c_dev *ap3216c_device = file->private_data;
	mutex_lock(&ap3216c_device->mtx);
	data[0] = ap3216c_device->als_data;
	data[1] = ap3216c_device->ps_data;
	data[2] = ap3216c_device->ir_data;
	mutex_unlock(&ap3216c_device->mtx);
	copy_to_user(buf,data,sizeof(data));
	return 0;
}


static int ap3216c_release(struct inode *inode, struct file *file)
{
	ap3216c_dev *ap3216c_device = file->private_data;
	if(!ap3216c_device)
		return 0;
	disable_irq(ap3216c_device->irq_index);
	cancel_work_sync(&ap3216c_device->work);
	dev_info(&ap3216c_device->client->dev,"device released\n");
	return 0;
}
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = ap3216c_open,
	.release = ap3216c_release,
	.read = ap3216c_read,
};
static int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    ap3216c_dev *ap3216c_device = devm_kzalloc(&client->dev,sizeof(ap3216c_dev),GFP_KERNEL);
	ap3216c_device->client = client;
	i2c_set_clientdata(client,ap3216c_device);
	mutex_init(&ap3216c_device->mtx);
	if(ap3216c_device->major)
	{
		ap3216c_device->dev_id = MKDEV(ap3216c_device->major,0);
		register_chrdev_region(ap3216c_device->dev_id,1,"ap3216cc");
	}
	else
	{
		alloc_chrdev_region(&ap3216c_device->dev_id,0,1,"ap3216cc");
		ap3216c_device->major = MAJOR(ap3216c_device->dev_id);
		ap3216c_device->minor = MINOR(ap3216c_device->dev_id);
	}
	ap3216c_device->cdev.owner = THIS_MODULE;
	cdev_init(&ap3216c_device->cdev,&fops);
	int ret =
	cdev_add(&ap3216c_device->cdev ,ap3216c_device->dev_id,1);
	if(ret)
	{
		dev_err(&client->dev,"error %d adding ap3216c",ret);
		unregister_chrdev_region(ap3216c_device->dev_id,1);
		return ret;
	}
	ap3216c_device->class = class_create(THIS_MODULE,"ap3216c");
	if (IS_ERR(ap3216c_device->class))
        return PTR_ERR(ap3216c_device->class);
	ap3216c_device->device = device_create(ap3216c_device->class,NULL,ap3216c_device->dev_id,NULL,"ap3216c");
	if (IS_ERR(ap3216c_device->device))
        return PTR_ERR(ap3216c_device->device);

	ap3216c_device->int_gpio_index = of_get_named_gpio(client->dev.of_node,"int-gpio",0);
	ap3216c_device->irq_index = gpio_to_irq(ap3216c_device->int_gpio_index);

	ret = devm_request_irq(&client->dev,ap3216c_device->irq_index,interrupt_data,
		IRQF_TRIGGER_RISING |IRQF_TRIGGER_FALLING,"ap3216",ap3216c_device);
	if(ret)
	{
		dev_err(&client->dev,"failed to request irq %d\n",ap3216c_device->irq_index);
		return ret;

	}
	if(ap3216c_device->irq_index>=0)
		disable_irq(ap3216c_device->irq_index);
	INIT_WORK(&ap3216c_device->work,work_callback);
	dev_info(&client->dev,"ap3216c probe successful\n");
	return 0;
}


static int ap3216c_remove(struct i2c_client *client)
{	
	ap3216c_dev *ap3216c_device = i2c_get_clientdata(client);

	device_destroy(ap3216c_device->class,ap3216c_device->dev_id);
	class_destroy(ap3216c_device->class);
	cdev_del(&ap3216c_device->cdev);
	unregister_chrdev_region(ap3216c_device->dev_id,1);
	gpio_free(ap3216c_device->int_gpio_index);
	devm_free_irq(&client->dev,ap3216c_device->irq_index,ap3216c_device);

	return 0;

}
static const struct i2c_device_id ap3216c_id[] = {
	{"ap3216c",0},
	{}
};
static const struct of_device_id i2c_ap3216c[] = {
	{ .compatible = "alientek,ap3216c"},
	{ }
};
static struct i2c_driver ap3216c_i2c_driver = {
	.probe	= ap3216c_probe,
	.remove	= ap3216c_remove,
	.driver	= {
		.name	= "ap3216c",
		.of_match_table = i2c_ap3216c,
	},
	.id_table = ap3216c_id,
};





static int __init ap3216c_module_init(void)
{
	return i2c_add_driver(&ap3216c_i2c_driver);
}

static void __exit ap3216c_module_exit(void)
{
	return i2c_del_driver(&ap3216c_i2c_driver);
}
module_init(ap3216c_module_init);
module_exit(ap3216c_module_exit);
MODULE_LICENSE("GPL");