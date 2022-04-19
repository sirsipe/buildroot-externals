#include "gpsservice.h"
#include <math.h> // isfinite

/**
 * @brief Construct a new GPSService::GPSService object
 * 
 */
GPSService::GPSService() 
    : ThreadedService(500),
    me_Fixstatus(FixStatus::NotAvailable),
    m_Timestamp(0L),
    m_Latitude(0.0f),
    m_Longitude(0.0f)
{

}

/**
 * @brief Getter for GPS fix status
 * 
 * @return enum GPSService::FixStatus 
 */
enum GPSService::FixStatus GPSService::GetFixStatus() const
{
    return me_Fixstatus;
}

/**
 * @brief Getter for GPS timestamp
 * 
 * @return long non-zero if GPS time
 *          has been obtained; corresponding
 *          to seconds. 0 if time not obtained.
 */
long GPSService::Timestamp() const
{
    return m_Timestamp;
}

/**
 * @brief Last known latitude
 * 
 * @return float 
 */
float GPSService::Lat() const
{
    return m_Latitude;
}

/**
 * @brief Last known longitude
 * 
 * @return float 
 */
float GPSService::Lon() const
{
    return m_Longitude;
}

/**
 * @brief Opens the local GPS stream and prepares the gps data.
 * 
 * @return true if successful
 * @return false otherwise
 */
bool GPSService::PreStart()
{
    if(gps_open("127.0.0.1", "2947", &m_gpsData) != 0)
        return false;
    
    return true;
}

/**
 * @brief Closes the GPS data
 * 
 */
void GPSService::PostStop()
{
    gps_close(&m_gpsData);
}

/**
 * @brief Enables the gpsstream to print its data.
 * 
 * @return true always
 * @return false never
 */
bool GPSService::PreServiceLoop()
{
    gps_stream(&m_gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
    return true;
}

/**
 * @brief Polls next values from GPS stream and caches them accordingly.
 * 
 * @return true if reads as expected
 * @return false if read error.
 */
bool GPSService::ServiceLoopTick()
{
    if(gps_waiting(&m_gpsData, 5000000))
	{
		if (gps_read(&m_gpsData, NULL, 0) == -1)
            return false;

		if (MODE_SET != (MODE_SET & m_gpsData.set))
			return true;
		
		if (m_gpsData.fix.mode < 0 || m_gpsData.fix.mode > 3)
			m_gpsData.fix.mode = 0;
		
        me_Fixstatus = static_cast<FixStatus>(m_gpsData.fix.mode);

		if (TIME_SET == (TIME_SET & m_gpsData.set))
		    m_Timestamp = m_gpsData.fix.time.tv_sec;

		if (isfinite(m_gpsData.fix.latitude) && isfinite(m_gpsData.fix.longitude))
		{
            m_Latitude = m_gpsData.fix.latitude;
            m_Longitude = m_gpsData.fix.longitude;
		}

	}

    return true;
}

/**
 * @brief Disables the gpsstream.
 * 
 */
void GPSService::PostServiceLoop()
{
    gps_stream(&m_gpsData, WATCH_DISABLE, NULL);
}