#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>

#include "asm-generic/gpio.h"
#include "asm/gpio.h"
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

typedef struct {

    struct input_dev *input;
    struct timer_list debounce_timer;
    int gpio_index;
    int irq_index;
}key_device;
static irqreturn_t key_interrupt(int irq, void*dev_id)
{
    key_device *dev = dev_id;
    pr_info("key_0 interrupt");
    mod_timer(&dev->debounce_timer, jiffies+msecs_to_jiffies(10));
    
    return IRQ_HANDLED;
} 
static void key_bounce_handler(unsigned long data)
{
    key_device *dev =(key_device *)data ;

    int val;
    
    val = gpio_get_value(dev->gpio_index);
    
    input_report_key(dev->input, KEY_0, !val);
    input_sync(dev->input);
}

static int key_probe(struct platform_device *pdev)
{
    key_device *dev ;
    struct input_dev *input;
    int ret ;
    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if(!dev)
    {
        return -ENOMEM;
    }
    dev->gpio_index = of_get_named_gpio(pdev->dev.of_node, "key-gpio", 0);
    if(dev->gpio_index<0)
    {
        dev_err(&pdev->dev,"get gpio failed");
        return dev->gpio_index;
    }
    dev->irq_index = gpio_to_irq(dev->gpio_index);
     ret = devm_request_irq(&pdev->dev, dev->irq_index, key_interrupt, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
     "key_interrupt",dev);
    if(ret)
    {
        dev_err(&pdev->dev,"request irq %d failed",dev->irq_index);
        goto err_irq;

    }
    input = input_allocate_device();
    if(!input)
    {
        ret = -ENOMEM;
        goto err_input;
    }
    input->name = pdev->name;

    input->phys = "gpio-keys/input0";

    input->id.bustype = BUS_HOST;

    __set_bit(EV_KEY,input->evbit);

    __set_bit(KEY_0,input->keybit);

    dev->input = input;

    platform_set_drvdata(pdev,dev);

    setup_timer(&dev->debounce_timer, key_bounce_handler, (unsigned long)dev);

    ret = input_register_device(dev->input);
    if(ret)
    {
        dev_err(&pdev->dev,"register input failed\n");
        goto err_reg;
    }
    return 0;
err_reg:
    input_free_device(dev->input);

err_input:
    gpio_free(dev->gpio_index);
err_irq:
    return ret;
}

static int key_remove(struct platform_device *pdev)
{   
    key_device *dev =platform_get_drvdata(pdev);

    del_timer_sync(&dev->debounce_timer);
    input_unregister_device(dev->input);
    gpio_free(dev->gpio_index);
    return 0;
}
const struct of_device_id key_of_match[] ={
    {.compatible = "key"},
    {},
};

MODULE_DEVICE_TABLE(of, key_of_match);




struct platform_driver key_driver ={
    .probe = key_probe,
    .remove = key_remove,
    .driver ={
        .name = "key",
        .of_match_table = key_of_match,
    },
    
};
module_platform_driver(key_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LRQ");
MODULE_DESCRIPTION("Platform-based GPIO Key Driver");