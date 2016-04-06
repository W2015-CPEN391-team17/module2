/*
 * GPS Data Logger
 * This contains functions necessary to send commands to the GPS to
 * start logging, stop logging, log an instantaneous moment, and more
 * importantly, extract the data from the log to get latitude and
 * longitude coordinates.
 *
 * Author: Timothy Choy
 * Last updated: Feb 23, 2016
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gps_points.h"
#include "graphics.h"
#include "conversion.h"
#include "datasets.h"

// from datasets.c
extern localDataSets localData;

struct logs gps_log[50];
struct points gps_points[70];
struct points gps_realtime;

/*
 * The GPS must be initialized before the user can read or write.
 * Call this function at the beginning before use.
 */
void init_gps(void)
{
	//set up 6850 control register to utilize a divide by 16 clock,
	//set RTS low, use 8 bits of data, no parity, 1 stop bit
	//transmitter interrupt disabled
	//program baud rate generator to use 9.6k baud
	GPS_CONTROL = 0x15;
	GPS_BAUD = 0x5;
}

/*
 * putchar_gps writes 1 byte of data from the serial port.
 */
char putchar_gps(char c)
{
	//poll Tx bit in 6850 status register. Wait for it to become '1'
	//write 'c' to the 6850 TxData register to output the character
	while(1)
	{
		if (GPS_STATUS & GPS_STATUS_TX_MASK)
		{
			GPS_TXDATA = c;
			return c;
		}
	}

	return c;
}

/*
 * getchar_gps reads 1 byte of data from the serial port.
 */
char getchar_gps(void){
	//poll Rx bit in 6850 status register. Wait for it to become '1'
	//read received character from 6850 register

	while(1)
	{
		if (GPS_STATUS & GPS_STATUS_RX_MASK)
		{
			return GPS_RXDATA;
		}
	}
	return '\0';
}

/*
 * Swaps endian after extracting latitude and longitude from GPS log.
 */
int swapEndian(char *s){
	register int val;

	val = strtoul(s, NULL, 16);
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
	val = (val << 16) | ((val >> 16) & 0xFFFF);

	return val;
}

/*
 * converts latitude from the swapped endian int to float.
 */
float FloatToLatitudeConversion(int x){

	float *ptr = (float *)(&x); // cast int to float
	float f = *ptr; // get the float

	return f;
}

/*
 * converts longitude from the swapped endian int to float.
 */
float FloatToLongitudeConversion(int x){

	float *ptr = (float *)(&x);
	float f = *ptr;

	return f;
}

/*
 * reads exactly one string from $ to end line from the log.
 */
void read_string(char *output){

//	printf("Reading string\n");
	memset(output, 0, 256);

	int i = 0;
	char c;
	int done = 0;

	while(done == 0){
		c = getchar_gps();

		if(c == '$'){
			while (c != '\n'){
				output[i] = c;
				c = getchar_gps();
				i++;
			}
			output[i] = '\0';
			//printf("%s\n", output);
			done = 1;
		}
	}
	return;
}

/*
 * Saves points as type float to an array gps_points to be converted
 * to screen points.
 */
