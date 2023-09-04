#include <i2cps.h>
int setI2C(unsigned int index, long slave_addr){};
int unsetI2C(int i2c_fd){};
int writeI2C_asFile(int i2c_fd, unsigned char writebuffer[],
                    unsigned char bytes){};
int readI2C_asFile(int i2c_fd, unsigned char readbuffer[], unsigned char bytes){};
