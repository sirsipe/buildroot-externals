#include "threadedservice.h"


/**
 * @brief Construct a new Threaded Service:: Threaded Service object
 * 
 * Delay of the loop is initialized to 1s.  
 */
ThreadedService::ThreadedService() :
    ThreadedService::ThreadedService(1000)
{

}

/**
 * @brief Construct a new Threaded Service:: Threaded Service object
 * 
 * @param delayInMilliseconds delay of the loop in milliseconds
 */
ThreadedService::ThreadedService(long delayInMilliseconds) :
    m_Thread(0),
    m_Status(eServiceStatusStopped)
{
    int seconds = (int)(delayInMilliseconds / 1000L);
    long nanoseconds = (delayInMilliseconds - (seconds * 1000L)) * 1000L * 1000L;
    m_LoopDelay = {seconds, nanoseconds};
}

/**
 * @brief Destroy the Threaded Service:: Threaded Service object
 *          Stops the service if it's running.
 */
ThreadedService::~ThreadedService()
{
    StopService();
}

/**
 * @brief Service status getter
 * 
 * @return eServiceStatus 
 */
enum ThreadedService::eServiceStatus ThreadedService::GetServiceStatus() const
{
    return m_Status;
}

/**
 * @brief Private service status setter.
 * 
 * @param newStatus New status to set.
 */
void ThreadedService::SetServiceStatus(enum ThreadedService::eServiceStatus newStatus)
{
    m_Status = newStatus;
}

/**
 * @brief Starts this service if its current status is
 *          stopped. 
 * 
 * @return true if service started and running successfully
 * @return false if service start failed
 */
bool ThreadedService::StartService()
{

    if (GetServiceStatus() != eServiceStatusStopped)
        return false;

    if(!PreStart())
        return false;

    SetServiceStatus(eServiceStatusStarting);

    pthread_create(&m_Thread, NULL, *(ThreadedService::Spawn), this);

    while(GetServiceStatus() == eServiceStatusStarting)
    {
        struct timespec tsr, ts = {0, 10000000L}; //10ms
		nanosleep(&ts, &tsr);
    }

    if (GetServiceStatus() == eServiceStatusError || !PostStart())
    {
        StopService();
        return false;
    }

    return true;
}

/**
 * @brief Stop the currently running service, or cleanup after
 *          service has stopped with error.
 * 
 */
void ThreadedService::StopService()
{
    if (GetServiceStatus() == eServiceStatusRunning || GetServiceStatus() == eServiceStatusError)
    {
        PreStop();

        SetServiceStatus(eServiceStatusStopping);
        pthread_join(m_Thread, NULL);
        m_Thread = 0;
        SetServiceStatus(eServiceStatusStopped);

        PostStop();
    }
}

/**
 * @brief A private static function router used to invoke the
 *          class member function loop for the thread.
 * 
 * @param thisPtr Pointer to ThreadedService object that should start the service loop. 
 * @return void* nullptr
 */
void* ThreadedService::Spawn(void* thisPtr)
{
    ((ThreadedService*)thisPtr)->ServiceLoop();
    return nullptr;
}

/**
 * @brief Private service loop that handles service statuses 
 *          and calls the user implementations.
 * 
 */
void ThreadedService::ServiceLoop()
{
    if (!PreServiceLoop())
        SetServiceStatus(eServiceStatusError);

    SetServiceStatus(eServiceStatusRunning);
    
    while(GetServiceStatus() == eServiceStatusRunning)
    {
        if(!ServiceLoopTick())
            SetServiceStatus(eServiceStatusError);
        
        struct timespec tsr;
		if(nanosleep(&m_LoopDelay, &tsr) < 0)
			SetServiceStatus(eServiceStatusError);
    }

    PostServiceLoop();
    
}