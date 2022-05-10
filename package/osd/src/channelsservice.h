#ifndef CHANNELSSERVICE_H_
#define CHANNELSSERVICE_H_

#include "threadedservice.h"

#define CHANNELS 6

/**
 * @brief A service pinging with master ping line,
 *          querying PWM values from arduino,
 *          and routing the PWM values to the 
 *          servo driver.
 * 
 */
class ChannelsService : public ThreadedService
{
public:
    ChannelsService();
    
    void GetChannels(short channels[CHANNELS]);

protected:
    virtual bool PreStart() override;
    virtual bool PostStart() override;
    
    virtual bool PreServiceLoop() override;
    virtual bool ServiceLoopTick() override;
    virtual void PostServiceLoop() override;

    virtual void PreStop() override;
    virtual void PostStop() override;

private:
    
    /**
     * @brief Internal class that handles the I2C
     *          traffic routing and PWM capturing.
     * 
     */
    class ChannelRouter : public ThreadedService
    {

        public:
            ChannelRouter();
            void GetChannels(short channels[CHANNELS]);

        protected:
            virtual bool PreServiceLoop() override;
            virtual bool ServiceLoopTick() override;
            
        private:
            short m_Channels[CHANNELS];
    };

private:
    ChannelRouter mChannelRouter;
    int m_GpioFileDescriptor;
    bool mb_PingHigh;
};

#endif