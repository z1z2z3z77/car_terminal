#include "V4L2Camera.h"
#include "plog/Log.h"

#include <cstdint>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cstring>

V4L2Camera::V4L2Camera(const std::string &_dev_path, uint32_t _width, uint32_t _height, uint32_t _frame_rate,
                       uint32_t _buffer_length)
    : dev_path(_dev_path), fd(-1), width(_width), height(_height), frame_rate(_frame_rate),
      is_capturing(false), buffer_length(_buffer_length), buffers(nullptr), frame_queue_length(_buffer_length)
{
    try
    {
        this->openDevice();  //打开设备节点
        this->setParameters();  //设置分辨率、格式和帧率
        this->initBuffer();  //申请内核缓冲区并进行内存映射 (mmap)
        PLOGI << "V4L2Camera Init " << this->dev_path << " Success!";
    }
    catch (std::exception &e)
    {
        PLOGE << e.what(); // 捕获初始化过程中的任何异常
    }
}

V4L2Camera::~V4L2Camera()
{
    this->stopCapture(); 
    this->deinitBuffer();
    this->closeDevice();
}
//开始采集
void V4L2Camera::startCapture()
{
    if (this->is_capturing == true)
        return;

    // ---------------------------------------------------------
    // 【核心修复】重新入队所有缓冲区
    // ---------------------------------------------------------
    // 原因：VIDIOC_STREAMOFF 会把所有缓冲区从驱动队列里踢出来。
    // 重启前必须把它们塞回去(QBUF)，否则驱动没有内存写数据，就会卡死。
    for (int i = 0; i < this->buffer_length; i++)
    {
        v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i; // 指定要入队的缓冲区索引

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        {
            PLOGE << "Failed to re-queue buffer index " << i;
            // 如果这里失败，通常意味着严重错误，但在重试逻辑中可以先记录日志
        }
    }

    // ---------------------------------------------------------
    // 开启视频流
    // ---------------------------------------------------------
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
        throw std::runtime_error("start capture failed");

    this->is_capturing = true;
    
    // 启动采集线程
    // 注意：之前的 thread 对象如果 join 过了，这里可以直接赋值覆盖
    this->capture_thread = std::thread(&V4L2Camera::dataCollectionLoop, this);
    
    PLOGI << "start capture success";
}
//结束采集
void V4L2Camera::stopCapture()
{
    if (this->is_capturing == false)
        return;

    // 【第一步】先设置标志位为 false
    // 这样当采集线程遇到 ioctl 失败时，它知道是因为“要退出了”，而不是“出故障了”
    this->is_capturing = false;

    // 【第二步】关闭视频流 
    // 这会强制唤醒正在阻塞等待数据的 ioctl
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
    {
        PLOGE << "StreamOff failed"; // 仅记录日志，不要抛出异常
    }

    // 【第三步】等待线程安全退出
    if (this->capture_thread.joinable())
        this->capture_thread.join();

    PLOGI << "Stop capture success";
}

void V4L2Camera::openDevice()
{   // 以读写模式 (O_RDWR) 打开设备文件
    this->fd = open(this->dev_path.c_str(), O_RDWR);
    if (fd < 0)
        throw std::runtime_error("Failed to open device " + this->dev_path);
    // 查询设备能力 v4l2_capability cap;
    // 填充结构体 cap
    v4l2_capability cap;
    // VIDIOC_QUERYCAP: 询问驱动支持什么功能
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
        throw std::runtime_error("Failed to query device capabilities");
    // 检查是否支持视频捕获 (V4L2_CAP_VIDEO_CAPTURE)
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        throw std::runtime_error("Device is not a video capture device");
    // 检查是否支持流式 I/O (V4L2_CAP_STREAMING)，我们需要用 mmap 方式
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
        throw std::runtime_error("Device does not support streaming I/O");

    PLOGI << "V4L2Camera Open " << this->dev_path;
}