void save_points(void){

	printf("Initiating dump\n");
	int i, lat_count, lat_end, long_count, long_end, save_count;
	int j = 200;
	int k = 0;
	int n = 23;
	int s = 0;
	int x = XRES/4;
	int y = 5*YRES/6;
	int r = 10;
	int count, log_count = 0;
	int place = 0;
	const char command[] = "$PMTK622,1*29\r\n";
	int length = strlen(command);
	char cur_string[256] = {0};

	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}

	while(gps_log[0].string[7] != 'X'){
		read_string(gps_log[0].string);
	}

	read_string(gps_log[0].string); // to dump the first unused log

	while(gps_log[0].string[11] == '0'){
		read_string(gps_log[0].string);
	}

	for(log_count = 1; log_count < 20; log_count++){
		read_string(gps_log[log_count].string);

		// draws circles while extracting logs from GPS (for UI)
		j = (n + j + 1) % 500;
		n = (n + k*j) % 350;
		k = (k + j + n + r) % 50;
		x += (XRES-(XRES/2))/20;
		WriteCircle((int)x, (int)y, (int)r, LIME);
	}

	// for first 9 logs, lat_count, long_count, lat_end, and long_end
	// are all 1 character shorter than 10 logs
	for(log_count = 0; log_count < 9; log_count++){

		strcpy(cur_string, (const char *)gps_log[log_count].string);

		lat_count = 24;
		long_count = 33;
		lat_end = 32;
		long_end = 41;

		for(count = 0; count < 6; count++){
			save_count = 0;
			while(lat_count <= lat_end){
				if(cur_string[lat_count] != ','){
					gps_points[place].latitude[save_count] = cur_string[lat_count];
					lat_count++;
					save_count++;
				}
				else
					lat_count++;
			}
			save_count = 0;
			while(long_count <= long_end){
				if(cur_string[long_count] != ','){
					gps_points[place].longitude[save_count] = cur_string[long_count];
					long_count++;
					save_count++;
				}
				else
					long_count++;
			}
			gps_points[place].long_swapped = swapEndian(gps_points[place].longitude);
			gps_points[place].lat_swapped = swapEndian(gps_points[place].latitude);

			gps_points[place].long_float = FloatToLongitudeConversion(gps_points[place].long_swapped);
			gps_points[place].lat_float = FloatToLatitudeConversion(gps_points[place].lat_swapped);

			place++;
			lat_count += 27;
			long_count += 27;
			lat_end += 36;
			long_end += 36;
		}
	}

	// this loop accounts for the next 10 logs off-by-one error
	for(log_count = 9; log_count < 12; log_count++){
		if(place == 70)
			break;

		strcpy(cur_string, (const char *)gps_log[log_count].string);

		lat_count = 25;
		long_count = 34;
		lat_end = 33;
		long_end = 42;

		for(count = 0; count < 6; count++){
			save_count = 0;
			while(lat_count <= lat_end){
				if(cur_string[lat_count] != ','){
					gps_points[place].latitude[save_count] = cur_string[lat_count];
					lat_count++;
					save_count++;
				}
				else
					lat_count++;
			}
			save_count = 0;
			while(long_count <= long_end){
				if(cur_string[long_count] != ','){
					gps_points[place].longitude[save_count] = cur_string[long_count];
					long_count++;
					save_count++;
				}
				else
					long_count++;
			}
			gps_points[place].long_swapped = swapEndian(gps_points[place].longitude);
			gps_points[place].lat_swapped = swapEndian(gps_points[place].latitude);

			gps_points[place].long_float = FloatToLongitudeConversion(gps_points[place].long_swapped);
			gps_points[place].lat_float = FloatToLatitudeConversion(gps_points[place].lat_swapped);

			place++;
			lat_count += 27;
			long_count += 27;
			lat_end += 36;
			long_end += 36;
		}
	}

	return;

}

/*
 * log_now will log a point at that instant. Ensure you sleep 3 seconds before
 * calling log_now another time.
 */
void log_now(void){
	int i;
	const char command[] = "$PMTK186,1*20\r\n";
	int length = strlen(command);
	char string[256] = {0};

	printf("Logging...\n");
	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}
}

/*
 * starts logging.
 */
void start_log(void){
	int i;
	const char command[] = "$PMTK185,0*22\r\n";
	int length = strlen(command);

	printf("Starting log...\n");
	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}
}

/*
 * stops logging.
 */
void stop_log(void){
	int i;
	const char command[] = "$PMTK185,1*23\r\n";
	int length = strlen(command);

	printf("Stopping log...\n");
	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}
}

/*
 * erases the log.
 */
void erase_log(void){
	int i;
	const char command[] = "$PMTK184,1*22\r\n";
	int length = strlen(command);

	printf("Erasing log...\n");
	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}
}

/*
 * Queries the log for info on mode and interval of time logging and more.
 */
void query_log(void){
	int i;
	const char command[] = "$PMTK183*38\r\n";
	char out[100];
	int length = strlen(command);

	printf("Query...\n");
	// here we send the command to the gps
	for(i = 0; i < length; i++){
		putchar_gps(command[i]);
	}

	read_string(out);
	while(out[0] != 'P' && out[15] != 'A'){
		read_string(out);
	}
	printf("%s", out);
}

