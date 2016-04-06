/*
 * conversion.c
 *
 *  Created on: 2016-02-06
 *      Author: Kyle
 */

#include "conversion.h"
#include "graphics.h" //for XRES/YRES and MENU_TOP
#include "gps_points.h"

#include <math.h>
#include <stdlib.h>

// from gps_points.c
extern struct points gps_points[100];

extern struct points gps_realtime;

/*
 * Assumes field is in line with lat/long. Converts global array of gps_points to array of GPSPoint scaled to screen pixels.
 * Callocs the returned pointer. Calling function needs to deal with free()'ing
 */
GPSPoint* convertGPSPoints( int nPoints ){
	GPSPoint* points = calloc(nPoints, sizeof(GPSPoint));

	double mX = (double) XRES / (TOPRIGHTLONG - TOPLEFTLONG); //Using two linear functions f(x) = mx + b so these are the m's
	double mY = (double) MENU_TOP / (TOPRIGHTLAT - BOTRIGHTLAT);

	int i;
	for(i = 0; i < nPoints; i++){
		points[i].x = mX * (gps_points[i].long_float - TOPLEFTLONG);
		points[i].y = MENU_TOP - (mY * (gps_points[i].lat_float - BOTRIGHTLAT));

		//Following if statements are for rounding
		if(points[i].x > XRES && points[i].x < XRES + 1){
			points[i].x = XRES;
		}
		if(points[i].y > MENU_TOP && points[i].y < MENU_TOP + 1){
			points[i].y = MENU_TOP;
		}
	}

	return points;
}

void convertGPSPointsGivenPoints( int nPoints, GPSPoint* points ){

	double mX = (double) XRES / (2*LONG_P_M_FLT); //Using two linear functions f(x) = mx + b so these are the m's
	double mY = (double) MENU_TOP / (2*LAT_P_M_FLT);

	int i;
	for(i = 0; i < nPoints; i++){
		points[i].x = mX * (points[i].x - ((0-gps_realtime.long_float) - LONG_P_M_FLT));
		points[i].y = MENU_TOP - (mY * (points[i].y - (gps_realtime.lat_float - LAT_P_M_FLT)));

		//Following if statements are for rounding
		if(points[i].x > XRES && points[i].x < XRES + 1){
			points[i].x = XRES;
		}
		if(points[i].y > MENU_TOP && points[i].y < MENU_TOP + 1){
			points[i].y = MENU_TOP;
		}
	}
}

/*
 * Converts GPSpoints to points for graphics. Does NOT take care of ALL points outside of range. Only works for topRight in quadrant 4
 * relative to topLeft. Needs to be tested.
 *
 * NOT USED DUE TO REQUIRING EXACTLY 90 DEGREE ANGLE AT CORNERS OF FIELD, INCLUDED FOR FUTURE USE (in more realistic version)
 *
 * Just does the basic trig for a 2D rotation
 */
void convertGPSReal( GPSPoint topLeft, GPSPoint topRight, GPSPoint points[], GPSPoint bottomLeft, int nPoints ){
	double theta = -atan((topRight.y - topLeft.y)/(topRight.x - topLeft.x));

	int i;
	for(i = 0; i < nPoints; i++){
		float oldY = points[i].y - topLeft.y;
		points[i].y = topLeft.y + oldY*cos(theta) + (points[i].x - topLeft.x)*sin(theta);
		points[i].x = topLeft.x + (points[i].x - topLeft.x)*cos(theta) - oldY*sin(theta);
	}

	float oldY = bottomLeft.y - topLeft.y;
	bottomLeft.y = topLeft.y + oldY*cos(theta) + (bottomLeft.x - topLeft.x)*sin(theta);
	bottomLeft.x = topLeft.x + (bottomLeft.x - topLeft.x)*cos(theta) - oldY*sin(theta);

	oldY = topRight.y - topLeft.y;
	topRight.y = topLeft.y + oldY*cos(theta) + (topRight.x - topLeft.x)*sin(theta);
	topRight.x = topLeft.x + (topRight.x - topLeft.x)*cos(theta) - oldY*sin(theta);

	double mX = XRES / (double) (topRight.x - topLeft.x);
	double mY = YRES / (double) (topRight.y - bottomLeft.y);

	int k;
	for( k = 0; k < nPoints; k++){
		points[k].x = mX * (points[k].x - topLeft.x);
		points[k].y = mY * (points[k].y - bottomLeft.y);

		if(points[k].x < 0 && points[k].x > -1){
			points[k].x = 0;
		}else if(points[k].x > XRES && points[k].x < XRES + 1){
			points[k].x = XRES;
		}

		if(points[k].y < 0 && points[k].y > -1){
					points[k].y = 0;
		}else if(points[k].y > YRES && points[k].y < YRES + 1){
			points[k].y = YRES;
		}
	}

	topRight.x = mX * (topRight.x - topLeft.x);//should eval to XRES
	topRight.y = mY * (topRight.y - bottomLeft.y);//should eval to YRES

	topLeft.x = mX * (topLeft.x - topLeft.x);//should eval to 0
	topLeft.y = mY * (topLeft.y - bottomLeft.y);//should eval to YRES

	bottomLeft.x = mX * (bottomLeft.x - topLeft.x);//should eval to XRES
	bottomLeft.y = mY * (bottomLeft.y - bottomLeft.y);//should eval to 0
}
