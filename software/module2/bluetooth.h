/*
 * bluetooth.h
 *
 * Functions for interfacing through bluetooth
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

// Bluetooth parameters
#define DONGLENAME "T17-GEOCACHE"
#define NAMELEN 12
#define DONGLEPASS "1717"
#define PASSLEN 4

// Control registers
#define BT_CONTROL (*(volatile unsigned char *)(0x84000220))
#define BT_STATUS (*(volatile unsigned char *)(0x84000220))
#define BT_TXDATA (*(volatile unsigned char *)(0x84000222))
#define BT_RXDATA (*(volatile unsigned char *)(0x84000222))
#define BT_BAUD (*(volatile unsigned char *)(0x84000224))

#define BT_STATUS_TX_MASK 0x02
#define BT_STATUS_RX_MASK 0x01

void init_bluetooth(void);
int test_getchar(void);
char getchar_btport(void);
char putchar_btport(char c);
void send_string(char string[], int length);
void bt_command_start(void);
void bt_command_end(void);
void reset_dongle(void);
void set_dongle_name(char name[], int length);
void set_dongle_pass(char pass[], int length);

#endif /* BLUETOOTH_H_ */