void V4L2Camera::closeDevice()
{
    if (this->fd < 0)
        return;
    close(this->fd);
    this->fd = -1;
    PLOGI << "V4L2Camera Close" << this->dev_path;
}
// 获取参数
void V4L2Camera::getParameters()
{
    v4l2_fmtdesc format_desc;    // 像素格式描述(YUYV/MJPEG 等格式)
    v4l2_frmsizeenum frame_size; // 分辨率描述（640*480 等尺寸信息）
    v4l2_frmivalenum frame_val;  // 帧率描述 （30fps、60fps）

    format_desc.index = 0;
    format_desc.type = frame_size.type = frame_val.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    while (ioctl(fd, VIDIOC_ENUM_FMT, &format_desc) == 0)
    {
        PLOGI << "Pixel Format:" << format_desc.description;
        frame_size.index = 0;
        frame_size.pixel_format = format_desc.pixelformat;
        frame_val.pixel_format = format_desc.pixelformat;
        // 获得该像素格式的分辨率
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frame_size) == 0)
        {
            PLOGI << "-Resolution:" << frame_size.discrete.width << "x" << frame_size.discrete.height;
            frame_val.index = 0; //分辨率索引清零
            frame_val.width = frame_size.discrete.width;
            frame_val.height = frame_size.discrete.height;
            // 获得该分辨率的帧率
            while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frame_val) == 0)
            {
                PLOGI << "-Frame Rate:" << frame_val.discrete.denominator / frame_val.discrete.numerator;
                frame_val.index++;
            }
            frame_size.index++;
        }
        format_desc.index++;
    }
}
// 设置参数
void V4L2Camera::setParameters()
{   
    PLOGI << "Trying to set Format: MJPEG, Width: " << this->width << ", Height: " << this->height;
    v4l2_format frame_format;
     memset(&frame_format, 0, sizeof(frame_format)); 
    // 设置像素格式和分辨率
    frame_format = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .fmt =
            {
                .pix =
                    {
                        .width = this->width,
                        .height = this->height,
                        .pixelformat = V4L2_PIX_FMT_MJPEG, // 强制指定为 YUYV 格式
                        .field = V4L2_FIELD_NONE,
                    },
            },
    };
    // VIDIOC_S_FMT: Set Format
    if (ioctl(fd, VIDIOC_S_FMT, &frame_format) < 0)
    {  std::string err_msg = "Failed to set frame format. Errno: " + std::string(strerror(errno));      
     PLOGE << err_msg;       
     throw std::runtime_error(err_msg);   
    }

    // 设置帧率
    v4l2_streamparm stream_param = {.type = V4L2_BUF_TYPE_VIDEO_CAPTURE};
    // 先获取当前的流参数
    if (ioctl(fd, VIDIOC_G_PARM, &stream_param) < 0)
        throw std::runtime_error("Failed to get stream parameters");
    // 检查驱动是否支持更改帧率
    if (!(stream_param.parm.capture.capability & V4L2_CAP_TIMEPERFRAME))
        throw std::runtime_error("Device does not support setting frame rate");
    // 设置帧率的分母和分子
    stream_param.parm.capture.timeperframe.numerator = 1;
    stream_param.parm.capture.timeperframe.denominator = this->frame_rate;
    // VIDIOC_S_PARM: Set Stream Parameters
    if (ioctl(fd, VIDIOC_S_PARM, &stream_param) < 0)
        throw std::runtime_error("Failed to set stream parameters");
}

