#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <linux/input.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
const std::string key_device ="/dev/input/event3";

int main()
{

    struct input_event event;
    int fd = open(key_device.c_str(),O_RDONLY);
    if(fd<0)
    {
        perror("event not exists");
        
        exit(-1);

    }

    while(1)
    {
        if(read(fd, &event, sizeof(struct input_event)) != sizeof(struct input_event))
        {
            perror("read failed");
            exit(-1);
        }
        if(event.type == EV_KEY)
        {
            switch(event.value)
            {
                case 0:
                    printf("code %d : 松开\n", event.code);
                    break;
                case 1:
                    printf("code %d : 按下\n", event.code);
                    break;
                case 2:
                    printf("cdoe %d : 长按\n", event.code);
                    break;
            }
        }
    }

   return 0; 
}