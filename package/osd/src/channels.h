#ifndef CHANNELS_H_
#define CHANNELS_H_

#include <pthread.h>

#define CHANNELS 6

class Channels
{
public:
    enum eMasterPingStatus
    {
        eMasterPingStatusStopped,
        eMasterPingStatusStarting,
        eMasterPingStatusRunning,
        eMasterPingStatusStopping,
        eMasterPingStatusError,
    };

    static Channels::eMasterPingStatus GetStatus();

    static bool Takeover();
    static void Release();

    static void GetChannels(short channels[CHANNELS]);

private:
    enum eMasterPingFileType
    {   
	    eMasterPingFileTypeDirection,
	    eMasterPingFileTypeValue
    };
 
    static void SetStatus(Channels::eMasterPingStatus newStatus);

    static void GetMasterPingFile(char* buffer, enum eMasterPingFileType type);
    static bool SetupMasterPing();

    static void StartChannelRouting();
    static void StopChannelRouting();

    static void* MasterPingProc(void*);
    static void* ChannelRoutingProc(void*);

private:
    static Channels::eMasterPingStatus z_MasterPingStatus;
    static pthread_t* zp_MasterPingThread;
    static pthread_t* zp_ChannelRoutingThread;
    static bool zb_ChannelRouting;
    static short z_Channels[CHANNELS];


};

#endif