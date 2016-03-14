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
#include <string.h>
#include <stdio.h>

//Struct for background/text colour pairs - only a limited selection are available to the user
typedef struct{
	int background;
	int text;
}back_textPair;

//Variables for the interpret menu
double percentageLeft, percentageRight, percentageForward, percentageBack;

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

	percentageBack = (double)backCount / nPoints;
	percentageLeft = (double)leftCount / nPoints;

	percentageForward = 1 - percentageBack;
	percentageRight = 1 - percentageLeft;
}

/*
 * Displays the Save/Load menu.
 */
void SaveLoadMenu(Point* p, Colours* scheme){
	WriteFilledRectangle(0, 0, XRES-1, MENU_TOP-1, WHITE);

	//If the background is WHITE then just draw boarders in black for buttons
	if(scheme->menuBackground == WHITE){
		Rectangle(SL_LEFT_BOX_XSTART, SL_TOPBOX_YSTART, SL_LEFT_BOX_XEND, SL_TOPBOC_YEND, BLACK);
		Rectangle(SL_RIGHT_BOX_XSTART, SL_TOPBOX_YSTART, SL_RIGHT_BOX_XEND, SL_TOPBOC_YEND, BLACK);
		Rectangle(SL_LEFT_BOX_XSTART, SL_LOWBOX_YSTART, SL_LEFT_BOX_XEND, SL_LOWBOX_YEND, BLACK);
		Rectangle(SL_RIGHT_BOX_XSTART, SL_LOWBOX_YSTART, SL_RIGHT_BOX_XEND, SL_LOWBOX_YEND, BLACK);
	}else{ //Else use scheme to determine button colour
		WriteFilledRectangle(SL_LEFT_BOX_XSTART, SL_TOPBOX_YSTART, SL_LEFT_BOX_XEND, SL_TOPBOC_YEND, scheme->menuBackground);
		WriteFilledRectangle(SL_RIGHT_BOX_XSTART, SL_TOPBOX_YSTART,  SL_RIGHT_BOX_XEND, SL_TOPBOC_YEND, scheme->menuBackground);
		WriteFilledRectangle(SL_LEFT_BOX_XSTART, SL_LOWBOX_YSTART, SL_LEFT_BOX_XEND, SL_LOWBOX_YEND, scheme->menuBackground);
		WriteFilledRectangle(SL_RIGHT_BOX_XSTART, SL_LOWBOX_YSTART, SL_RIGHT_BOX_XEND, SL_LOWBOX_YEND, scheme->menuBackground);
	}

	//Draw text in buttons explaining them
	Text((SL_LEFT_BOX_XSTART + SL_LEFT_BOX_XEND)/2-19, (SL_TOPBOX_YSTART + SL_TOPBOC_YEND)/2-5, scheme->text, scheme->menuBackground, "<", 0);
	Text((SL_RIGHT_BOX_XSTART + SL_RIGHT_BOX_XEND)/2+19, (SL_TOPBOX_YSTART + SL_TOPBOC_YEND)/2-5, scheme->text, scheme->menuBackground, ">", 0);
	Text(SL_LEFT_BOX_XSTART + 10, SL_LOWBOX_YSTART + 10, scheme->text, scheme->menuBackground, "Load", 0);
	Text(SL_RIGHT_BOX_XSTART + 10, SL_LOWBOX_YSTART + 10, scheme->text, scheme->menuBackground, "Save", 0);

	int set = 0;//0 == GPS, else dataSet = set - 1.
	char* str = "DataSet   ";
	Text(XRES/2-25, (YRES/2 + 50)/2, scheme->text, scheme->menuBackground, "GPS", 0);

	//boolean
	int buttonTouched = 0;
	do{
		if(buttonTouched){
			buttonTouched = 0;
		}

		*p = GetPress();

		if(p->y > SL_TOPBOX_YSTART && p->y < SL_TOPBOC_YEND){
			//if decrement button touched, decrement set, display such has been done to user
			if(p->x > SL_LEFT_BOX_XSTART && p->x < SL_LEFT_BOX_XEND){
				buttonTouched = 1;
				WriteFilledRectangle(SL_LEFT_BOX_XEND + 1, SL_TOPBOX_YSTART, SL_RIGHT_BOX_XSTART - 1, SL_TOPBOC_YEND, WHITE);

				if(set == 0){
					set = MAX_N_SETS;//wrap-around
				}else{
					set--;
				}

				if(set == 0){
					Text(XRES/2-25, (YRES/2 + 50)/2, scheme->text, scheme->menuBackground, "GPS", 0);
				}else{
					str[8] = (char)(set / 10) + '0';
					str[9] = (char)(set % 10) + '0';
					if(str[8] == '0'){
						str[8] = str[9];
						str[9] = '\0';
					}

					Text(XRES/2-45, (YRES/2 + 50)/2, scheme->text, scheme->menuBackground, str, 0);
				}

				GetRelease();
			}else if(p->x > SL_RIGHT_BOX_XSTART && p->x < SL_RIGHT_BOX_XEND){
				//if increment button touched, increment set, display such has been done to user
				buttonTouched = 1;
				WriteFilledRectangle(SL_LEFT_BOX_XEND + 1, SL_TOPBOX_YSTART, SL_RIGHT_BOX_XSTART - 1, SL_TOPBOC_YEND, WHITE);

				if(set == MAX_N_SETS){
					set = 0;//wrap-around
				}else{
					set++;
				}

				if(set == 0){
					Text(XRES/2-25, (YRES/2 + 50)/2, scheme->text, scheme->menuBackground, "GPS", 0);
				}else{
					str[8] = (char)(set / 10) + '0';
					str[9] = (char)(set % 10) + '0';
					if(str[8] == '0'){
						str[8] = str[9];
						str[9] = '\0';
					}

					Text(XRES/2-45, (YRES/2 + 50)/2, scheme->text, scheme->menuBackground, str, 0);
				}

				GetRelease();
			}
		}else if(p->y < SL_LOWBOX_YEND && p->y > SL_LOWBOX_YSTART){
			//If load button touched, do following
			if(p->x > SL_LEFT_BOX_XSTART && p->x < SL_LEFT_BOX_XEND){
				buttonTouched = 1;

				if (set != 0) {
					//GPS is set 0 so we add 1 to get the correct index
					load_into_workingDataSet(set-1);
				} else {
					loadgps_workingDataSet();
				}

				return;
			}else if(p->x > SL_RIGHT_BOX_XSTART && p->x < SL_RIGHT_BOX_XEND){
				//If save button touched, do following
				buttonTouched = 1;
		
				if (set != 0) { // check set number since cannot save to GPS
					//GPS is set 0 so we add 1 to get the correct index
					save_from_workingDataSet(set-1);
				}

				WriteFilledRectangle(SL_LEFT_BOX_XSTART, 0, XRES-1, SL_TOPBOX_YSTART-1, WHITE);
				if(set == 0){
					Text(XRES/3-45, 10, BLACK, WHITE, "Cannot save to GPS", 0);
					}else{
					char tmpStr[strlen(str)+26];
					strcpy(tmpStr, "Current DataSet saved to ");
					strcpy(tmpStr+25,str);

					Text(XRES/3-45, 10, BLACK, WHITE, tmpStr, 0);
				}

				GetRelease();
			}
		}

	}while(buttonTouched || (p->y > MENU_TOP && p->x < XRES/NMENUS)); //Do this while a button in the submenu has been touched or the Save/Load menu button has been touched
}

