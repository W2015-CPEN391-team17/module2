#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "datasets.h"

#define FILENAME "data.txt"
#define JSON_DATA_MAX_LEN ((100)*(MAX_N_SETS)*(MAX_N_POINTS))

// Save from localData into the given file on the SD card
// Return 0 on success, return -1 on error writing to the SD card,
// return -2 if the SD card could not be opened, return -3 if manual
// disconnection of the SD card was necessary, otherwise return a
// non-zero value.
int sd_card_save(char *filename);

// Load into localData from the given file on the SD card
// Return 0 on success, return -2 if SD card could not be opened,
// return -3 if manual disconnection of the SD card was necessary,
// otherwise return a non-zero value.
int sd_card_load(char *filename);

#endif /* SD_CARD_H_ */
