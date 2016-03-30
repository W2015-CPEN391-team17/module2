/*
 * sub_menus.c
 *
 *  Created on: 2016-02-11
 *      Author: Kyle
 */

#include "sub_menus.h"
#include "graphics.h"
#include "touchScreen.h"
#include "datasets.h"
#include "bluetooth.h"
#include <string.h>
#include <stdio.h>

extern localDataSets localData;

//Struct for background/text colour pairs - only a limited selection are available to the user
typedef struct{
	int background;
	int text;
}back_textPair;

//Variables for the interpret menu
double percentageN, percentageS, percentageE, percentageW;
int timePerPoint = 1;//In seconds
int currNPoints;

//In seconds
void setInterpretTimePerPoint(int timePerPointTemp){
	timePerPoint = timePerPointTemp;
}
/*
 * Initialises the interpret menu, must be called each time the heat map data is changed. Saves doing computations every time
 * the interpret menu is brought up.
 */
void initInterpret(int count[HEATMAP_H][HEATMAP_V], int nPoints){
	int backCount = 0;
	int leftCount = 0;

	int i;
	for(i = 0; i < HEATMAP_H; i++){
		int j;
		for(j = 0; j < HEATMAP_V; j++){
			if(i <= HEATMAP_H / 2){
				backCount += count[i][j];
			}

			if(j <= HEATMAP_V / 2){
				leftCount += count[i][j];
			}
		}
	}

	percentageW = (double)backCount / nPoints;
	percentageN = (double)leftCount / nPoints;

	percentageE = 1 - percentageW;
	percentageS = 1 - percentageN;

	currNPoints = nPoints;
}

/*
 * Displays the Save/Load menu.
 */
int SaveLoadMenu(Point* p, Colours* scheme, int prevAg){
	WriteFilledRectangle(0, 0, XRES-1, MENU_TOP-1, WHITE);

	//If the background is WHITE then just draw boarders in black for buttons
	if(scheme->menuBackground == WHITE){
		Rectangle(SL_LEFT_BOX_XSTART, SL_TOPBOX_YSTART, SL_LEFT_BOX_XEND, SL_TOPBOC_YEND, BLACK);
		Rectangle(SL_RIGHT_BOX_XSTART, SL_TOPBOX_YSTART, SL_RIGHT_BOX_XEND, SL_TOPBOC_YEND, BLACK);
		Rectangle(SL_UPPERBOX_XSTART, SL_UPPERBOX_YSTART, SL_UPPERBOX_XEND, SL_UPPERBOX_YEND, BLACK);
	}else{ //Else use scheme to determine button colour
		WriteFilledRectangle(SL_LEFT_BOX_XSTART, SL_TOPBOX_YSTART, SL_LEFT_BOX_XEND, SL_TOPBOC_YEND, scheme->menuBackground);
		WriteFilledRectangle(SL_RIGHT_BOX_XSTART, SL_TOPBOX_YSTART,  SL_RIGHT_BOX_XEND, SL_TOPBOC_YEND, scheme->menuBackground);
		WriteFilledRectangle(SL_UPPERBOX_XSTART, SL_UPPERBOX_YSTART, SL_UPPERBOX_XEND, SL_UPPERBOX_YEND, scheme->menuBackground);
	}

	//Draw text in buttons explaining them
	Text(SL_LEFT_BOX_XSTART+19, (SL_TOPBOX_YSTART + SL_TOPBOC_YEND)/2, scheme->text, scheme->menuBackground, "Load Latest Data", 0);
	Text(SL_RIGHT_BOX_XSTART+19, (SL_TOPBOX_YSTART + SL_TOPBOC_YEND)/2, scheme->text, scheme->menuBackground, "Load Aggregated Data", 0);
	Text(SL_UPPERBOX_XSTART+19, (SL_UPPERBOX_YSTART + SL_UPPERBOX_YEND)/2, scheme->text, scheme->menuBackground, "Download Path from BT", 0);

	do{
		*p = GetPress();

		if(p->y < SL_TOPBOC_YEND && p->y > SL_TOPBOX_YSTART && p->x > SL_LEFT_BOX_XSTART && p->x < SL_RIGHT_BOX_XEND){
			if(p->x < SL_LEFT_BOX_XEND){
				//Load latest data
				return FALSE;
			}else if(p->x > SL_RIGHT_BOX_XSTART){
				//Load aggregate
				return TRUE;
			}
		}else if(p->x > SL_UPPERBOX_XSTART && p->x < SL_UPPERBOX_XEND && p->y > SL_UPPERBOX_YSTART && p->y < SL_UPPERBOX_YEND){
			processBT();
			//Add more here to re-generate heatmap etc etc
			return prevAg;//Maybe change to T/F
		}
	}while(p->x < XRES/3 && p->y > MENU_TOP);


	return prevAg;
}

