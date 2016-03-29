/*
 * sub_menus.h
 *
 *  Created on: 2016-02-11
 *      Author: Kyle
 */

#ifndef SUB_MENUS_H_
#define SUB_MENUS_H_

#include "touchScreen.h"
#include "graphics.h"

#define NPAIRS 3 //Number of text/background colours
#define INITPAIR 0 //Should stay 0 - is the initial text/background pair
#define BUFFER_BTW_BUTTONS 50 //Amount of pixels between buttons
#define NMENUS 3 //Number of submenus

//Dimensions for save/load menu
#define SL_LEFT_BOX_XSTART BUFFER_BTW_BUTTONS
#define SL_LEFT_BOX_XEND XRES/2 - BUFFER_BTW_BUTTONS/2
#define SL_RIGHT_BOX_XSTART XRES/2 + BUFFER_BTW_BUTTONS/2
#define SL_RIGHT_BOX_XEND XRES - BUFFER_BTW_BUTTONS
#define SL_TOPBOX_YSTART YRES/3
#define SL_TOPBOC_YEND 2*YRES/3

//Dimensons for current position menu
#define POS_LEFT_ALIGN XRES/2-200
#define POS_RIGHT POS_LEFT_ALIGN + 400
#define POS_TOP_YEND 0
#define POS_BOT_ALIGN POS_TOP_YEND + 100

//Dimensions for interpret menu
#define I_LEFT_ALIGN XRES/5
#define I_RIGHT_ALIGN 3*XRES/5
#define I_TOP_ALIGN 10
#define I_BOT_ALIGN 210

//Dimensions for settings menu
#define SET_LEFT_ALIGN XRES/2-100
#define SET_RIGHT SET_LEFT_ALIGN + 200
#define SET_TOP_YEND SET_BOT_YSTART - BUFFER_BTW_BUTTONS
#define SET_BOT_YSTART MENU_TOP/2
#define SET_BOT_YEND MENU_TOP - BUFFER_BTW_BUTTONS

void initInterpret(int count[HEATMAP_H][HEATMAP_V], int nPoints);

void setInterpretTimePerPoint(int timePerPointTemp);

int SaveLoadMenu(Point*, Colours*, int);

void InterpretMenu(Point*, Colours*);

void SettingsMenu(Point*, Colours*);

#endif /* SUB_MENUS_H_ */
