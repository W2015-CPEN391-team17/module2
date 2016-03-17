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
void command_start(void)
{
	usleep(1000000); // 1s wait
	send_string("$$$", 3);
	usleep(1000000);
}

// Exits command mode, with proper delays
void command_end(void)
{
	usleep(1000000); // 1s wait
	send_string("---\r\n", 5);
	usleep(1000000);
}

// Resets the bluetooth dongle to default settings.
void reset_dongle(void)
{
	command_start();
	send_string("SF,", 3);
	send_string("1\r\n", 3);
	command_end();
}

// Sets the bluetooth dongle's name to the given string.
void set_dongle_name(char name[], int length)
{
	command_start();
	send_string("SN,", 3);
	send_string(name, length);
	send_string("\r\n", 2);
	command_end();
}

// Sets the bluetooth dongle's password to the given string.
void set_dongle_pass(char pass[], int length)
{
	command_start();
	send_string("SP,", 3);
	send_string(pass, length);
	send_string("\r\n", 2);
	command_end();
}
