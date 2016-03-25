/*
 * graphics.c
 * Functions for the Graphics Controller
 */

#include <stdio.h>
#include "graphics.h"
#include "OutGraphicsCharFont2.h"

/*******************************************************************************************
 * Return TRUE if (x,y) is between (0,0) and (XRES-1,YRES-1)
 * and print an error message and return FALSE otherwise
 ******************************************************************************************/
int check_if_point_is_on_screen(int x, int y) {
	if (x < 0) {
		printf("ERROR: Point not on screen: x value (%d) less than 0\n", x);
		return FALSE;
	} else if (x > XRES-1) {
		printf("ERROR: Point not on screen: x value (%d) greater than XRES-1\n", x);
		return FALSE;
	} else if (y < 0) {
		printf("ERROR: Point not on screen: y value (%d) less than 0\n", y);
		return FALSE;
	} else if (y > YRES-1) {
		printf("ERROR: Point not on screen: y value (%d) greater than YRES-1\n", y);
		return FALSE;
	} else {
		return TRUE;
	}
}

/**********************************************************************************
* Write the entire screen in the given colour
************************************************************************************/
void clear_screen(int colour) {
	WriteFilledRectangle(0, 0, XRES-1, YRES-1, colour);
}

/*******************************************************************************************
* Writes a single pixel to the x,y coords specified using the specified colour
* Note colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
********************************************************************************************/
void WriteAPixel(int x, int y, int Colour)
{
	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x, y)) {
		printf("WriteAPixel failed for (%d,%d)\n", x, y);
		return;
	}

	WAIT_FOR_GRAPHICS;				// is graphics ready for new command

	GraphicsX1Reg = x;				// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;		// set pixel colour
	GraphicsCommandReg = PutAPixel;	// give graphics "write pixel" command
}

/*********************************************************************************************
* Read a single pixel from the x,y coords specified and returns its colour
* Note returned colour is a byte and represents a palette number (0-255) not a 24 bit RGB value
*********************************************************************************************/

int ReadAPixel(int x, int y)
{
	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x, y)) {
		printf("ReadAPixel failed for (%d,%d)\n", x, y);
		return -1;
	}

	WAIT_FOR_GRAPHICS;					// is graphics ready for new command

	GraphicsX1Reg = x;					// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;					// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;	// return the palette number (colour)
}

