/*
 * datasets.h
 *
 * Structures for GPS data (mapped to screen pixels) representing a path
 *
 */

#ifndef DATASETS_H_
#define DATASETS_H_

#include "conversion.h"

#define MAX_N_POINTS 1024

//assume double-digit at most (aka <= 99)
#define MAX_N_SETS 10

typedef struct {
	int size; //number of points in the set
	GPSPoint points[MAX_N_POINTS]; //
} dataSet;

typedef struct {
	dataSet workingDataSet; //data currently being displayed
	dataSet dataSets[MAX_N_SETS]; //cache for SD contents
	int headTimeQueue;
} localDataSets;

// data being displayed on the screen and cached from SD card
localDataSets localData;

/*
 * Copy the data from the slot with the given index in datasets
 * to workingDataSet
 */
void load_into_workingDataSet(int index);

/*
 * Copy the data from the workingDataSet to the slot with the
 * given index in datasets
 */
void save_from_workingDataSet(int index);

/*
 * On start up, copy the data from the GPS to the workingDataSet
 * to display the GPS data immediately
 */
void loadgps_workingDataSet(void);

/*
 * Load data from FILENAME on SD card to localData.dataSets
 */
void load_from_SD_to_dataSets(void);

/*
 * Save localData.dataSets to file FILENAME on SD card
 */
void save_to_SD_from_dataSets(void);

#endif /* DATASETS_H_ */
