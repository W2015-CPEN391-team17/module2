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
	int result = sd_card_load(FILENAME);
	if (result) {
		printf("load_from_SD_to_dataSets result!!!\n");
	}
}

/*
 * Save localData.dataSets to file FILENAME on SD card
 */
void save_to_SD_from_dataSets() {
	int result = sd_card_save(FILENAME);
	if (result) {
		printf("save_to_SD_from_dataSets result!!!\n");
	}
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