/*******************************************************************************************
* Write a horizontal line (hardware-accelerated) starting at the x,y coords specified
* of the given length to the right
* Will print an error and return without writing anything if the start/end points are off
* of the screen or if length is negative
********************************************************************************************/
void WriteHLine(int x1, int y1, int length, int Colour)
{
	int x2 = x1 + length;

	if (ASSERT_POINTS_ARE_VALID && length < 0) {
		printf("ERROR: WriteHLine failed because length < 0 (length is %d)\n", length);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: WriteHLine failed for starting point (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x2, y1)) {
		printf("ERROR: WriteHLine failed for ending point (%d,%d)\n", x2, y1);
		return;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsY1Reg = y1;
	GraphicsX2Reg = x2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawHLine;
}

/*******************************************************************************************
* Write a vertical line (hardware-accelerated) starting at the x,y coords specified
* of the given length downwards.
* Will print an error and return without writing anything if the start/end points are off
* of the screen or if length is negative
********************************************************************************************/
void WriteVLine(int x1, int y1, int length, int Colour)
{
	int y2 = y1 + length;

	if (ASSERT_POINTS_ARE_VALID && length < 0) {
		printf("ERROR: WriteVLine failed because length < 0 (length is %d)\n", length);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: WriteVLine failed for starting point (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y2)) {
		printf("ERROR: WriteVLine failed for ending point (%d,%d)\n", x1, y2);
		return;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawVLine;
}

/*******************************************************************************************
* Write a Bresenham line (hardware-accelerated) from x1,y1 to x2,y2
* Will print an error and return without writing anything if the start/end points are off
* of the screen
********************************************************************************************/
void WriteLine(int x1, int y1, int x2, int y2, int Colour)
{
	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: WriteLine failed for starting point (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x2, y2)) {
		printf("ERROR: WriteLine failed for ending point (%d,%d)\n", x2, y2);
		return;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsY1Reg = y1;
	GraphicsX2Reg = x2;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawLine;
}

/**********************************************************************************
* subroutine to program a hardware (graphics chip) palette number with an RGB value
* e.g. ProgramPalette(RED, 0x00FF0000) ;
************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;

    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;        // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                // program green and blue into ls 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour; // issue command
}

/* Shape drawing functions */
/*
 * Draw a non-filled rectangle at a top-left point at (x1, y1) to a
 * bottom-right point at (x2, y2).
 * Preconditions: x1 <= x2 and y1 <= y2
 */
void Rectangle(int x1, int y1, int x2, int y2, int color)
{
	if (ASSERT_POINTS_ARE_VALID && x1 > x2) {
		printf("ERROR: DrawRectangle failed because x1 > x2 (x1 is %d, x2 is %d)\n", x1, x2);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && y1 > y2) {
		printf("ERROR: DrawRectangle failed because y1 > y2 (y1 is %d, y2 is %d)\n", y1, y2);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: DrawRectangle failed for top-left corner (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x2, y2)) {
		printf("ERROR: DrawRectangle failed for bottom-right corner (%d,%d)\n", x2, y2);
		return;
	}

	WriteHLine(x1, y1, x2-x1, color);
	WriteVLine(x1, y1, y2-y1, color);
	WriteHLine(x1, y2, x2-x1+1, color); // the plus one ensures the bottom-right corner is drawn
	WriteVLine(x2, y1, y2-y1, color);
}

/*******************************************************************************************
* Write a filled rectangle (hardware-accelerated) to the screen
* If x1 > x2 or y1 < y2 or if any corner does not fit on the screen, an error message
* will be printed and the function will return without drawing anything
********************************************************************************************/
void WriteFilledRectangle(int x1, int y1, int x2, int y2, int Colour)
{
	if (ASSERT_POINTS_ARE_VALID && x1 > x2) {
		printf("ERROR: WriteFilledRectangle failed because x1 > x2 (x1 is %d, x2 is %d)\n", x1, x2);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && y1 > y2) {
		printf("ERROR: WriteFilledRectangle failed because y1 > y2 (y1 is %d, y2 is %d)\n", y1, y2);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: WriteFilledRectangle failed for top-left corner (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x2, y2)) {
		printf("ERROR: WriteFilledRectangle failed for bottom-right corner (%d,%d)\n", x2, y2);
		return;
	}

	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsY1Reg = y1;
	GraphicsX2Reg = x2;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawRectangle;
}

/*
 * Draw a filled rectangle at a top-left point (x1, y1) to a
 * bottom-right point at (x2, y2).
 * Preconditions: x1 <= x2 and y1 <= y2
 */
void TestFilledRectangle(int x1, int y1, int x2, int y2, int color)
{
	if (ASSERT_POINTS_ARE_VALID && x1 > x2) {
		printf("ERROR: DrawFilledRectangle failed because x1 > x2 (x1 is %d, x2 is %d)\n", x1, x2);
	}

	if (ASSERT_POINTS_ARE_VALID && x1 > x2) {
		printf("ERROR: DrawFilledRectangle failed because y1 > y2 (y1 is %d, y2 is %d)\n", y1, y2);
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x1, y1)) {
		printf("ERROR: DrawFilledRectangle failed for top-left corner (%d,%d)\n", x1, y1);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x2, y2)) {
		printf("ERROR: DrawFilledRectangle failed for bottom-right corner (%d,%d)\n", x2, y2);
		return;
	}

	int i;
	for (i = y1; i <= y2; i++) {
		WriteHLine(x1, i, x2-x1, color);
	}
}

/*
 * Draw a circle (hardware-accelerated)
 * Prints an error message and returns without drawing anything if any points would be off screen
 */
void WriteCircle(int x0, int y0, int radius, int color)
{
#if 0
	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0)) {
		printf("ERROR: DrawCircle failed for center point (%d,%d)\n", x0, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0+radius, y0)) {
		printf("ERROR: DrawCircle failed in positive x direction (%d,%d)\n", x0+radius, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0+radius)) {
		printf("ERROR: DrawCircle failed in positive y direction (%d,%d)\n", x0, y0+radius);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0-radius, y0)) {
		printf("ERROR: DrawCircle failed in negative x direction (%d,%d)\n", x0-radius, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0-radius)) {
		printf("ERROR: DrawCircle failed in negative y direction (%d,%d)\n", x0, y0-radius);
		return;
	}
#endif
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x0;
	GraphicsY1Reg = y0;
	GraphicsX2Reg = radius; //pass in radius using X2 reg
	GraphicsColourReg = color;
	GraphicsCommandReg = DrawCircle;
}

/*
 * Draw a circle (one pixel at a time)
 * Prints an error message and returns without drawing anything if any points would be off screen
 */