void V4L2Camera::initBuffer()
{   // 在用户空间分配管理结构体
    this->buffers = std::unique_ptr<BufferInfo[]>(new BufferInfo[this->buffer_length]);
    // 在内核申请缓冲区
    v4l2_requestbuffers req_buf;
    req_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req_buf.memory = V4L2_MEMORY_MMAP;   // 使用内存映射模式
    req_buf.count = this->buffer_length; // 申请数量
    if (ioctl(fd, VIDIOC_REQBUFS, &req_buf) < 0)
        throw std::runtime_error("Failed to request buffer");
    //逐个映射缓冲区
    v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    for (int i = 0; i < this->buffer_length; i++)
    {
        buf.index = i;
        // VIDIOC_QUERYBUF: 查询第 i 个缓冲区的物理信息（偏移量、长度）
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0)
            throw std::runtime_error("Failed to query buffer");
        
        // 映射缓冲区到用户空间
        // 记录长度
        this->buffers[i].length = buf.length;
        // mmap: 将内核空间的视频缓冲区映射到用户空间的指针 start
        // 映射到用户空间的起始地址为 NULL，系统会自动选择一个合适的地址
        // PROT_READ | PROT_WRITE: 映射区域可读可写
        // MAP_SHARED: 映射区域对所有映射此对象的进程可见，修改会同步到物理内存
        // fd: 设备文件描述符
        // buf.m.offset: 缓冲区在物理内存中的偏移量
        this->buffers[i].start = (uint8_t *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (this->buffers[i].start == MAP_FAILED)
            throw std::runtime_error("Failed to map buffer");
        
        // 将缓冲区放入采集队列
        //VIDIOC_QBUF: Queue Buffer。告诉驱动这个缓冲区是空的，可以填充视频数据了
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
            throw std::runtime_error("Failed to queue buffer");
    }
}

void V4L2Camera::deinitBuffer()
{
    if (this->buffers == nullptr)
        return;
    for (int i = 0; i < this->buffer_length; i++)
    {
        v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        // 出队
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0)
        {
            PLOGE << "Failed to dequeue buffer";
            // throw std::runtime_error("Failed to dequeue buffer");
        }
        // 解除映射
        munmap(this->buffers[i].start, this->buffers[i].length);
        this->buffers[i].start = nullptr;
    }
}

void V4L2Camera::dataCollectionLoop()
{
    v4l2_buffer buf; 
    
    while (this->is_capturing)
    {
        // 【关键 1】每次循环必须清零结构体！
        // 否则 buf 里残留的旧 index 或 dirty data 会导致 QBUF 失败
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // 【关键 2】移除 for 循环
        // 驱动不保证按顺序给数据，它给哪个 index，我们就处理哪个
        
        // --- 1. 出队 (Dequeue) ---
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0)
        {
            // 如果是因为点击了 Stop 而失败，直接退出，不报错
            if (this->is_capturing == false) break;

            PLOGE << "Failed to dequeue buffer";
            // 避免死循环狂刷 CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // --- 数据处理 ---
        int i = buf.index; // 获取驱动返回的 buffer 索引
        if (i >= 0 && i < this->buffer_length)
        {
            FrameData frame_data;
            frame_data.width = this->width;
            frame_data.height = this->height;
            // 获取 MJPEG 数据实际长度 (Bytes Used)
            frame_data.length = buf.bytesused; 

            // 申请内存
            frame_data.yuyv_data = std::shared_ptr<uint8_t>(new uint8_t[frame_data.length], [](uint8_t *p) { delete[] p; });
            
            // 拷贝数据 (注意使用 frame_data.length)
            memcpy(frame_data.yuyv_data.get(), this->buffers[i].start, frame_data.length);

            // 推入队列
            {   
                std::lock_guard<std::mutex> lock(this->frame_queue_mutex);
                this->frame_queue.push(frame_data);
                if (this->frame_queue.size() > this->frame_queue_length)
                    this->frame_queue.pop();
            }
        }

        // --- 2. 入队 (Queue) ---
        // 将用完的缓冲区还给驱动
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        {
            // 退出时 QBUF 失败是正常的，直接忽略
            if (this->is_capturing == false) break;
            
            PLOGE << "Failed to queue buffer"; 
        }
    }
}
//获取帧
// 从队列中取出一帧图像数据
// 如果队列为空，返回一个空的 FrameData 对象
FrameData V4L2Camera::getFrame()
{   // 加锁，从队列头部取出一帧数据
    std::lock_guard<std::mutex> lock(this->frame_queue_mutex);
    if (this->frame_queue.empty())
    {   // 队列为空时返回空对象
        FrameData temp;
        temp.yuyv_data = nullptr;
        return temp;
    }
        
    FrameData frame_data = this->frame_queue.front();
    this->frame_queue.pop();
    return frame_data;
}
