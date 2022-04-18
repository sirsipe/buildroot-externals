#include "raylib.h"
//#include <gps.h>
//#include <math.h>
#include <stdio.h>
//#include <errno.h>
//#include <unistd.h>
#include "channels.h"


/*
#define MODE_STR_NUM 4

static char* mode_str[MODE_STR_NUM] = {
	"n/a",
	"None",
	"2D",
	"3D"
};
*/

int main(void)
{

    
    

    InitWindow(800, 450, "raylib [core] example - basic window");
    Channels::Takeover();
    while (!WindowShouldClose())
    {

        short chs[CHANNELS];
        Channels::GetChannels(chs);

        BeginDrawing();
        ClearBackground(BLANK);
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

    Channels::Release();

    return 0;
}