void TestCircle(int x0, int y0, int radius, int color)
{
	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0)) {
		printf("ERROR: DrawCircle failed for center point (%d,%d)\n", x0, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0+radius, y0)) {
		printf("ERROR: DrawCircle failed in positive x direction (%d,%d)\n", x0+radius, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0+radius)) {
		printf("ERROR: DrawCircle failed in positive y direction (%d,%d)\n", x0, y0+radius);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0-radius, y0)) {
		printf("ERROR: DrawCircle failed in negative x direction (%d,%d)\n", x0-radius, y0);
		return;
	}

	if (ASSERT_POINTS_ARE_VALID && !check_if_point_is_on_screen(x0, y0-radius)) {
		printf("ERROR: DrawCircle failed in negative y direction (%d,%d)\n", x0, y0-radius);
		return;
	}

	int x = radius;
	int y = 0;
	int decisionOver2 = 1 - x;   // Decision criterion divided by 2 evaluated at x=r, y=0

	while( y <= x ) {
		WriteAPixel( x + x0,  y + y0, color); // Octant 1
		WriteAPixel( y + x0,  x + y0, color); // Octant 2
		WriteAPixel(-x + x0,  y + y0, color); // Octant 4
		WriteAPixel(-y + x0,  x + y0, color); // Octant 3
		WriteAPixel(-x + x0, -y + y0, color); // Octant 5
		WriteAPixel(-y + x0, -x + y0, color); // Octant 6
		WriteAPixel( x + x0, -y + y0, color); // Octant 7
		WriteAPixel( y + x0, -x + y0, color); // Octant 8
		y++;
		if (decisionOver2 <= 0) {
		  decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
		}
		else {
		  x--;
		  decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
		}
	}
}

/*
 * Draw text string on a single line.
 * Preconditions: text != NULL
 * Note: Writing a space character with erase set to true will set all pixels
 * in the character to the background colour
 */
void Text(int x, int y, int font_color, int background_color, char *text, int erase)
{
	const int text_char_x_size = 12;
	if (text != NULL) {
		int i;
		for (i = 0; text[i] != '\0'; i++) {
			  OutGraphicsCharFont2a(x+(text_char_x_size * i), y, font_color, background_color, (int) text[i], erase);
		}
	}
}

/*
 * Draw at a top-left point at (x1, y1) to a bottom-right point at (x2, y2).
 * It is up to the user to ensure that the button dimensions are larger than
 * the text dimensions.
 * Preconditions: x1 <= x2, y1 <= y2 and text != NULL
 */
void Button(int x1, int y1, int x2, int y2, int outline_color, int font_color, int fill_color, char *text)
{
	const int text_padding_x = 6;
	const int text_padding_y = 6;
	WriteFilledRectangle(x1, y1, x2, y2, fill_color);
	Rectangle(x1, y1, x2, y2, outline_color);
	Text(x1+text_padding_x, y1+text_padding_y, font_color, fill_color, text, 1);
}

/*********************************************************************************************
* Draw a horizontal line (1 pixel at a time) starting at the x,y coords specified
* Use for testing only
*********************************************************************************************/

void TestHLine(int x1, int y1, int length, int Colour)
{
	int i;
	for(i = x1; i < x1+length; i++ )
		WriteAPixel(i, y1, Colour);
}

/*********************************************************************************************
* Draw a vertical line (1 pixel at a time) starting at the x,y coords specified
* Use for testing only
*********************************************************************************************/

void TestVLine(int x1, int y1, int length, int Colour)
{
	int i;
	for(i = y1; i < y1+length; i++ )
		WriteAPixel(x1, i, Colour);
}

/*******************************************************************************
* Implementation of Bresenhams line drawing algorithm (1 pixel at a time)
* Use for testing only
*******************************************************************************/

int abs(int a)
{
    if(a < 0)
        return -a ;
    else
        return a ;
}

int sign(int a)
{
    if(a < 0)
        return -1 ;
    else if (a == 0)
        return 0 ;
    else
        return 1 ;
}

void TestLine(int x1, int y1, int x2, int y2, int Colour)
{
    int x = x1;
    int y = y1;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int s1 = sign(x2 - x1);
    int s2 = sign(y2 - y1);
    int i, temp, interchange = 0, error ;

// if x1=x2 and y1=y2 then it is a line of zero length

    if(dx == 0 && dy == 0)
        return ;

 // must be a complex line so use bresenhams algorithm
    else    {

// swap delta x and delta y depending upon slop of line

        if(dy > dx) {
            temp = dx ;
            dx = dy ;
            dy = temp ;
            interchange = 1 ;
        }

// initialise the error term to compensate for non-zero intercept

        error = (dy << 1) - dx ;    // (2 * dy) - dx

// main loop
        for(i = 1; i <= dx; i++)    {
            WriteAPixel(x, y, Colour);

            while(error >= 0)   {
                if(interchange == 1)
                    x += s1 ;
                else
                    y += s2 ;

                error -= (dx << 1) ;    // times 2
            }

            if(interchange == 1)
                y += s2 ;
            else
                x += s1 ;

            error += (dy << 1) ;    // times 2
        }
    }
}

