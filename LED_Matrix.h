/*********************************************************************
This is a library for MAX7219/7221 driven LED Matrix Displays

This library is licensed under the GNU GPLv3 (https://www.gnu.org/licenses/gpl.html) open source license.

frameBuffer organisation:
                      DISPLAY (X devices)
       -----------------------------------------
       | MSB frameBuffer[0] LSB|               |   
D      |     frameBuffer[1]    |               |
I      |     frameBuffer[2]    |               |
S      |     frameBuffer[3]    |     ...       |
P      |     frameBuffer[4]    |               |==   
L      |     frameBuffer[5]    |               |  | 
A      |     frameBuffer[6]    |               |  | 
Y      |     frameBuffer[7]    |               |  | 
       -----------------------------------------  |
(                                                 |
Y   ================== ... =======================                                                                 
   |                       
d  |  --------------------------------------------
e  |  |                | frameBuffer[8*(X*Y-1)+0] |   
v  |  |                | frameBuffer[8*(X*Y-1)+1] |   
i  |  |                | frameBuffer[8*(X*Y-1)+2] |   
c  ===|      ...       | frameBuffer[8*(X*Y-1)+3] | =====> Arduino/ESP8266/ESP32  
e     |                | frameBuffer[8*(X*Y-1)+4] |   
s     |                | frameBuffer[8*(X*Y-1)+5] |   
)     |                | frameBuffer[8*(X*Y-1)+6] |   
      |                | frameBuffer[8*(X*Y-1)+7] |   
      --------------------------------------------

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


class LED_Matrix : public Print {
	
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
    LED_MATRIX <Name of object>(<DIN Pin on Arduino>, <CLK Pin on Arduino>, <CS Pin on Arduino>, <Number of Devices in X Dir.>, <Number of Devices in Y Dir.>);
    LED_MATRIX matrix(4, 5, 6, 4); // 4 devices daisy chained on port 4...6	
    */  	
    LED_Matrix(unsigned int dinPin, unsigned int clkPin, unsigned int csPin, unsigned int devicesX, unsigned int devicesY);

    /*
    begin: call it during setup to get all the initialisation stuff done
    */	
	void begin(void);

    /*
    on: switches all segments on
    */	
    void on(void);

    /*
    off: switches all segments off
    */	
    void off(void);

    /*
     * setFont: Selects between available fonts.
     * Build in fonts are:
     * 1) 6*7 fonts variable x-size
     * 2) 6*7 fonts fixed x-size
     */
    void setFont(uint8_t font);
    
    /*
	display: displays the content of the framebuffer
	*/
	void display(void);

	/*
	getNumDevices: get the configured number of devices call in constructor
	*/
    unsigned int getNumDevicesX(void);

	/*
	getNumDevices: get the configured number of devices call in constructor
	*/
    unsigned int getNumDevicesY(void);

    /*
	setIntensity: sets the intensity of all segments
	intensity can be 0 = minimal to 15 = maximum
    */	
	void setIntensity(uint8_t intensity);

    /*
	clear: Clear the framebuffer 
	Dont forget to call display() to see the result!
	*/
	void clear(void);

    // clear just the rectangle between (x0,y0) and (x1,y1)
	void clear(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);

    /*
    getSize: Gets the size of a text in pixel
    */
    unsigned int getSize(char* text);
    
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
                  (8*#DevicesX-1,8*#DevicesY-1)  
	*/			  
    void setPixel(unsigned int col, unsigned int row, bool val=true);
	
	/*
	getPixel: get a pixel from a position
	Result: true if pixel is set, false if not set
	*/
	bool getPixel(unsigned int col, unsigned int row);
	
	/*
	scrollDisplay: scrolls the display for 1 pixel in the given direction
	Direction is one of: scrollUp, scrollDown, scrollLeft, scrollRight	
	*/
	void scrollDisplay(scrollDir dir);
	void scrollDisplay(scrollDir dir, unsigned int first, unsigned int last);
	void scrollDisplay(scrollDir dir, unsigned int firstX, unsigned int lastX, unsigned int firstY, unsigned int lastY);
	
    /*
    displayTest: state = true turns on all LED of <deviceNumber>
                 state = false turns it off.
    */				 
	void displayTest(unsigned int deviceNumber, bool state);
    
    /*
    setCursor: sets the Cursor to position x,y => Only used for print command!
    */
    void setCursor(unsigned int col, unsigned int row);
    


	//private stuff
	private:
    int _dinPin;
	int _clkPin;
	int _csPin; 
	unsigned int _numDevices;
    unsigned int _devicesX;
    unsigned int _devicesY;
    uint8_t _intensity;
    uint8_t _font;
    unsigned int  maxCol;
    unsigned int  maxRow;
    unsigned int  aktCol;
    unsigned int  aktRow;
    
	uint8_t* frameBuffer;
	/*
	write2device: writes a combination of <register><data> to a specific device.
	In a daisy chain it counts like this:
	------------    ------------     ------------     -----------
	| device 0 | -- | device 1 | ... | device N | --- | Arduino |
	------------    ------------     ------------     -----------
    Only the selected device gets the data all other devices get NOP (no operation)
    */	
    void write2device(unsigned int deviceNumber, uint8_t max_reg, uint8_t max_data);

    unsigned long getFBaddr(unsigned int col, unsigned int row);
    
    virtual size_t write(uint8_t c);
    
    virtual size_t write(const uint8_t *buffer, size_t size);
};

#endif /* _LED_MATRIX_H_ */
