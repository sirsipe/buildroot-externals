#include "raylib.h"
#include <gps.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define CHANNELS 6
#define I2C_ADDR_ARDUINO 0x09
#define I2C_ADDR_SERVO 0x40

#define MASTER_PING_GPIO "4"

#define MODE_STR_NUM 4

static int KILL_SIGNAL = 0;


static char* mode_str[MODE_STR_NUM] = {
	"n/a",
	"None",
	"2D",
	"3D"
};

enum MasterPingFileType
{
	direction,
	value
};

void GetMasterPingFile(char* buffer, enum MasterPingFileType type)
{
	strcat(buffer, "/sys/class/gpio/gpio");
        strcat(buffer, MASTER_PING_GPIO);

	switch (type)
	{
		case direction:
			strcat(buffer, "/direction");
			break;
		case value:
			strcat(buffer, "/value");
                        break;
	}
}

int SetupMasterPing()
{

	int fd = open("/sys/class/gpio/export", O_WRONLY);
	if (!fd)
	{
		printf("Failed to export master ping gpio");
		return 0;
	}
	write(fd, MASTER_PING_GPIO, 1);
	close(fd);
	
	char dirFile[128] = { NULL };
	GetMasterPingFile(dirFile, direction);

	fd = open(dirFile, O_WRONLY);
	if(!fd)
	{
		printf("Failed to set master ping gpio direction");
                return 0;
	}
	
	write(fd, "out", 3);
	close(fd);

	return 1;
}

void* PingPong(void* ptr)
{

	int iVal = 1;
	char valFile[128] = { NULL };
        GetMasterPingFile(valFile, value);
	int fd = open(valFile, O_WRONLY);

	while(fd && !KILL_SIGNAL)
	{
		if (iVal)
		{
			iVal = 0;
			write(fd, "1", 1);
			printf("Ping\n");
		}
		else
		{
			iVal = 1;
			write(fd, "0", 1);
			printf("Pong\n");
		}

	        struct timespec tsr, ts = {0, 300000000L}; //300ms
		if(nanosleep(&ts, &tsr) < 0)
		{
			printf("SLEEP FAILED\n");
		}
	}

	close(fd);
	return NULL;
}



int main(void)
{


	if (!SetupMasterPing())
		return 0;

	pthread_t PingPongThread;
	pthread_create(&PingPongThread, NULL, *PingPong, NULL);
	
	usleep(1000);

	

    char buffer[CHANNELS];
    int fd;
    fd = open("/dev/i2c-1", O_RDWR);


    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose())
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

        
        BeginDrawing();
        ClearBackground(BLANK);
        for(int i = 0; i < CHANNELS; i++)
        {
	    	char str[100] = {0};
		sprintf(str, "CH %d: %d", (i+1), (int)buffer[i]);
            	DrawText(str, 190, 200 + (25*i), 20, WHITE);
            	DrawText(str, 191, 201 + (25*i), 20, BLACK);
        }
        EndDrawing();

        struct timespec tsr, ts = {0, 100000000L}; //100ms
        if(nanosleep(&ts, &tsr) < 0)
        {
        	printf("SLEEP FAILED\n");
        }

    }

    CloseWindow();

    KILL_SIGNAL = 1;
    pthread_join(PingPongThread, NULL);

    return 0;
}
