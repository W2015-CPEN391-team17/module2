#include <stdio.h>
#include <stdlib.h>
#include <Altera_UP_SD_Card_Avalon_Interface.h>
#include <string.h>
#include "sd_card.h"
#include "datasets.h"
#include "conversion.h"
#include "cJSON.h"

// Use ASCII char '!' to delimit the end of useful data in a file
#define SD_SENTINEL 33

void sd_card_cJSON_stringify(dataSet in[], int in_len, char out[])
{
	// Convert GPSPoint and dataSet structs into JSON string data
	cJSON *root;

	//DEBUG
	printf("in sd_card_cJSON_stringify\n");

	root = cJSON_CreateArray();
	if (in != NULL) {

		//DEBUG
		printf("beginning for-loop: Convert dataSet into JSON string data\n");

		int i;
		for (i = 0; i < in_len; i++) {
			// Convert dataSet into JSON string data
			cJSON *temp_dataset, *temp_gpspoints;
			cJSON_AddItemToArray(root, temp_dataset = cJSON_CreateObject());
			cJSON_AddNumberToObject(temp_dataset, "size", in[i].size);
			cJSON_AddItemToObject(temp_dataset, "gpspoints", temp_gpspoints = cJSON_CreateArray());
			// Convert gpspoints into JSON string data

			// DEBUG
			printf("beginning for-loop: Convert gpspoints into JSON string data\n");

			int j;
			for (j = 0; j < in[i].size; j++) {
				cJSON *temp_gpspoint;
				cJSON_AddItemToArray(temp_gpspoints, temp_gpspoint = cJSON_CreateObject());
				cJSON_AddItemToObject(temp_gpspoint, "x", cJSON_CreateNumber(in[i].points[j].x));
				cJSON_AddItemToObject(temp_gpspoint, "y", cJSON_CreateNumber(in[i].points[j].y));
			}
		}
		// DEBUG
		printf("finished for loops\n");
	}
	//DEBUG
	printf("Copy JSON string representation to out[]\n");

	// Copy JSON string representation to out[]
	strcat(out, cJSON_Print(root));

	//DEBUG
	printf("Append the sentinel to the end of out\n");

	// Append the sentinel to the end of out
	char sentinel[1] = "";
	sentinel[0] = (char)(SD_SENTINEL);

	//DEBUG
	printf("sentinel[0]: %c\n", sentinel[0]);

	strcat(out, sentinel);

	//DEBUG
	//printf("out[]: %s\n", out);

	//DEBUG
	printf("exiting sd_card_cJSON_stringify\n");

	// Cleanup cJSON data
	// For some reason cJSON_Delete(in_root) causes the program
	// to freeze; this is commented out for now.
	//cJSON_Delete(in_root);
}

void sd_card_cJSON_parse(char data[], dataSet out[])
{
	// Convert JSON string data back into GPSPoint and dataSet structs
	int success = 0;  // set to 0 on successful conversion; otherwise set to 1
	dataSet new_set;  // dataSet retrieved from JSON string data

	cJSON *in_root = cJSON_Parse(data);
	int dataSet_index = 0;
	cJSON *in_dataSet = cJSON_GetArrayItem(in_root, dataSet_index);
	while (in_dataSet != NULL) {
		new_set.size = cJSON_GetObjectItem(in_dataSet, "size")->valuedouble;
		cJSON *gpspoints = cJSON_GetObjectItem(in_dataSet, "gpspoints");
		if (gpspoints != NULL) {
			// Read each gpspoint
			int gpspoint_index = 0;
			cJSON *gpspoint = cJSON_GetArrayItem(gpspoints, gpspoint_index);
			while (gpspoint != NULL) {
				GPSPoint new_pt;
				new_pt.x = cJSON_GetObjectItem(gpspoint, "x")->valuedouble;
				new_pt.y = cJSON_GetObjectItem(gpspoint, "y")->valuedouble;
				new_set.points[gpspoint_index] = new_pt;
				gpspoint_index++;
				gpspoint = cJSON_GetArrayItem(gpspoints, gpspoint_index);
			}
			cJSON_Delete(gpspoint);
		}
		else {
			success = 1;
		}
		out[dataSet_index] = new_set;
		cJSON_Delete(gpspoints);
		dataSet_index++;
		in_dataSet = cJSON_GetArrayItem(in_root, dataSet_index);
	}
	cJSON_Delete(in_dataSet);
	// For some reason cJSON_Delete(in_root) causes the program
	// to freeze; this is commented out for now.
	//cJSON_Delete(in_root);
}

