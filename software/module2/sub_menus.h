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
#define BUFFER_BTW_BUTTONS 50 //Amount of black pixels between buttons
#define NMENUS 3 //Number of submenus

//Dimensions for save/load menu
#define SL_LEFT_BOX_XSTART XRES/5
#define SL_LEFT_BOX_XEND 100 + SL_LEFT_BOX_XSTART
#define SL_RIGHT_BOX_XSTART SL_RIGHT_BOX_XEND - 100
#define SL_RIGHT_BOX_XEND 4*XRES/5
#define SL_TOPBOX_YSTART YRES/4
#define SL_TOPBOC_YEND 62 + SL_TOPBOX_YSTART
#define SL_LOWBOX_YSTART SL_TOPBOC_YEND + BUFFER_BTW_BUTTONS
#define SL_LOWBOX_YEND SL_LOWBOX_YSTART + 50

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

void SaveLoadMenu(Point*, Colours*);

void InterpretMenu(Point*, Colours*);

void SettingsMenu(Point*, Colours*);

#endif /* SUB_MENUS_H_ */
