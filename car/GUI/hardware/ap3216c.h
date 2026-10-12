#pragma once  //只包含头文件一次
#include <string>
#include <cstdint>

struct SensorData
{
    uint16_t ir_data;  // 红外光数据
    uint16_t als_data; // 环境光数据
    uint16_t ps_data;  // 接近传感器数据
};

class AP3216C  // 类名：AP3216C
{
public:
    AP3216C(const std::string &_dev_path);
    ~AP3216C();
    
    /**
     * @brief 读取传感器数据
     * @return 返回包含 IR、ALS、PS 的传感器数据结构体
     */
    SensorData readData();
    
    /**
     * @brief 检查设备是否成功打开
     * @return 返回 true 表示设备已打开
     */
    bool isOpen() const;

private:  // 私有成员变量
    void openDevice();  // 打开设备文件
    void closeDevice();  // 关闭设备文件

    int fd;
    std::string dev_path;  // 设备文件路径
};