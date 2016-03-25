/*
 * graphics.h
 */

#ifndef EXERCISE1_7_H_
#define EXERCISE1_7_H_

#define TRUE 1
#define FALSE 0

#define XRES 800
#define YRES 480
// Heatmap properties
#define HEATMAP_H 10
#define HEATMAP_V 6
#define HM_SHADES 10

// Screen divisions.
#define MENU_TOP 440
#define GOAL_WIDTH 60

#define ASSERT_POINTS_ARE_VALID !!TRUE

// graphics registers all address begin with '8' so as to by pass data cache on NIOS
#define GraphicsCommandReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsStatusReg   		(*(volatile unsigned short int *)(0x84000000))
#define GraphicsX1Reg   			(*(volatile unsigned short int *)(0x84000002))
#define GraphicsY1Reg   			(*(volatile unsigned short int *)(0x84000004))
#define GraphicsX2Reg   			(*(volatile unsigned short int *)(0x84000006))
#define GraphicsY2Reg   			(*(volatile unsigned short int *)(0x84000008))
#define GraphicsColourReg   		(*(volatile unsigned short int *)(0x8400000E))
#define GraphicsBackGroundColourReg   	(*(volatile unsigned short int *)(0x84000010))

// this macro pauses until the graphics chip status register indicates that it is idle
#define WAIT_FOR_GRAPHICS		while((GraphicsStatusReg & 0x0001) != 0x0001);

// values for the graphics command register
#define DrawHLine				1
#define DrawVLine				2
#define DrawLine				3
#define	PutAPixel				0xA
#define	GetAPixel				0xB
#define	ProgramPaletteColour    0x10
#define DrawRectangle			0x11
#define DrawCircle				0x12

// defined constants representing colours pre-programmed into colour palette
// there are 256 colours but only 8 are shown below, we write these to the colour registers
//
// the header files "colours.h" contains constants for all 256 colours
// while the course file ColourPaletteData.c contains the 24 bit RGB data
// that is pre-programmed into the palette
#define	BLACK			0
#define	WHITE			1
#define	RED				2
#define	LIME			3
#define	BLUE			4
#define	YELLOW			5
#define	CYAN			6
#define	MAGENTA			7

#define NCOLOURS 8

//Struct for colour set
typedef struct{
	int menuBackground;
	int text;
	int connectTheDotsLine;
	int pairNum;
	int shades[HM_SHADES];
}Colours;

void WriteAPixel(int x, int y, int Colour);
void WriteHLine(int x1, int y1, int length, int Colour);
void WriteVLine(int x1, int y1, int length, int Colour);
void WriteLine(int x1, int y1, int x2, int y2, int Colour);
int ReadAPixel(int x, int y);
void ProgramPalette(int PaletteNumber, int RGB);
void WriteFilledRectangle(int x1, int y1, int x2, int y2, int color);
void Rectangle(int x1, int y1, int x2, int y2, int color);
void WriteCircle(int x0, int y0, int radius, int color);
void Text(int x, int y, int font_color, int background_color, char *text, int erase);
void Button(int x1, int y1, int x2, int y2, int outline_color, int font_color,
				int fill_color, char *text);
void clear_screen(int colour);

// use these only for testing single-command functions
void TestHLine(int x1, int y1, int length, int Colour);
void TestVLine(int x1, int y1, int length, int Colour);
void TestLine(int x1, int y1, int x2, int y2, int Colour);
void TestFilledRectangle(int x1, int y1, int x2, int y2, int color);
void TestCircle(int x0, int y0, int radius, int color);
void line_test_screen();

#endif /* EXERCISE1_7_H_ */
