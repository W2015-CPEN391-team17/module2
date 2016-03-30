/*
 * Exercise 1.6 for CPEN 391.
 * Connects to the bluetooth dongle and sets its name and password.
 *
 */

#include "bluetooth.h"
#include <stdio.h>
#include <unistd.h>

//call this function at the start of the program before
//attempting to read or write via BT port
void init_bluetooth(void)
{
	//set up 6850 control register to utilize a divide by 16 clock,
	//set RTS low, use 8 bits of data, no parity, 1 stop bit
	//transmitter interrupt disabled
	//program baud rate generator to use 115k baud
	BT_CONTROL = 0x15;
	BT_BAUD = 0x01;
}

// Test if there is data to be received, without actually polling or waiting
int test_getchar(void)
{
	return BT_STATUS & BT_STATUS_RX_MASK;
}

char getchar_btport(void)
{
	//poll Rx bit in 6850 status register. Wait for it to become '1'
	while(1)
	{
		if (BT_STATUS & BT_STATUS_RX_MASK)
		{
			return BT_RXDATA;
		}
	}
}

// Receives a string from the bluetooth dongle into the given buffer through the serial port.
void receive_string(char buffer[], int maxlen)
{
	int i;
	for (i = 0; i < maxlen; i++)
	{
		usleep(100000); //100ms wait
		if (test_getchar())
		{
			buffer[i] = getchar_btport();
			if (buffer[i] == '\0')
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	//DEBUG PRINT
	printf("Received string: %s\n", buffer);
}

char putchar_btport(char c)
{
	//poll Tx bit in 6850 status register. Wait for it to become '1'
	//write 'c' to the 6850 TxData register to output the character
	while(1)
	{
		if (BT_STATUS & BT_STATUS_TX_MASK)
		{
			BT_TXDATA = c;
			break;
		}
	}
	return c;
}

// Sends the given string to the bluetooth dongle through the serial port.
void send_string(char string[], int length)
{
	int i;
	for (i = 0; i < length; i++)
	{
		usleep(100000); //100ms wait
		putchar_btport(string[i]);
	}
}

// Enters command mode, with proper delays
void bt_command_start(void)
{
	usleep(1000000); // 1s wait
	send_string("$$$", 3);
	printf("Bluetooth dongle entered command mode\n");
	usleep(1000000);
}

// Exits command mode, with proper delays
void bt_command_end(void)
{
	usleep(1000000); // 1s wait
	send_string("---\r\n", 5);
	printf("Bluetooth dongle exited command mode");
	usleep(1000000);
}

// Resets the bluetooth dongle to default settings.
void reset_dongle(void)
{
	send_string("SF,", 3);
	send_string("1\r\n", 3);
	printf("Bluetooth dongle reset\n");
}

// Sets the bluetooth dongle's name to the given string.
void set_dongle_name(char name[], int length)
{
	send_string("SN,", 3);
	send_string(name, length);
	send_string("\r\n", 2);
	printf("Bluetooth dongle name set to %s\n", name);
}

// Sets the bluetooth dongle's password to the given string.
void set_dongle_pass(char pass[], int length)
{
	send_string("SP,", 3);
	send_string(pass, length);
	send_string("\r\n", 2);
	printf("Bluetooth dongle pass set to %s\n", pass);
}
