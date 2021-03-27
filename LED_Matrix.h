/*********************************************************************
This is a library for MAX7219/7221 driven LED Matrix Displays

This library is licensed under the GNU GPLv3 (https://www.gnu.org/licenses/gpl.html) open source license.

frameBuffer organisation:
                      DISPLAY (X devices)
------------------------------------------------------------------
| MSB frameBuffer[0] LSB|               | frameBuffer[8*(X-1)+0] |   
|     frameBuffer[1]    |               | frameBuffer[8*(X-1)+1] |   
|     frameBuffer[2]    |               | frameBuffer[8*(X-1)+2] |   
|     frameBuffer[3]    |     ...       | frameBuffer[8*(X-1)+3] |   
|     frameBuffer[4]    |               | frameBuffer[8*(X-1)+4] |   
|     frameBuffer[5]    |               | frameBuffer[8*(X-1)+5] |   
|     frameBuffer[6]    |               | frameBuffer[8*(X-1)+6] |   
|     frameBuffer[7]    |               | frameBuffer[8*(X-1)+7] |   
------------------------------------------------------------------


**********************************************************************/

#ifndef _LED_MATRIX_H_
#define _LED_MATRIX_H_

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

#include <stdlib.h>
#include <Wire.h>
#include "Arduino.h"
#include "LED_Matrix_font.h"

#define MAX7219_NOP          0x00
#define MAX7219_DIGIT0       0x01
#define MAX7219_DIGIT1       0x02
#define MAX7219_DIGIT2       0x03
#define MAX7219_DIGIT3       0x04
#define MAX7219_DIGIT4       0x05
#define MAX7219_DIGIT5       0x06
#define MAX7219_DIGIT6       0x07
#define MAX7219_DIGIT7       0x08
#define MAX7219_DECODE_MODE  0x09
#define MAX7219_INTENSITY    0x0A
#define MAX7219_SCAN_LIMIT   0x0B
#define MAX7219_SHUTDOWN     0x0C
#define MAX7219_DISPLAY_TEST 0x0F

#define FONT_SIZE 6

class LED_Matrix {
	
	public:
    // public variables and typedefinitions

	enum class scrollDir {
			scrollUp = 0,
			scrollDown,
			scrollLeft,
			scrollRight
	};
	
    /*
    The constructor, call:
    LED_MATRIX <Name of object>(<DIN Pin on Arduino>, <CLK Pin on Arduino>, <CS Pin on Arduino>, <Number of Devices>);
    LED_MATRIX matrix(4, 5, 6, 4); // 4 devices daisy chained on port 4...6	
    */  	
    LED_Matrix(int dinPin, int clkPin, int csPin, int numDevices);

    /*
    begin: call it during setup to get all the initialisation stuff done
    */	
	void begin(void);

    /*
	display: displays the content of the framebuffer
	*/
	void display(void);

	/*
	getNumDevices: get the configured number of devices call in constructor
	*/
    int getNumDevices(void);
	
	/*
	setIntensity: sets the intensity of all segments
	intensity can be 0 = minimal to 15 = maximum
    */	
	void setIntensity(byte intensity);

    /*
	clear: Clear the framebuffer
	Dont forget to call display() to see the result!
	*/
	void clear(void);

	/*
	setPixel: set or delete a single pixel.
	val = true ==> set this pixel
	val = false ==> delete this pixel
	
	Matrix counts like this:
	 (0,0)   col ->
	 r ------------------------
	 o |                      |
	 w |                      |
	 | |                      |
	 V ------------------------
                  (8*#Devices-1,7)  
	*/			  
    void setPixel(int col, int row, bool val);
	
	/*
	getPixel: get a pixel from a position
	Result: true if pixel is set, false if not set
	*/
	bool getPixel(int col, int row);
	
    /*
	printChar: prints a single char at position <col>
	*/
	void printChar(int col, char c);

    /*
	swapChar: swaps a single char at position <col> against the new char <c>
	          <delayTime> is the time to wait in ms between the single steps
			  <dir> the direction is one of: scrollUp, scrollDown, scrollLeft, scrollRight
			  call like this in your program:
			  matrix.swapChar(25,'9',matrix.scrollDir::scrollRight, 100);
	*/
	void swapChar(int col, char c, scrollDir dir, int delayTime);
	
	/*
	scrollDisplay: scrolls the display for 1 pixel in the given direction
	Direction is one of: scrollUp, scrollDown, scrollLeft, scrollRight	
	*/
	void scrollDisplay(scrollDir dir);
	
	/*
	scrollDisplayPart: scrolls a part of the display for 1 pixel in the given direction
	Scrollarea starts on column <col> and is <width> pixel width.
	direction is one of: scrollUp, scrollDown, scrollLeft, scrollRight
	*/
	void scrollDisplayPart(int col, int width, scrollDir dir);

    /*
    displayTest: state = true turns on all LED of <deviceNumber>
                 state = false turns it off.
    */				 
	void displayTest(int deviceNumber, bool state);

	//private stuff
	private:
    int _dinPin;
	int _clkPin;
	int _csPin; 
	int _numDevices;
	byte* frameBuffer;
	/*
	write2device: writes a combination of <register><data> to a specific device.
	In a daisy chain it counts like this:
	------------    ------------     ------------     -----------
	| device 0 | -- | device 1 | ... | device N | --- | Arduino |
	------------    ------------     ------------     -----------
    Only the selected device gets the data all other devices get NOP (no operation)
    */	
    void write2device(int deviceNumber, byte max_reg, byte max_data);
	
};

#endif /* _LED_MATRIX_H_ */