/*
 * conversion.h
 *
 *  Created on: 2016-02-06
 *      Author: Kyle
 */

#ifndef CONVERSION_H_
#define CONVERSION_H_

//Co-ordinates for corners
#define TOPRIGHTLAT (float) 49.268000
#define TOPRIGHTLONG (float) -123.248000
#define TOPLEFTLAT (float) 49.268000
#define TOPLEFTLONG (float) -123.251700
#define BOTRIGHTLAT (float) 49.266500
#define BOTRIGHTLONG (float) -123.248000

#include "touchScreen.h"

typedef struct { float x, y; } GPSPoint;

GPSPoint* convertGPSPoints( int nPoints );
void convertGPSReal( GPSPoint topLeft, GPSPoint topRight, GPSPoint points[], GPSPoint bottomRight, int nPoints);


#endif /* CONVERSION_H_ */
