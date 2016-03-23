#include <stdio.h>
#include <stdlib.h>
#include <Altera_UP_SD_Card_Avalon_Interface.h>
#include <string.h>
#include "sd_card.h"
#include "datasets.h"
#include "conversion.h"

// from datasets.c
extern localDataSets localData;

int sd_card_save(char *filename)
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
							//DEBUG
							printf("Writing buffer to sd card\n");

							// Write the entire struct to the SD card (one byte at a time)
							int size = sizeof(localDataSets);
							int i;
							for (i = 0; i < size; i++) {
								if (alt_up_sd_card_write(file_handle, localData.bytes[i]) == false) {
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

int sd_card_load(char *filename)
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
							// Read the entire struct from the SD card (one byte at a time)
							short int read = alt_up_sd_card_read(file_handle);
							int size = sizeof(localDataSets);
							int i;
							for (i = 0; i < size; i++) {
								read = alt_up_sd_card_read(file_handle);
								if (read < 0) {
									printf("SD card read failure: %d\n", read);
								}
								localData.bytes[i] = (unsigned char)read;
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