/*
 * Displays the interpret menu
 */
void InterpretMenu(Point* p, Colours* scheme){
	WriteFilledRectangle(0, 0, XRES-1, MENU_TOP-1, scheme->menuBackground);

	char* str = "Time South:    %";
	str[12] = (char)((int)percentageS + '0');
	str[13] = (char)((int)(10 * percentageS) % 10 + '0');
	str[14] = (char)((int)(100 * percentageS) % 10 + '0');

	if(str[12] == '0'){
		str[12] = ' ';
		if(str[13] == '0'){
			str[13] = ' ';
		}
	}

	Text(I_LEFT_ALIGN, I_TOP_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time North:    %";

	str[12] = (char)((int)percentageN + '0');
	str[13] = (char)((int)(10 * percentageN) % 10 + '0');
	str[14] = (char)((int)(100 * percentageN) % 10 + '0');

	if(str[12] == '0'){
		str[12] = ' ';
		if(str[13] == '0'){
			str[13] = ' ';
		}
	}

	Text(I_LEFT_ALIGN, I_BOT_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time East:    %";

	str[11] = (char)((int)percentageE + '0');
	str[12] = (char)((int)(10 * percentageE) % 10 + '0');
	str[13] = (char)((int)(100 * percentageE) % 10 + '0');

	if(str[11] == '0'){
		str[11] = ' ';
		if(str[12] == '0'){
			str[12] = ' ';
		}
	}

	Text(I_RIGHT_ALIGN, I_TOP_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time West:    %";

	str[11] = (char)((int)percentageW + '0');
	str[12] = (char)((int)(10 * percentageW) % 10 + '0');
	str[13] = (char)((int)(100 * percentageW) % 10 + '0');

	if(str[11] == '0'){
		str[11] = ' ';
		if(str[12] == '0'){
			str[12] = ' ';
		}
	}

	Text(I_RIGHT_ALIGN, I_BOT_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	if(localData.dataSets[localData.headTimeQueue].size == currNPoints){
		int timeTaken = currNPoints * timePerPoint / 60;
		if(timeTaken > 999){
			str = "Time Taken: >999 minutes";
		}else{
			str = "Time Taken:     minute(s)";//In minutes, max 999
			str[12] = (char)(timeTaken/100 + '0');
			str[13] = (char)((timeTaken/10) % 10 + '0');
			str[14] = (char)(timeTaken % 10 + '0');

			if(str[12] == '0'){
				str[12] = ' ';
				if(str[13] == '0'){
					str[13] = ' ';
				}
			}
		}

		Text(XRES/2 - 125, I_BOT_ALIGN + BUFFER_BTW_BUTTONS, scheme->text, scheme->menuBackground, str, 0);
	}else{
		int nSets = 0;
		int i;
		for(i = 0; i < MAX_N_SETS; i++){
			if(localData.dataSets[i].size > 0){
				nSets++;
			}
		}

		int averageTimeTaken = currNPoints * timePerPoint / 60 / nSets;
		if(averageTimeTaken > 999){
			str = "Average Time Taken: >999 minutes";
		}else{
			str = "Average Time Taken:     minute(s)";//In minutes, max 999
			str[20] = (char)(averageTimeTaken/100 + '0');
			str[21] = (char)((averageTimeTaken/10) % 10 + '0');
			str[22] = (char)(averageTimeTaken % 10 + '0');

			if(str[20] == '0'){
				str[20] = ' ';
				if(str[21] == '0'){
					str[21] = ' ';
				}
			}
		}
		Text(XRES/2 - 200, I_BOT_ALIGN + BUFFER_BTW_BUTTONS, scheme->text, scheme->menuBackground, str, 0);
	}


	do{
		*p = GetPress();
	}while(p->x > XRES/NMENUS && p->x < 2*XRES/NMENUS && p->y > MENU_TOP);
}

/*
 * Allows user to change the menu background colours as well as text colour and line (for connect-the-dots) colour.
 * Makes sure the text/background pair is acceptable (text is viewable on background).
 * Makes sure the line colour is not white and is not the same as the background colour.
 */
void SettingsMenu(Point* p, Colours* scheme){
	back_textPair pairs[NPAIRS]; //Initialise pairs of background/text colours
	pairs[INITPAIR].background = WHITE;
	pairs[INITPAIR].text = BLACK;
	pairs[1].background = CYAN;
	pairs[1].text = RED;
	pairs[2].background = YELLOW;
	pairs[2].text = BLUE;

	WriteFilledRectangle(0, 0, XRES-1, MENU_TOP-1, BLACK);//Clear background

	//Boolean
	int settingsTouched = 0;
	int buttonTouched = 0;// 0 == Menu just opened, 1 == Line, 2 == Text/Background
	do{
		//Redraw buttons/lines with new colours if the button pressed wasn't the settings button
		if(!settingsTouched){
			if(buttonTouched == 0 || buttonTouched == 2){
				WriteFilledRectangle(SET_LEFT_ALIGN, 0, SET_RIGHT, SET_TOP_YEND,scheme->menuBackground);//Create buttons
				WriteFilledRectangle(SET_LEFT_ALIGN, SET_BOT_YSTART, SET_RIGHT, SET_BOT_YEND,scheme->menuBackground);

				Text(SET_LEFT_ALIGN+10, (SET_TOP_YEND)/2, scheme->text, scheme->menuBackground, "Text/background", 0);//Print text on buttons
				Text(SET_LEFT_ALIGN+10, SET_BOT_YSTART + 1, scheme->text, scheme->menuBackground, "Line Colour", 0);
			}
			//Display example line
			WriteLine(SET_LEFT_ALIGN, SET_BOT_YEND-1, SET_RIGHT, SET_BOT_YSTART-1, scheme->connectTheDotsLine);
			WriteLine(SET_LEFT_ALIGN, SET_BOT_YEND, SET_RIGHT, SET_BOT_YSTART, scheme->connectTheDotsLine);
			WriteLine(SET_LEFT_ALIGN, SET_BOT_YEND+1, SET_RIGHT, SET_BOT_YSTART, scheme->connectTheDotsLine);
		}else{
			settingsTouched = 0;
		}

		*p = GetPress();

		//If the text/background colour button is pressed
		if(p->x <= SET_RIGHT && p->x >= SET_LEFT_ALIGN && p->y <= SET_TOP_YEND){
			buttonTouched = 2;
			do{
				scheme->menuBackground = pairs[(scheme->pairNum + 1) % NPAIRS].background;
				scheme->text = pairs[(scheme->pairNum + 1) % NPAIRS].text;
				scheme->pairNum = (scheme->pairNum + 1) % NPAIRS;
			}while(scheme->menuBackground == scheme->connectTheDotsLine);//Change the colour pair if the background was the same colour as the line
			GetRelease();
		}else if(p->x <= SET_RIGHT && p->x >= SET_LEFT_ALIGN && p->y <= SET_BOT_YEND && p->y > SET_BOT_YSTART){ //If the line colour button was pressed
			buttonTouched = 1;
			do{
				scheme->connectTheDotsLine = (scheme->connectTheDotsLine + 1) % NCOLOURS;
			}while(scheme->connectTheDotsLine == WHITE || scheme->connectTheDotsLine == scheme->menuBackground);//Change the line colour if it is white or is the same colour as the background
			GetRelease();
		}else if(!(p->x >= 2 * XRES / NMENUS && p->y >= MENU_TOP)){//If touched outside of this menu's buttons and not the settings button itself break out of menu
			break;
		}else{//Else the settings button was touched
			settingsTouched = 1;
		}
	}while(1); //Do this while in settings menu

}
