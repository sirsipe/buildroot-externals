#ifndef GPSSERVICE_H_
#define GPSSERVICE_H_

#include "threadedservice.h"
#include <gps.h>

/**
 * @brief A service polling GPS data and holding cache of
 *            last known values.
 * 
 */
class GPSService : public ThreadedService
{
public:
    GPSService();

    /**
     * @brief Enumeration of GPS fix status.
     * 
     */
    enum FixStatus
    {
        NotAvailable = 0,
        None = 1,
        TwoD = 2,
        ThreeD = 3
    };

    enum GPSService::FixStatus GetFixStatus() const;
    long Timestamp() const;
    float Lat() const;
    float Lon() const;
    
protected:
    virtual bool PreStart() override;
    virtual void PostStop() override;
    
    virtual bool PreServiceLoop() override;
    virtual bool ServiceLoopTick() override;
    virtual void PostServiceLoop() override;
private:
    struct gps_data_t m_gpsData;

    FixStatus me_Fixstatus;
    long m_Timestamp;
    float m_Latitude;
    float m_Longitude;

};

#endif