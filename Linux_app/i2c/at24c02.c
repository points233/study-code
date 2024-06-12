#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "smbus.h"
#include "i2cbusses.h"
#include <time.h>

/* ./at24c02 <i2c_bus_num> w points233
 * ./at24c02 <i2c_bus_num> r
 */
int main(int argc, char **argv)
{
    unsigned char dev_addr = 0x50;
    unsigned char mem_add = 0;
    unsigned char buf[32];

    int file;
    char filename[20];
    unsigned char *str;
    struct timespec req;

    int ret;

    if(argc != 3 && argc != 4)
    {
        printf("Usage:\n");
        printf("write eeprom:%s <i2c_bus_num> w string\n", argv[0]);
        printf("read  eeprom:%s <i2c_bus_num> r\n", argv[0]);
        return -1;
    }

	file = open_i2c_dev(argv[1][0] - '0', filename, sizeof(filename), 0);
	if (file < 0)
    {
        printf("can't open %s\n", filename);
        return -1;
    }

    if(set_slave_addr(file, dev_addr, 1))
    {
        printf("can't set_slave_addr\n");
        return -1;
    }

    if(argv[2][0] == 'w')
    {
        // write str:argv[3]
        str = argv[3];
        req.tv_sec = 0;
        req.tv_nsec = 20000000;
        while(*str)
        {
            ret = i2c_smbus_write_byte_data(file, mem_add, *str);
            if(ret != 0)
            {
                printf("i2c_smbus_write_byte_data error\n");
                return -1;
            }
            nanosleep(&req, NULL);
            str++;
            mem_add++;
        }
        ret = i2c_smbus_write_byte_data(file, mem_add, 0);
        if(ret != 0)
        {
            printf("i2c_smbus_write_byte_data error\n");
            return -1;
        }
    }
    else
    {
        ret = i2c_smbus_read_i2c_block_data(file, mem_add, sizeof(buf), buf);
        if(ret < 0)
        {
            printf("i2c_smbus_write_byte_data error\n");
            return -1;
        }
        buf[31] = '\0';
        printf("get data:%s\n", buf);
    }

    return 0;
}