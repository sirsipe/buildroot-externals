#include "rotaccelservice.h"
#include "i2cbus.h"
#include <unistd.h>        // read/write
#include <sys/ioctl.h>     // ioctl
#include <linux/i2c-dev.h> // I2C_SLAVE

#define MPU9250_I2C_ADDR 0x68
#define ROTACCELSERVICE_LOOPWAIT 50

RotAccelService::RotAccelService() :
    ThreadedService(ROTACCELSERVICE_LOOPWAIT),
    m_RotX(0), m_RotY(0), m_RotZ(0),
    m_AccX(0), m_AccY(0), m_AccZ(0),
    m_Temp(0)
{
    
}

void RotAccelService::GetRotation(short& rX, short& rY, short& rZ) const
{
    rX = m_RotX;
    rY = m_RotY;
    rZ = m_RotZ;
}

void RotAccelService::GetAcceleration(short& rX, short& rY, short& rZ) const
{
    rX = m_AccX;
    rY = m_AccY;
    rZ = m_AccZ;
}

short RotAccelService::GetTemp() const
{
    return m_Temp;
}

bool RotAccelService::PreServiceLoop() 
{
    return I2CBus::getInstance().Open() || I2CBus::getInstance().IsOpen();
}

bool RotAccelService::ServiceLoopTick()
{
    char buffer[14];
    I2CCALL getAllData = [&buffer](int fd){
    
        if (ioctl(fd, I2C_SLAVE, MPU9250_I2C_ADDR) < 0)
            return false;
        
        buffer[0] = MPU9250_DATA_BEGIN;

        write(fd, buffer, 1);
        read(fd, buffer, 14);
        return true;
    };

    if (!I2CBus::getInstance().Call(getAllData))
        return false;
    
    //MPU9250_ACCEL_XOUT
    m_AccX = (((short)buffer[0]) << 8) | buffer[1];
    //MPU9250_ACCEL_YOUT
    m_AccY = (((short)buffer[2]) << 8) | buffer[3];
    //MPU9250_ACCEL_ZOUT
    m_AccZ = (((short)buffer[4]) << 8) | buffer[5];

    //MPU9250_TEMP_OUT
    m_Temp = (((short)buffer[6]) << 8) | buffer[7];

    //MPU9250_GYRO_XOUT
    m_RotX = (((short)buffer[8]) << 8) | buffer[9];
    //MPU9250_GYRO_YOUT
    m_RotY = (((short)buffer[10]) << 8) | buffer[11];
    //MPU9250_GYRO_ZOUT
    m_RotZ = (((short)buffer[12]) << 8) | buffer[12];

    return true;
}