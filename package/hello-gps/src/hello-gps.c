#include <gps.h>
#include <math.h>
//#include <unistd.h>

#define MODE_STR_NUM 4
static char* mode_str[MODE_STR_NUM] = {
	"n/a",
	"None",
	"2D",
	"3D"
};



int main(void) {

	struct gps_data_t gps_data;

	if(0 != gps_open("127.0.0.1", "2947", &gps_data))
	{
		printf("Open error. Bye, bye\n");
		return 1;
	}

	(void) gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

	while(gps_waiting(&gps_data, 5000000))
	{
		if (-1 == gps_read(&gps_data, NULL, 0))
		{
			printf("Read error. Bye, bye\n");
			break;
		}

		if (MODE_SET != (MODE_SET & gps_data.set))
		{
			continue;
		}


		if (0 > gps_data.fix.mode || MODE_STR_NUM <= gps_data.fix.mode)
		{
			gps_data.fix.mode = 0;
		}

		printf("Fix mode: %s (%d) Time: ", mode_str[gps_data.fix.mode], gps_data.fix.mode);
		
		if (TIME_SET == (TIME_SET & gps_data.set))
		{
			printf("%ld.%09ld ", gps_data.fix.time.tv_sec, gps_data.fix.time.tv_nsec);
		}
		else
		{
			puts("n/a ");
		}

		if (isfinite(gps_data.fix.latitude) && isfinite(gps_data.fix.longitude))
		{
			printf("Lat %.6f Lon %.6f\n", gps_data.fix.latitude, gps_data.fix.longitude);
		}

	}

	/* When you are done... */
	(void) gps_stream(&gps_data, WATCH_DISABLE, NULL);
	(void) gps_close(&gps_data);

	return 0;
}
