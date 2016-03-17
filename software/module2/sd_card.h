#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "datasets.h"

#define FILENAME "data.txt"
#define JSON_DATA_MAX_LEN ((100)*(MAX_N_SETS)*(MAX_N_POINTS))

// An array of dataSets are represented in JSON with this schema:
//[
// {
//	"size": <double>,
//	"gpspoints": [
//	              {
//					"x": <double>,
//					"y": <double>
//	              }
//	]
// }
//]!
//
// The '!' is a sentinel value denoting the end of useful information.

// The parameter `out[]` contains the string representation of the
// JSON data including the '!' sentinel value.
void sd_card_cJSON_stringify(dataSet in[], int in_len, char out[]);

// The parameter `data[]` contains the string representation of the
// JSON data including the '!' sentinel value.
void sd_card_cJSON_parse(char data[], dataSet out[]);

// Save dataSet dsets[] to the SD card.
// Return 0 on success, return -1 on error writing to the SD card,
// return -2 if the SD card could not be opened, return -3 if manual
// disconnection of the SD card was necessary, otherwise return a
// non-zero value.
int sd_card_save(dataSet dsets[], int dsets_len, char *filename);

// Load dataSet from the SD card to dataSet out[].
// Return 0 on success, return -2 if SD card could not be opened,
// return -3 if manual disconnection of the SD card was necessary,
// otherwise return a non-zero value.
int sd_card_load(dataSet out[], int out_len, char *filename);

#endif /* SD_CARD_H_ */
