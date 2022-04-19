#ifndef THREADEDSERVICE_H_
#define THREADEDSERVICE_H_

#include <pthread.h>
#include <unistd.h>     // nanosleep

/**
 * @brief An abstract base class providing standardized
 *          methods to start a service loop in its own
 *          thread.
 * 
 */
class ThreadedService
{
public:

    /**
     * @brief Status enumeration of possible service states.
     * 
     */
    enum eServiceStatus
    {
        eServiceStatusStopped,
        eServiceStatusStarting,
        eServiceStatusRunning ,
        eServiceStatusStopping,
        eServiceStatusError,
    };

    ThreadedService();
    ThreadedService(long delayInMilliseconds);
    ~ThreadedService();

    enum ThreadedService::eServiceStatus GetServiceStatus() const;

    bool StartService();
    void StopService();

protected:

    // PreServiceLoop, ServiceLoopTick and PostServiceLoop
    // are invoke int the same worker thread.
    virtual bool PreServiceLoop() { return true; };
    virtual bool ServiceLoopTick() = 0;
    virtual void PostServiceLoop() { };
    
    virtual bool PreStart() { return true; };
    virtual bool PostStart() { return true; };
    virtual void PreStop() { };
    virtual void PostStop() { };
    
private:
    static void* Spawn(void* obj);

    void SetServiceStatus(enum ThreadedService::eServiceStatus newStatus);
    void ServiceLoop();

    void StatusString(char str[64], enum ThreadedService::eServiceStatus status);

private:
    struct timespec m_LoopDelay;
    pthread_t m_Thread;
    eServiceStatus m_Status;
};


#endif