void save_demo_points(int set) {
	int size = 100;
	int i = 0;
	int xval = 10;
	int yval = 4;

	localData.dataSets[set].size = size;

	if( set == 0 ){
		for( i = 0; i < 39; i++ ){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 10;
			yval += 10;
		}

		for( i = 39; i < 48; i++ ){
			xval += 35;

			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}

		for( i = 48; i < 60; i++ ){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}

		for( i = 60; i < 70; i++ ){
			xval -= 15;
			yval -= 20;

			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}

		for( i = 70; i < 79; i++ ){
			yval -= 20;

			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}

		for( i = 79; i < 100; i++ ){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}
	}
	else if( set == 1 ){
		for( i = 0; i < 30; i++){
			localData.dataSets[set].points[i].x = 4;
			localData.dataSets[set].points[i].y = 4;
		}

		for( i = 30; i < 45; i++){
			localData.dataSets[set].points[i].x = 4;
			localData.dataSets[set].points[i].y = 75;
		}

		for( i = 45; i < 60; i++){
			localData.dataSets[set].points[i].x = 81;
			localData.dataSets[set].points[i].y = 4;
		}

		for( i = 60; i < 70; i++){
			localData.dataSets[set].points[i].x = 81;
			localData.dataSets[set].points[i].y = 75;
		}

		for( i = 70; i < 77; i++){
			localData.dataSets[set].points[i].x = 162;
			localData.dataSets[set].points[i].y = 4;
		}

		for( i = 77; i < 85; i++){
			localData.dataSets[set].points[i].x = 4;
			localData.dataSets[set].points[i].y = 150;
		}

		for( i = 85; i < 90; i++){
			localData.dataSets[set].points[i].x = 243;
			localData.dataSets[set].points[i].y = 4;
		}

		for( i = 90; i < 93; i++){
			localData.dataSets[set].points[i].x = 162;
			localData.dataSets[set].points[i].y = 75;
		}

		for( i = 93; i < 95; i++){
			localData.dataSets[set].points[i].x = 81;
			localData.dataSets[set].points[i].y = 150;
		}

		for( i = 95; i < 97; i++){
			localData.dataSets[set].points[i].x = 4;
			localData.dataSets[set].points[i].y = 225;
		}

		for( i = 97; i < 100; i++){
			localData.dataSets[set].points[i].x = 4;
			localData.dataSets[set].points[i].y = 300;
		}
	}
	else if( set == 2 ){
		for( i = 0; i < 100; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
		}
	}
	else if( set == 3 ){
		for( i = 0; i < 100; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
			yval += 4;
		}
	}
	else if( set == 4 ){
		for( i = 0; i < 50; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
			yval += 8;
		}
		for( i = 50; i < 100; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
			yval -= 8;
		}
	}
	else if( set == 5 ){
		for( i = 0; i < 33; i++){
			localData.dataSets[set].points[i].x = 150;
			localData.dataSets[set].points[i].y = 150;
		}

		for( i = 33; i < 66; i++){
			localData.dataSets[set].points[i].x = 450;
			localData.dataSets[set].points[i].y = 250;
		}

		for( i = 66; i < 100; i++){
			localData.dataSets[set].points[i].x = 700;
			localData.dataSets[set].points[i].y = 420;
		}
	}
	else if( set == 6 ){
		for( i = 0; i < 33; i++){
			localData.dataSets[set].points[i].x = 1;
			localData.dataSets[set].points[i].y = 1;
		}

		for( i = 33; i < 66; i++){
			localData.dataSets[set].points[i].x = 80;
			localData.dataSets[set].points[i].y = 80;
		}

		for( i = 66; i < 100; i++){
			localData.dataSets[set].points[i].x = 160;
			localData.dataSets[set].points[i].y = 160;
		}
	}
	else if( set == 7 ){
		for( i = 0; i < 33; i++){
			localData.dataSets[set].points[i].x = 700;
			localData.dataSets[set].points[i].y = 150;
		}

		for( i = 33; i < 66; i++){
			localData.dataSets[set].points[i].x = 600;
			localData.dataSets[set].points[i].y = 150;
		}

		for( i = 66; i < 100; i++){
			localData.dataSets[set].points[i].x = 500;
			localData.dataSets[set].points[i].y = 150;
		}
	}
	else if( set == 8 ){
		for( i = 0; i < 33; i++){
			localData.dataSets[set].points[i].x = 700;
			localData.dataSets[set].points[i].y = 420;
		}

		for( i = 33; i < 66; i++){
			localData.dataSets[set].points[i].x = 420;
			localData.dataSets[set].points[i].y = 220;
		}

		for( i = 66; i < 100; i++){
			localData.dataSets[set].points[i].x = 1;
			localData.dataSets[set].points[i].y = 5;
		}
	}
	else if( set == 9 ){
		localData.dataSets[set].size = 500;

		for( i = 0; i < 50; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
			yval += 8;
		}

		for( i = 50; i < 100; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 7;
			yval -= 5;
		}

		for( i = 100; i < 130; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 1;
			yval += 3;
		}

		for( i = 130; i < 160; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 2;
			yval -= 5;
		}

		for( i = 160; i < 200; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 4;
			yval += 5;
		}

		for( i = 200; i < 210; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 10;
		}

		for( i = 210; i < 220; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			yval -= 10;
		}

		for( i = 220; i < 230; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval += 10;
		}

		for( i = 230; i < 240; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			yval += 10;
		}

		for( i = 240; i < 400; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 2;
			yval -= 1;
		}

		for( i = 400; i < 420; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 3;
			yval += 6;
		}

		for( i = 420; i < 450; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;

			xval -= 2;
		}

		for( i = 450; i < 500; i++){
			localData.dataSets[set].points[i].x = xval;
			localData.dataSets[set].points[i].y = yval;
		}
	}

}