/*
 * Displays the interpret menu
 */
void InterpretMenu(Point* p, Colours* scheme){
	WriteFilledRectangle(0, 0, XRES-1, MENU_TOP-1, scheme->menuBackground);

	char* str = "Time on Right:    %";
	str[15] = (char)((int)percentageRight + '0');
	str[16] = (char)((int)(10 * percentageRight) % 10 + '0');
	str[17] = (char)((int)(100 * percentageRight) % 10 + '0');

	if(str[15] == '0'){
		str[15] = ' ';
		if(str[16] == '0'){
			str[16] = ' ';
		}
	}

	Text(I_LEFT_ALIGN, I_TOP_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time on Left:    %";

	str[14] = (char)((int)percentageLeft + '0');
	str[15] = (char)((int)(10 * percentageLeft) % 10 + '0');
	str[16] = (char)((int)(100 * percentageLeft) % 10 + '0');

	if(str[14] == '0'){
		str[14] = ' ';
		if(str[15] == '0'){
			str[15] = ' ';
		}
	}

	Text(I_LEFT_ALIGN, I_BOT_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time Forward:    %";

	str[14] = (char)((int)percentageForward + '0');
	str[15] = (char)((int)(10 * percentageForward) % 10 + '0');
	str[16] = (char)((int)(100 * percentageForward) % 10 + '0');

	if(str[14] == '0'){
		str[14] = ' ';
		if(str[15] == '0'){
			str[15] = ' ';
		}
	}

	Text(I_RIGHT_ALIGN, I_TOP_ALIGN, scheme->text, scheme->menuBackground, str, 0);

	str = "Time Back:    %";

	str[11] = (char)((int)percentageBack + '0');
	str[12] = (char)((int)(10 * percentageBack) % 10 + '0');
	str[13] = (char)((int)(100 * percentageBack) % 10 + '0');

	if(str[11] == '0'){
		str[11] = ' ';
		if(str[12] == '0'){
			str[12] = ' ';
		}
	}

	Text(I_RIGHT_ALIGN, I_BOT_ALIGN, scheme->text, scheme->menuBackground, str, 0);

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