/*******************************************************************************
* Compare functions to check for correctness
* Use for testing only
*******************************************************************************/
void write_test_screen() {
	// write RED lines (software) over the entire screen area
	// if we do hardware-accelerated lines and they fail,
	// the rest of the test will be harder to see
	int i;
	for(i = 0; i <= YRES-1; i++) {
		TestHLine(0, i, XRES-1, RED);
	}

	//each pair of lines (for horizontal/vertical) should have the same length

	//for horizontal lines,
	//test lines should start at both even and odd indexes
	//and have even and odd lengths
	//because they may be drawn incorrectly when drawing two pixels per cycle

	// even start pixel, even length
	TestHLine(100,100,100,LIME);
	TestHLine(100,101,100,LIME);
	TestHLine(100,102,100,LIME);
	TestHLine(100,103,100,LIME);
	TestHLine(100,104,100,LIME);
	WriteHLine(100,105,100,BLUE);
	WriteHLine(100,106,100,BLUE);
	WriteHLine(100,107,100,BLUE);
	WriteHLine(100,108,100,BLUE);
	WriteHLine(100,109,100,BLUE);

	// odd start pixel, even length
	TestHLine(101,120,100,LIME);
	TestHLine(101,121,100,LIME);
	TestHLine(101,122,100,LIME);
	TestHLine(101,123,100,LIME);
	TestHLine(101,124,100,LIME);
	WriteHLine(101,125,100,BLUE);
	WriteHLine(101,126,100,BLUE);
	WriteHLine(101,127,100,BLUE);
	WriteHLine(101,128,100,BLUE);
	WriteHLine(101,129,100,BLUE);

	// even start pixel, odd length
	TestHLine(100,140,101,LIME);
	TestHLine(100,141,101,LIME);
	TestHLine(100,142,101,LIME);
	TestHLine(100,143,101,LIME);
	TestHLine(100,144,101,LIME);
	WriteHLine(100,145,101,BLUE);
	WriteHLine(100,146,101,BLUE);
	WriteHLine(100,147,101,BLUE);
	WriteHLine(100,148,101,BLUE);
	WriteHLine(100,149,101,BLUE);

	// odd start pixel, odd length
	TestHLine(101,160,101,LIME);
	TestHLine(101,161,101,LIME);
	TestHLine(101,162,101,LIME);
	TestHLine(101,163,101,LIME);
	TestHLine(101,164,101,LIME);
	WriteHLine(101,165,101,BLUE);
	WriteHLine(101,166,101,BLUE);
	WriteHLine(101,167,101,BLUE);
	WriteHLine(101,168,101,BLUE);
	WriteHLine(101,169,101,BLUE);

	// test vertical lines
	TestVLine(200,200,100,LIME);
	TestVLine(201,200,100,LIME);
	TestVLine(202,200,100,LIME);
	TestVLine(203,200,100,LIME);
	TestVLine(204,200,100,LIME);
	WriteVLine(205,200,100,BLUE);
	WriteVLine(206,200,100,BLUE);
	WriteVLine(207,200,100,BLUE);
	WriteVLine(208,200,100,BLUE);
	WriteVLine(209,200,100,BLUE);

	// compare bresenham lines
	// should see a black line on top
	// and a purple line a few pixels below it
	// otherwise the hardware-accelerated line doesn't perfectly match

	WriteLine(240,340,440,440,MAGENTA);
	WriteLine(239,339,439,439,MAGENTA);
	TestLine(240,340,440,440,BLACK);
	TestLine(239,339,439,439,BLACK);

	TestLine(250,350,450,450,BLACK);
	TestLine(249,349,449,449,BLACK);
	WriteLine(250,350,450,450,MAGENTA);
	WriteLine(249,349,449,449,MAGENTA);

	// compare rectangles
	// should see WHITE rectangles above
	// and a LIME rectangles below

	// even to even
	WriteFilledRectangle(400, 250, 500, 300, LIME);
	TestFilledRectangle(400, 250, 500, 300, WHITE);

	// even to odd
	WriteFilledRectangle(600, 250, 701, 300, LIME);
	TestFilledRectangle(600, 250, 701, 300, WHITE);

	// odd to even
	TestFilledRectangle(401, 350, 500, 400, WHITE);
	WriteFilledRectangle(401, 350, 500, 400, LIME);

	// odd to odd
	TestFilledRectangle(601, 350, 701, 400, WHITE);
	WriteFilledRectangle(601, 350, 701, 400, LIME);

	// compare circles
	WriteCircle(500, 100, 50, LIME);
	TestCircle(500, 100, 50, WHITE);
	TestCircle(700, 100, 50, WHITE);
	WriteCircle(700, 100, 50, LIME);
}
