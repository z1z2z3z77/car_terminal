#include "ap3216c.h"
#include "plog/Log.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>



AP3216C::AP3216C(const std::string &_dev_path) : fd(-1), dev_path(_dev_path)
{
    this->openDevice();
}

AP3216C::~AP3216C()
{
    this->closeDevice();
}

void AP3216C::openDevice()
{
    this->fd = open(dev_path.c_str(), O_RDWR);
    if(this->fd < 0)
    {
        PLOGE << "fail to open ap3216c device" << this->dev_path;
        throw::std::runtime_error("fail to open ap3216c device");
    }
    PLOGI << "led device opened:"<<this->dev_path;
}


void AP3216C::closeDevice()
{   
    if(this->fd >=0)
    {
    close(this->fd);
    this->fd = -1;
    PLOGI << "led device closed " << this->dev_path;
    }
}

SensorData AP3216C::readData()
{
    SensorData data = {0, 0, 0};
    if(this->fd < 0)
    {
        PLOGE << "ap3216c device is not open";
        return data;
    }

    short raw_data[3] = {0};
    int ret = read(this->fd, raw_data, sizeof(raw_data));
    if(ret < 0)
    {
        PLOGE << "fail to read ap3216c data";
        return data;
    }

    data.ir_data = (uint16_t)raw_data[0];
    data.als_data = (uint16_t)raw_data[1];

    data.ps_data = (uint16_t)raw_data[2];

    return data;
}

bool AP3216C::isOpen() const
{
    return this->fd >=0;
}
