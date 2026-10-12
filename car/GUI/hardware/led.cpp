#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdexcept>
#include"led.h"
#include "plog/Log.h"

LED::LED(const std::string &_dev_path) : fd(-1), is_on(false), dev_path(_dev_path)
{
    this->openDevice();
}

LED::~LED()
{
    this->closeDevice();
}

void LED::openDevice()
{
    this->fd = open(dev_path.c_str(), O_RDWR);
    if(this->fd < 0)
    {
        throw std::runtime_error("fail to open device");

    }
    PLOGI << "led device opened:"<<this->dev_path;
}


void LED::closeDevice()
{
    close(this->fd);
    this->fd = -1;
    PLOGI << "led device closed " << this->dev_path;
}


void LED::on()
{
    if(this->is_on)
        return ;
    this->is_on = true;
    ioctl(this->fd, LED_ON);
    PLOGI << "led turned on:"<<this->dev_path;
}


void LED::off()
{
    if(!this->is_on)
        return ;
    this->is_on = false;
    ioctl(this->fd, LED_OFF);
    PLOGI << "led turned off: " << this->dev_path; 
}