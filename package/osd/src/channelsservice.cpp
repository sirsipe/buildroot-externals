#include "channelsservice.h"
#include "i2cbus.h"

#include <string.h>     // strcat
#include <unistd.h>     // nanosleep
#include <fcntl.h>      // O_WRONLY
#include <sys/ioctl.h>  // ioctl
#include <linux/i2c-dev.h> // I2C_SLAVE


#define MASTER_PING_GPIO "4"

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define MASTER_PING_GPIO_DIR  "/sys/class/gpio/gpio" MASTER_PING_GPIO "/direction"
#define MASTER_PING_GPIO_VALUE  "/sys/class/gpio/gpio" MASTER_PING_GPIO "/value"



#define I2C_ADDR_ARDUINO 0x09
#define I2C_ADDR_SERVO 0x40

#define MASTERPING_LOOPWAIT 300
#define CHANNELROUTER_LOOPWAIT 100

/**
 * @brief Construct a new Channels Service:: Channels Service object
 * 
 */
ChannelsService::ChannelsService()
    : ThreadedService(MASTERPING_LOOPWAIT),
    m_GpioFileDescriptor(0),
    mb_PingHigh(false)
{

}

/**
 * @brief Query last known channel values.
 * 
 * @param channels 
 */
void ChannelsService::GetChannels(short channels[CHANNELS])
{
    mChannelRouter.GetChannels(channels);
}

/**
 * @brief Before allowing the service to start,
 *          exports the master ping gpio and
 *          configures it as out -pin.
 * 
 * @return true if configuring succesful
 * @return false otherwise
 */
bool ChannelsService::PreStart()
{

	int fd = open(GPIO_EXPORT, O_WRONLY);
	if (!fd)
		return false;
	
	write(fd, MASTER_PING_GPIO, 1);
	close(fd);
	
	fd = open(MASTER_PING_GPIO_DIR, O_WRONLY);
	if(!fd)
	    return false;
	
	write(fd, "out", 3);
	close(fd);

	return true;
}

/**
 * @brief After service has started successfully,
 *          waits for 500ms and then starts the
 *          channel router service.
 * 
 * @return true if successful
 * @return false otherwise
 */
bool ChannelsService::PostStart()
{ 
    // Give arduino 500ms to react to slave select.
    struct timespec tsr2, ts2 = {0, 500000000L}; 
    nanosleep(&ts2, &tsr2);

    if(!mChannelRouter.StartService())
        return false;

    return true;
}

/**
 * @brief Before stopping this service,
 *          stops the channel router 
 *          service first.
 * 
 */
void ChannelsService::PreStop()
{
    mChannelRouter.StopService();
}

/**
 * @brief Before pinging the master ping line,
 *          open the GPIO device for writing.
 * 
 * @return true if successful
 * @return false otherwise
 */
bool ChannelsService::PreServiceLoop()
{
    mb_PingHigh = true;
    m_GpioFileDescriptor = open(MASTER_PING_GPIO_VALUE, O_WRONLY);

    if (!m_GpioFileDescriptor)
        return false;

    return true;
}

/**
 * @brief Set master pin to high/low every
 *          second call.
 * 
 * @return true if successful
 * @return false otherwise
 */
bool ChannelsService::ServiceLoopTick()
{
    if (!m_GpioFileDescriptor)
        return false;
   
    if (mb_PingHigh)
        write(m_GpioFileDescriptor, "1", 1);
    else
        write(m_GpioFileDescriptor, "0", 1);

    mb_PingHigh = !mb_PingHigh;

	return true;
}

/**
 * @brief Closes the master pin device before 
 *          exiting the thread.
 * 
 */
void ChannelsService::PostServiceLoop()
{
    if (m_GpioFileDescriptor)
        close(m_GpioFileDescriptor);
    m_GpioFileDescriptor = 0;
}

/**
 * @brief Unexports the master ping pin
 *          after the service has been stopped.
 * 
 */
void ChannelsService::PostStop()
{

	int fd = open(GPIO_UNEXPORT, O_WRONLY);
	if (fd)
	{	
	    write(fd, MASTER_PING_GPIO, 1);
	    close(fd);
    }
}

/**
 * @brief Construct a new Channels Service:: Channel Router:: Channel Router object
 * 
 */
ChannelsService::ChannelRouter::ChannelRouter()
    : ThreadedService(CHANNELROUTER_LOOPWAIT)
{

}

/**
 * @brief Query the last known channel values.
 * 
 * @param channels 
 */
void ChannelsService::ChannelRouter::GetChannels(short channels[CHANNELS])
{
    for (size_t i = 0; i < CHANNELS; i++)
        channels[i] = m_Channels[i]; 
}

/**
 * @brief Ensure the I2C can be opened or is open already. 
 * 
 * @return true if successful
 * @return false otherwise
 */
bool ChannelsService::ChannelRouter::PreServiceLoop()
{
    return I2CBus::getInstance().Open() || I2CBus::getInstance().IsOpen();
}


/**
 * @brief Service loop that reads the PWM values from arduino
 *          and re-routes them to the servo driver, while
 *          caching the values at the same time.
 * 
 * @return true if successful
 * @return false otherwise
 */
bool ChannelsService::ChannelRouter::ServiceLoopTick()
{
    char buffer[CHANNELS];

    I2CCALL getBuffer = [&buffer](int fd){
    
        if (ioctl(fd, I2C_SLAVE, I2C_ADDR_ARDUINO) < 0)
            return false;
                
        read(fd, buffer, CHANNELS);
        return true;
    };

    if (!I2CBus::getInstance().Call(getBuffer))
        return false;

    char pwm[5];

    I2CCALL setPWM = [&pwm](int fd){
    
        if (ioctl(fd, I2C_SLAVE, I2C_ADDR_ARDUINO) < 0)
            return false;
                
        write(fd, pwm, 5);
        return true;
    };    

    for (int i = 0; i < CHANNELS; i++)
    {
        short val = (short)buffer[i];// == 0 ? 0 : map(buffer[i], 1, 0xFF, 900, 2100);
        m_Channels[i] = val;

        // TODO ......

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

        if(!I2CBus::getInstance().Call(setPWM))
            return false;
      
    }

    return true;
}

