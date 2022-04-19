#include "raylib.h"
#include <stdio.h>
#include "channelsservice.h"
#include "gpsservice.h"


int main(void)
{

    InitWindow(800, 450, "raylib [core] example - basic window");
    ChannelsService channels;
    GPSService gps;

    channels.StartService();
    gps.StartService();

    while (!WindowShouldClose())
    {

        short chs[CHANNELS];
        channels.GetChannels(chs);

        BeginDrawing();
        ClearBackground(BLANK);

        char gpsstr[100] = {0};
		sprintf(gpsstr, "GPS Fix: %d", (int)gps.GetFixStatus());
        DrawText(gpsstr, 190, 150, 20, WHITE);
        DrawText(gpsstr, 191, 151, 20, BLACK);

        for(int i = 0; i < CHANNELS; i++)
        {
	    	char str[100] = {0};
		    sprintf(str, "CH %d: %d", (i+1), (int)chs[i]);
            DrawText(str, 190, 200 + (25*i), 20, WHITE);
           	DrawText(str, 191, 201 + (25*i), 20, BLACK);
            
        }


        EndDrawing();
    }

    CloseWindow();

    channels.StopService();
    gps.StopService();
    
    return 0;
}
