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

/*
 * Copy the data from the slot with the given index in datasets
 * to workingDataSet
 */
void load_into_workingDataSet(int index) {
	//for the slot with the given index
	dataSet *copyFromSet = &(localData.dataSets[index]);

	//copy the data
	int size = copyFromSet->size;
	localData.workingDataSet.size = size;
	int i;
	for(i = 0; i < size; i++) {
		localData.workingDataSet.points[i].x = copyFromSet->points[i].x;
		localData.workingDataSet.points[i].y = copyFromSet->points[i].y;
	}
}

/*
 * Copy the data from the workingDataSet to the slot with the
 * given index in datasets
 */
void save_from_workingDataSet(int index) {
	//to the slot with the given index
	dataSet *copyToSet = &localData.dataSets[index];

	//copy the data
	int size = localData.workingDataSet.size;
	copyToSet->size = size;
	int i;
	for(i = 0; i < size; i++) {
		copyToSet->points[i].x = localData.workingDataSet.points[i].x;
		copyToSet->points[i].y = localData.workingDataSet.points[i].y;
	}

	//TODO test this
	save_to_SD_from_dataSets();

	// DEBUG
	printf("exiting from save_from_workingDataSet\n");
}

/*
 * On start up, copy the data from the GPS to the workingDataSet
 * to display the GPS data immediately
 */
void loadgps_workingDataSet(){
	GPSPoint* screen_points;

	screen_points = convertGPSPoints(70);

	GPSPoint *workingDataPoints = localData.workingDataSet.points;

	int j;
	for( j = 0; j < 70; j++ ){
		workingDataPoints[j].x = screen_points[j].x;
		workingDataPoints[j].y = screen_points[j].y;
	}

	free(screen_points); // free the calloc'd address space from convertGPSPoints

	localData.workingDataSet.size = 70;
}

/*
 * Load SD card contents to localData.dataSets
 */
void load_from_SD_to_dataSets() {
	//TODO check return value of below call
	Text(10,10,BLACK,WHITE,"Loading from SD Card...",0);
	sd_card_load(localData.dataSets, MAX_N_SETS, FILENAME);
}

/*
 * Save localData.dataSets to file FILENAME on SD card
 */
void save_to_SD_from_dataSets() {
	//TODO check return value of below call

	// DEBUG
	printf("entering save_to_SD_from_dataSets\n");

	sd_card_save(localData.dataSets, MAX_N_SETS, FILENAME);

	// DEBUG
	printf("finished sd_card_save\n");

	char teststr[JSON_DATA_MAX_LEN] = "";
	sd_card_cJSON_stringify(localData.dataSets, MAX_N_SETS, teststr);

	// DEBUG
	printf("exiting from save_to_SD_from_dataSets\n");
}
