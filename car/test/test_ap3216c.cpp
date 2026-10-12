#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>


using namespace std;
const string ap3216c_device = "/dev/ap3216c";
int main()
{
    int fd = open(ap3216c_device.c_str(), O_RDWR);
    if (fd < 0)
    {
        cout << "open /dev/ap3216c failed" << endl;
        return -1;
    }
    uint16_t data[3];
    while(1)
    {
        read(fd, data ,sizeof(data));

        cout << "ir data: " << data[0] << endl; 
        cout << "als data: " << data[1] << endl;
        cout << "ps data: " <<data[2] << endl << endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    close(fd);
}