int sd_card_save(dataSet dsets[], int dsets_len, char *filename)
{
	alt_up_sd_card_dev *device_reference = NULL;
	int connected = 0;

	if((device_reference = alt_up_sd_card_open_dev("/dev/Altera_UP_SD_Card_Avalon_Interface_0")) == NULL)
	{
		return -2;
	}
	if (device_reference != NULL ) {
		while (1) {
			if ((connected == 0) && (alt_up_sd_card_is_Present())){
				if (alt_up_sd_card_is_FAT16()) {
					short int file_handle;
					if (alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16()) {
						file_handle = alt_up_sd_card_fopen(filename, false);
						if (file_handle == -1) {
							// File most likely does not exist, so attempt to create it
							file_handle = alt_up_sd_card_fopen(filename, true);
						}
						switch (file_handle) {
						case -1: {
							printf("File NOT Opened (file_handle is -1)\n");
							break;
						}
						case -2: {
							printf("File NOT Opened (file_handle is -2)\n");
							break;
						}
						default: {
							// Convert dsets[] into JSON string

							//DEBUG
							printf("Converting into string\n");

							char json_data[JSON_DATA_MAX_LEN] = "";
							sd_card_cJSON_stringify(dsets, dsets_len, json_data);

							// Write the buffer to the SD card

							//DEBUG
							printf("Writing buffer to sd card\n");

							int i;
							for (i = 0; json_data[i] != '\0'; i++) {
								if (alt_up_sd_card_write(file_handle, (unsigned char)json_data[i]) == false) {
									printf("Error writing to file...\n");
									alt_up_sd_card_fclose(file_handle);
									return -1;
								}
							}

							if (alt_up_sd_card_fclose(file_handle) == false) {
								printf("WARNING: alt_up_sd_card_fclose was unsuccessful\n");
							}
							return 0;
						}
						}
					}
				}
				connected = 1;
				printf("Please disconnect the SD card.\n");
			}
			else if((connected == 1) && (alt_up_sd_card_is_Present() == false)){
				printf("Card disconnected.\n");
				return -3;
			}
		}
	}
	else {
		printf("Can't open device\n");
	}
	// Should not get to this point
	return 1;
}

int sd_card_load(dataSet out[], int out_len, char *filename)
{
	alt_up_sd_card_dev *device_reference = NULL;
	int connected = 0;

	if((device_reference = alt_up_sd_card_open_dev("/dev/Altera_UP_SD_Card_Avalon_Interface_0")) == NULL)
	{
		return -2;
	}

	if (device_reference != NULL ) {
		while (1) {
			if ((connected == 0) && (alt_up_sd_card_is_Present())){
				if (alt_up_sd_card_is_FAT16()) {
					short int file_handle;
					if (alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16()) {
						file_handle = alt_up_sd_card_fopen(filename, false);
						if (file_handle == -1) {
							// File most likely does not exist, so attempt to create it
							file_handle = alt_up_sd_card_fopen(filename, true);
						}
						switch (file_handle) {
						case -1: {
							printf("File NOT Opened (file_handle is -1)\n");
							break;
						}
						case -2: {
							printf("File NOT Opened (file_handle is -2)\n");
							break;
						}
						default: {
							char json_data[JSON_DATA_MAX_LEN] = "";
							// Read data from the SD card into json_data
							short int read = alt_up_sd_card_read(file_handle);
							while (!(read < 0) && read != SD_SENTINEL) {
								char temp[2];
								temp[0] = (char)read;
								temp[1] = '\0';
								strcat(json_data, temp);
								read = alt_up_sd_card_read(file_handle);
							}

							// Generate dataset array from json_data
							// and assign it to out[]
							sd_card_cJSON_parse(json_data, out);

							if (alt_up_sd_card_fclose(file_handle) == false) {
								printf("WARNING: alt_up_sd_card_fclose was unsuccessful\n");
							}
							return 0;
						}
						}
					}
				}
				connected = 1;
				printf("Please disconnect the SD card.\n");
			}
			else if((connected == 1) && (alt_up_sd_card_is_Present() == false)){
				printf("Card disconnected.\n");
				return -3;
			}
		}
	}
	else {
		printf("Can't open device\n");
	}
	// Should not get to this point
	return 1;
}
