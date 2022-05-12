#ifndef ROTACCELSERVICE_H_
#define ROTACCELSERVICE_H_

#include "threadedservice.h"
#include <gps.h>

/* ---- MPU9250 registers ----*/
// All needed data is in 14 bytes starting from
#define MPU9250_DATA_BEGIN              0x3B
/*
#define MPU9250_ACCEL_XOUT_H            0x3B
#define MPU9250_ACCEL_XOUT_L            0x3C
#define MPU9250_ACCEL_YOUT_H            0x3D
#define MPU9250_ACCEL_YOUT_L            0x3E
#define MPU9250_ACCEL_ZOUT_H            0x3F
#define MPU9250_ACCEL_ZOUT_L            0x40
#define MPU9250_TEMP_OUT_H              0x41
#define MPU9250_TEMP_OUT_L              0x42
#define MPU9250_GYRO_XOUT_H             0x43
#define MPU9250_GYRO_XOUT_L             0x44
#define MPU9250_GYRO_YOUT_H             0x45
#define MPU9250_GYRO_YOUT_L             0x46
#define MPU9250_GYRO_ZOUT_H             0x47
#define MPU9250_GYRO_ZOUT_L             0x48
*/

/**
 * @brief A service polling Rotation and Acceleration components
 *          from MPU9250 and holding cache of last known values.
 *            
 */
class RotAccelService : public ThreadedService
{
public:
    RotAccelService();

    void GetRotation(short& rX, short& rY, short& rZ) const;
    void GetAcceleration(short& rX, short& rY, short& rZ) const;
    short GetTemp() const;

protected:
    virtual bool PreServiceLoop() override;
    virtual bool ServiceLoopTick() override;
private:
    short m_RotX, m_RotY, m_RotZ;
    short m_AccX, m_AccY, m_AccZ;
    short m_Temp;
};

#endif