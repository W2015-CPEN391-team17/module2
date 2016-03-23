/*
 * Entry point for module 1.
 *
 */

#include <stdio.h>
#include <Altera_UP_SD_Card_Avalon_Interface.h>
#include "conversion.h"
#include "touchScreen.h"
#include "colours.h"
#include "graphics.h"
#include "heatmap.h"
#include "gps_points.h"
#include "sub_menus.h"
#include "sd_card.h"
#include "datasets.h"
#include "bluetooth.h"

// Meta stuff.
void initialize_components(void);
void initialize_colourScheme(void);
void initialize_datasets(void);
void initialize_demodata(void);
void cleanup(void);

// Data-independent drawing functions.
void draw_menu(void);

// Main menu function
void main_menu(void);

// Draw settings
Colours colourScheme;

#define GPSPOINTLEN 2
#define GPSPOINTSETLEN 2

// from datasets.c
extern localDataSets localData;

void compareLocalDataSets(localDataSets *original, localDataSets *copied) {
	printf("original headTimeQueue: %d, copied headTimeQueue: %d\n",
			original->headTimeQueue, copied->headTimeQueue);
	printf("TODO didn't write the other checks yet\n");
}

int main()
{
  Point p;

  // test SD card
  initialize_demodata();
  struct localDataSets originalLocalData = localData;
  save_to_SD_from_dataSets();
  load_from_SD_to_dataSets();
  //TODO compare localData and originalLocalData
  compareLocalDataSets(&originalLocalData, &localData);

#if 0
  printf("Starting module 2 code.\n");
  initialize_components();

  init_gps(); //TODO why do we do this twice?

  Text(10, 10, "Touch to get current location.");
  p = GetPress();
  p = GetRelease();

  read_gps_realtime();
  initialize_colourScheme();
  initialize_datasets();

  //save_points();

  //TODO DEBUG
  //initialize_demodata();
  //save_to_SD_from_dataSets();
  //load_from_SD_to_dataSets();
  //printf("\n\nlocalDataSets size %d\n\n", sizeof(localDataSets));

  main_menu();

  // Should never reach this point, but here in case we implement an exit button.
  cleanup();
#endif
  printf("Program terminated.\n");

  return 0;
}

// initialize each of the hardware components and clear the screen
void initialize_components(void)
{
	init_gps();
	init_bluetooth();
	Init_Touch();
	clear_screen(WHITE);
}

// initialize the colour scheme that will be used for the menus and data visualizations
void initialize_colourScheme(void)
{
	colourScheme.menuBackground = WHITE;
	colourScheme.text = BLACK;
	colourScheme.connectTheDotsLine = BLACK;
	colourScheme.pairNum = INITPAIR;
	colourScheme.shades[0] = OLIVE_DRAB;
	colourScheme.shades[1] = YELLOW_GREEN;
	colourScheme.shades[2] = LAWN_GREEN;
	colourScheme.shades[3] = GREEN_YELLOW;
	colourScheme.shades[4] = YELLOW;
	colourScheme.shades[5] = GOLD;
	colourScheme.shades[6] = ORANGE;
	colourScheme.shades[7] = DARK_ORANGE;
	colourScheme.shades[8] = ORANGE_RED;
	colourScheme.shades[9] = RED;
}

void initialize_datasets()
{
	// set all data sets to 0
	int i;
	for(i = 0; i < MAX_N_SETS; i++) {
		localData.dataSets[i].size = 0;
	}

	localData.headTimeQueue = 0;

	aggregateSet.size = 0;
}

void initialize_demodata()
{
	// set all data sets to demo data
	int set = 0;
	for(set = 0; set < MAX_N_SETS; set++){
		save_demo_points(set);
	}

	setupAggregate();
}

void read_from_SD()
{

}

void cleanup(void)
{
	//Nothing yet
}

void draw_menu(void)
{
	WriteFilledRectangle(0, MENU_TOP, XRES-1, YRES-1, colourScheme.menuBackground);
	WriteHLine(0, MENU_TOP, XRES - 1, BLACK);
	WriteVLine(XRES/3, MENU_TOP, YRES - MENU_TOP - 1, BLACK);
	WriteVLine(XRES*2/3, MENU_TOP, YRES - MENU_TOP - 1, BLACK);
	Text(10, (MENU_TOP + YRES)/2, colourScheme.text, colourScheme.menuBackground, "Load", 0);
	Text(XRES/3 + 10, (MENU_TOP + YRES)/2, colourScheme.text, colourScheme.menuBackground, "Interpret", 0);
	Text(XRES*2/3 + 10, (MENU_TOP + YRES)/2, colourScheme.text, colourScheme.menuBackground, "Settings", 0);
}

void main_menu(void)
{
	clear_screen(WHITE);

	draw_heatmap(localData.dataSets[localData.headTimeQueue].points, localData.dataSets[localData.headTimeQueue].size, colourScheme);
	Rectangle(XRES/2-5, MENU_TOP/2-5, XRES/2+5, MENU_TOP/2+5, BLACK);
	draw_menu();
	Point p;
	p = GetPress();
	GetRelease();

	int firstTime = TRUE;
	int showing_heatmap = TRUE;
	int outSubMenu = FALSE;
	int aggregate = FALSE;
	while(1)
	{
		if(p.y < MENU_TOP){
			if(!outSubMenu && !firstTime){
				p = GetPress();
				GetRelease();
			}
		}

		if(p.y < MENU_TOP){
			if(!outSubMenu && !firstTime){
				showing_heatmap = !showing_heatmap;
			}else{
				outSubMenu = FALSE;
			}

			if(showing_heatmap){
				WriteFilledRectangle(0,0,XRES-1,MENU_TOP-1,WHITE);
				if(aggregate){
					connect_points_all_sets(localData, colourScheme);
				}else{
					connect_points(localData.dataSets[localData.headTimeQueue].points, localData.dataSets[localData.headTimeQueue].size, colourScheme.connectTheDotsLine);
				}
			}else{
				if(aggregate){
					draw_heatmap(aggregateSet.points, aggregateSet.size, colourScheme);
				}else{
					draw_heatmap(localData.dataSets[localData.headTimeQueue].points, localData.dataSets[localData.headTimeQueue].size, colourScheme);
				}
			}

			Rectangle(XRES/2-5, MENU_TOP/2-5, XRES/2+5, MENU_TOP/2+5, BLACK);
		}else{
			if(firstTime){
				showing_heatmap = !showing_heatmap;
			}

			if(p.x < XRES / NMENUS){
				//Save/Load touched
				aggregate = SaveLoadMenu(&p, &colourScheme);
				showing_heatmap = FALSE;
				GetRelease();
			}else if(p.x < 2 * XRES / NMENUS){
				//Interpret touched
				InterpretMenu(&p, &colourScheme);
				GetRelease();
			}else{
				//Settings touched
				SettingsMenu(&p, &colourScheme);
				GetRelease();
				draw_menu();
			}
			outSubMenu = TRUE;
		}
		if(firstTime){
			firstTime = FALSE;
		}
	}
}
