#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <unistd.h>

#define CHANNELS 6
#define I2C_ADDR_ARDUINO 0x09
#define I2C_ADDR_SERVO 0x40

short map(char val, char fromlow, char fromhigh, short tolow, short tohigh)
{
  float fVal = val;
  fVal -= fromlow;
  fVal /= (fromhigh-fromlow);
  fVal *= (tohigh-tolow);
  fVal += tolow;
  return (short)fVal;
}

int main (void) {
    char buffer[CHANNELS];
    int fd;
    fd = open("/dev/i2c-1", O_RDWR);

while(1)
{
    // Target the arduino
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR_ARDUINO) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        return 1;
    }

    // Read the buffer
    read(fd, buffer, CHANNELS);

    // Target the servo driver
    if (ioctl(fd, I2C_SLAVE, I2C_ADDR_SERVO) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        return 1;
    }

     
    char pwm[5];
    for (int i = 0; i < CHANNELS; i++)
    {
 /*      short val = buffer[i] == 0 ? 0 : map(buffer[i], 1, 0xFF, 900, 2100);
       printf("%d\t", (int)val);
       if (!buffer[i]) continue;
*/
       pwm[0] = 6 + (4*i);
       
       pwm[1] = 0;
       pwm[2] = 0;   
  /*     
       double pulse = 1000000.0;
       pulse / 50.0;
       pulse /= 4096.0;
       pulse = (double)val / pulse;

       val = (short)pulse; 
*/
       pwm[3] = buffer[i]; //(char)val;
       pwm[4] = 0; //(char)(val >> 8);

       write(fd, pwm, 5);

    }
    usleep(50);
}
  
    return 0;
}
