/*
 * datasets.c
 *
 * Functions for GPS data (mapped to screen pixels) representing a path
 *
 */

#include "datasets.h"
#include "gps_points.h"
#include "sd_card.h"
#include "graphics.h"

// data being displayed on the screen and cached from SD card
localDataSets localData;

/*
 * Load SD card contents to localData.dataSets
 */
void load_from_SD_to_dataSets() {
	Text(10,10,BLACK,WHITE,"Loading from SD Card...",0);

	//TODO check return value?
	sd_card_load(localData.dataSets, MAX_N_SETS, FILENAME);
}

/*
 * Save localData.dataSets to file FILENAME on SD card
 */
void save_to_SD_from_dataSets() {

	// DEBUG
	printf("entering save_to_SD_from_dataSets\n");

	//TODO check return value?
	sd_card_save(localData.dataSets, MAX_N_SETS, FILENAME);

	// DEBUG
	printf("finished sd_card_save\n");

	char teststr[JSON_DATA_MAX_LEN] = "";

	// DEBUG
	printf("exiting from save_to_SD_from_dataSets\n");
}

//Should be called each time new data comes in
void setupAggregate(){
	aggregateSet.size = 0;

	int i;
	for(i = 0; i < MAX_N_SETS; i++){
		int agIndex = aggregateSet.size;
		aggregateSet.size += localData.dataSets[i].size;

		int j;
		for(j = 0; j < localData.dataSets[i].size; j++, agIndex++){
			aggregateSet.points[agIndex].x = localData.dataSets[i].points[j].x;
			aggregateSet.points[agIndex].y = localData.dataSets[i].points[j].y;
		}
	}
}
