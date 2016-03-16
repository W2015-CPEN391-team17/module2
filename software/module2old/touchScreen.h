
/*
 * touchScreen.h
 *
 *  Created on: 2016-02-03
 *      Author: Kyle
 */

#ifndef TOUCHSCREEN_H_
#define TOUCHSCREEN_H_

typedef struct{ int x, y; } Point;

void Init_Touch ( void );
int ScreenTouched( void);
void WaitForTouch();
Point GetPress(void);
Point GetRelease(void);



#endif /* TOUCHSCREEN_H_ */
