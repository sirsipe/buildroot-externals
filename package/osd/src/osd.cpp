#include "raylib.h"
#include <stdio.h>
#include "channelsservice.h"
#include "gpsservice.h"
#include "rotaccelservice.h"

void DT(char* text, int x, int y, int size)
{
    DrawText(text, x, y, size, WHITE);
    DrawText(text, x+1, y+1, size, BLACK);
}

int main(void)
{

    InitWindow(800, 450, "raylib [core] example - basic window");
    ChannelsService channels;
    GPSService gps;
    RotAccelService rotAcc;

    channels.StartService();
    gps.StartService();
    rotAcc.StartService();

    while (!WindowShouldClose())
    {

        short chs[CHANNELS];
        channels.GetChannels(chs);

        BeginDrawing();
        ClearBackground(BLANK);

        char str[100] = {0};
    	sprintf(str, "GPS Fix: %d", (int)gps.GetFixStatus());
        DT(str, 190, 50, 20);
        
        short rX, rY, rZ;
        rotAcc.GetRotation(rX, rY, rZ);
        sprintf(str, "Rot: %d, %d, %d", (int)rX, (int)rY, (int)rZ);
        DT(str, 190, 100, 20);
        
        rotAcc.GetAcceleration(rX, rY, rZ);
        sprintf(str, "Acc: %d, %d, %d", (int)rX, (int)rY, (int)rZ);
        DT(str, 190, 125, 20);
        
        sprintf(str, "Temp: %d", (int)rotAcc.GetTemp());
        DT(str, 190, 150, 20);

        for(int i = 0; i < CHANNELS; i++)
        {
	    	sprintf(str, "CH %d: %d", (i+1), (int)chs[i]);
            DT(str, 190, 200 + (25*i), 20);
        }


        EndDrawing();
    }

    CloseWindow();

    channels.StopService();
    gps.StopService();
    rotAcc.StopService();
    return 0;
}
