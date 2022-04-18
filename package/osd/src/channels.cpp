#include "channels.h"
#include <pthread.h>
#include <string.h>     // strcat
#include <unistd.h>     // nanosleep
#include <fcntl.h>      // O_WRONLY
#include <sys/ioctl.h>  // ioctl
#include <linux/i2c-dev.h> // I2C_SLAVE

#define MASTER_PING_GPIO "4"
#define I2C_ADDR_ARDUINO 0x09
#define I2C_ADDR_SERVO 0x40


Channels::eMasterPingStatus Channels::z_MasterPingStatus = Channels::eMasterPingStatusStopped;
pthread_t* Channels::zp_MasterPingThread = nullptr;
pthread_t* Channels::zp_ChannelRoutingThread = nullptr;
bool Channels::zb_ChannelRouting = false;
short Channels::z_Channels[CHANNELS] = { 0 };

Channels::eMasterPingStatus Channels::GetStatus()
{
    return z_MasterPingStatus;
}

void Channels::GetChannels(short channels[CHANNELS])
{
    for (size_t i = 0; i < CHANNELS; i++)
        channels[i] = z_Channels[i]; 
}

void Channels::SetStatus(Channels::eMasterPingStatus newStatus)
{
    z_MasterPingStatus = newStatus;
}

bool Channels::Takeover()
{
    if (GetStatus() != eMasterPingStatusStopped)
        return false;

    if (!SetupMasterPing())
        return false;

    SetStatus(eMasterPingStatusStarting);

    zp_MasterPingThread = new pthread_t();

    pthread_create(zp_MasterPingThread, NULL, *(Channels::MasterPingProc), NULL);

    while(GetStatus() == eMasterPingStatusStarting)
    {
        struct timespec tsr, ts = {0, 10000000L}; //10ms
		nanosleep(&ts, &tsr);
    }

    if (GetStatus() == eMasterPingStatusError)
    {
        Release();
        return false;
    }

    // Give arduino 500ms to react to slave select.
    struct timespec tsr2, ts2 = {0, 500000000L}; 
    nanosleep(&ts2, &tsr2);
    StartChannelRouting();

    return true;
}

void Channels::Release()
{
    if (GetStatus() == eMasterPingStatusRunning || GetStatus() == eMasterPingStatusError)
    {

        if (zb_ChannelRouting)
            StopChannelRouting();
        
        SetStatus(eMasterPingStatusStopping);
        pthread_join(*zp_MasterPingThread, NULL);
        delete zp_MasterPingThread;
        zp_MasterPingThread = nullptr;
        SetStatus(eMasterPingStatusStopped);
    }
}
    

void Channels::StartChannelRouting()
{
    if (zb_ChannelRouting)
        return;

    zb_ChannelRouting = true;
    zp_ChannelRoutingThread = new pthread_t();
    pthread_create(zp_ChannelRoutingThread, NULL, *(Channels::ChannelRoutingProc), NULL);
}

void Channels::StopChannelRouting()
{
    if (!zb_ChannelRouting)
        return;

    zb_ChannelRouting = false;
    pthread_join(*zp_ChannelRoutingThread, NULL);
    delete zp_ChannelRoutingThread;
    zp_ChannelRoutingThread = nullptr;
}

void Channels::GetMasterPingFile(char* buffer, enum eMasterPingFileType type)
{

	strcat(buffer, "/sys/class/gpio/gpio");
    strcat(buffer, MASTER_PING_GPIO);

	switch (type)
	{
		case eMasterPingFileTypeDirection:
			strcat(buffer, "/direction");
			break;
		case eMasterPingFileTypeValue:
			strcat(buffer, "/value");
            break;
    }
}

bool Channels::SetupMasterPing()
{

	int fd = open("/sys/class/gpio/export", O_WRONLY);
	if (!fd)
		return false;
	
	write(fd, MASTER_PING_GPIO, 1);
	close(fd);
	
	char dirFile[128] = "";
	GetMasterPingFile(dirFile, eMasterPingFileTypeDirection);

	fd = open(dirFile, O_WRONLY);
	if(!fd)
	    return false;
	
	write(fd, "out", 3);
	close(fd);

	return true;
}

void* Channels::MasterPingProc(void*)
{
    bool bVal = true;

	char valFile[128] = "";
    GetMasterPingFile(valFile, eMasterPingFileTypeValue);	
    int fd = open(valFile, O_WRONLY);

    if (!fd)
    {
        SetStatus(eMasterPingStatusError);
        return nullptr;
    }

    SetStatus(eMasterPingStatusRunning);

	while(GetStatus() == eMasterPingStatusRunning)
	{
		if (bVal)
			write(fd, "1", 1);
		else
			write(fd, "0", 1);
        bVal = !bVal;

        struct timespec tsr, ts = {0, 300000000L}; //300ms
		if(nanosleep(&ts, &tsr) < 0)
			SetStatus(eMasterPingStatusError);		
	}
	close(fd);
	return nullptr;
}

void* Channels::ChannelRoutingProc(void*)
{
    int fd = open("/dev/i2c-1", O_RDWR);
    char buffer[CHANNELS];
    while(zb_ChannelRouting)
    {

        if (ioctl(fd, I2C_SLAVE, I2C_ADDR_ARDUINO) < 0)
            continue;
            
        read(fd, buffer, CHANNELS);

        if (ioctl(fd, I2C_SLAVE, I2C_ADDR_SERVO) < 0)
            continue;

        char pwm[5];
        for (int i = 0; i < CHANNELS; i++)
        {
            short val = (short)buffer[i];// == 0 ? 0 : map(buffer[i], 1, 0xFF, 900, 2100);
            z_Channels[i] = val;

        //    printf("%d\t", (int)val);
            if (!buffer[i]) continue;
        
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

        struct timespec tsr, ts = {0, 100000000L}; //100ms
		nanosleep(&ts, &tsr);
    }

    return nullptr;
}