void read_gps_realtime(void){

	int i;
	const char command[] = "$PMTK622,1*29\r\n";
	int length = strlen(command);
	char output[256] = {0};
	int lat_start = 18;
	int long_start = 30;
	char NS;
	char EW;
	float switch_pole = -1;

	// here we send the command to the gps
	//for(i = 0; i < length; i++){
	//	putchar_gps(command[i]);
	//}

	// The next lines are to actually parse the data to get
	// latitude/longitude information from realtime output
	read_string(output);
	printf("%s\n", output);

	while(output[0] != '$' || output[1] != 'G' ||
		  output[2] != 'P' || output[3] != 'G' ||
		  output[4] != 'G' || output[5] != 'A') {
		read_string(output);
		printf("%s\n", output);
	}

	i = lat_start;
	int j = 0;
	while(output[i] != ','){
		gps_realtime.latitude[j] = output[i];
		i++;
		j++;
	}

	i++; // this is to account for the comma parsing

	NS = output[i];

	i = long_start; // to account for next comma before longitude data
	int k = 0;
	while(output[i] != ','){
		gps_realtime.longitude[k] = output[i];
		i++;
		k++;
	}

	i++; // account for comma before long_pole data

	EW = output[i];

	printf("latitude: %s\n", gps_realtime.latitude);
	printf("longitude: %s\n", gps_realtime.longitude);

	datetime_to_degrees(gps_realtime.latitude, gps_realtime.longitude);

	/*if(NS == 'S'){
		gps_realtime.latitude *= switch_pole;
	}

	if(EW == 'W'){
		gps_realtime.longitude *= switch_pole;
	}*/

	printf("latitude: %f %c \n", gps_realtime.lat_float, NS);
	printf("longitude: %f %c \n", gps_realtime.long_float, EW);
}

void datetime_to_degrees(char *lat, char *lon){

	char lat_minutes[10] = {0};
	char lon_minutes[10] = {0};
	char lat_day[5] = {0};
	char lon_day[5] = {0};
	float latmin_temp, lonmin_temp, latday_temp, londay_temp;
	int i;
	int end_lat = 8;
	int end_lon = 9;

	// break up (d)dd and mm.mmmm
	for( i=2; i < end_lat; i++ ){
		lat_minutes[i-2] = lat[i];
	}

	for( i=3; i < end_lon; i++ ){
		lon_minutes[i-3] = lon[i];
	}

	for( i=0; i < 2; i++ ){
		lat_day[i] = lat[i];
	}

	for( i=0; i < 3; i++ ){
		lon_day[i] = lon[i];
	}
	
	printf("lat mins: %s\n", lat_minutes);
	printf("long mins: %s\n", lon_minutes);

	printf("lat day: %s\n", lat_day);
	printf("long day: %s\n", lon_day);
	
	// strings to float values
	latmin_temp = atof(lat_minutes);
	lonmin_temp = atof(lon_minutes);
	latday_temp = atof(lat_day);
	londay_temp = atof(lon_day);
	
	printf("lat min float: %f\n", latmin_temp);
	printf("long min float: %f\n", lonmin_temp);
	
	printf("lat min float: %f\n", latday_temp);
	printf("long min float: %f\n", londay_temp);

	// convert from date/time format to degree format
	gps_realtime.lat_float = latmin_temp/60 + latday_temp;
	gps_realtime.long_float = lonmin_temp/60 + londay_temp;
	
}
