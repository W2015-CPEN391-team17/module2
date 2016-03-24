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
	int size; // number of points in the set
	GPSPoint points[MAX_N_POINTS];
} dataSet;

typedef struct {
	char firstNumber; //TODO just for testing
	char secondNumber; //TODO just for testing
	dataSet dataSets[MAX_N_SETS]; // cache for SD contents
	int headTimeQueue;
} localDataSets;

struct{
	int size;
	GPSPoint points[MAX_N_POINTS*MAX_N_SETS];
} aggregateSet;

/*
 * Load data from FILENAME on SD card to localData.dataSets
 */
void load_from_SD_to_dataSets(void);

/*
 * Save localData.dataSets to file FILENAME on SD card
 */
void save_to_SD_from_dataSets(void);

void setupAggregate();

#endif /* DATASETS_H_ */
