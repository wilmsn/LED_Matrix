/*
 * LED_Matrix.cpp
 *
 *  Created on: 28.11.2018
 *      Author: Norbert Wilms
 *
 * This is a library for MAX7219 driven LED Matrix Displays
 * More details in header file!
 *
*/

#include "LED_Matrix.h"

LED_Matrix::LED_Matrix(int dinPin, int clkPin, int csPin, int numDevices) { 
	_dinPin=dinPin;
	_clkPin=clkPin;
	_csPin=csPin;
	_numDevices=numDevices;
	frameBuffer = new byte[_numDevices*8];
	pinMode(_clkPin, OUTPUT);
    pinMode(_csPin, OUTPUT);
    pinMode(_dinPin, OUTPUT);
    digitalWrite(_clkPin, HIGH);
    digitalWrite(_csPin, HIGH);
    digitalWrite(_dinPin, HIGH);
}

void LED_Matrix::begin(void) {
  for (int address=0; address < _numDevices; address++) {
	displayTest(address, false);  
    write2device(address, MAX7219_SHUTDOWN, 0x01);
    write2device(address, MAX7219_DECODE_MODE, 0x00);
    write2device(address, MAX7219_SCAN_LIMIT, 0x07); 
    write2device(address, MAX7219_INTENSITY, 0x03);
//  clear the display
  }  
  clear();
  display();
}

void LED_Matrix::setIntensity(byte intensity) {
  for (int address=0; address < _numDevices; address++) {
    write2device(address, MAX7219_INTENSITY, intensity);
  }	
}

int LED_Matrix::getNumDevices(void) {
	return _numDevices; 
}
 
void LED_Matrix::clear(void) {
//  delete framebuffer
    for (int fb_addr=0; fb_addr<(_numDevices*8); fb_addr++) {
       frameBuffer[fb_addr]=0;
    }	
}

void LED_Matrix::display(void) {

  for (int address=0; address < _numDevices; address++) {
    for (byte row=1; row < 9; row++) {
			write2device(address, row, frameBuffer[ (8*address) + row - 1 ]); 
	}
  }  
}	

void LED_Matrix::setPixel(int col, int row, bool val) {
	int address=(int)col/8;
	byte colInSegment=(address*8)+7-col;
	// frameBuffer address is 8*address+row
	if (val) {
	// set a pixel
		frameBuffer[8*address+row]=frameBuffer[8*address+row] | 0x01 << colInSegment;
	} else {
	//delete a pixel	
		frameBuffer[8*address+row]=frameBuffer[8*address+row] & ~(0x01 << colInSegment);
	}			
}

bool LED_Matrix::getPixel(int col, int row) {
	int address=(int)col/8;
	byte colInSegment=(address*8)+7-col;
	// frameBuffer address is 8*address+row
	return	frameBuffer[8*address+row] & (0x01 << colInSegment);	
}

void LED_Matrix::printChar(int col, char c) {
	byte res;
	byte mybit;
	for ( int i=0; i < FONT_SIZE; i++) {
		res=pgm_read_byte(&myFont7x6[c - 32][i]);
		mybit=0x01;
		for ( int row=0; row < 8; row++) {
			setPixel(col+i, row, res & ( mybit << row)); 	
		}
	}
}

void LED_Matrix::swapChar(int col, char c, scrollDir dir, int delayTime) {
	byte res;
	byte mybit;
	switch (dir) {
		case scrollDir::scrollUp:
		{
			for (int row=0; row < 8; row++) {
				scrollDisplayPart(col, FONT_SIZE, dir);
				for ( int i=0; i < FONT_SIZE; i++) {
					res=pgm_read_byte(&myFont7x6[c - 32][i]);
					mybit=0x01;
					setPixel(col+i, 7, res & ( mybit << row)); 	
				}
				display();
				delay(delayTime);
			}
		}
		break;		
		case scrollDir::scrollDown:
		{
			for (int row=7; row > -1; row--) {
				scrollDisplayPart(col, FONT_SIZE, dir);
				for ( int i=0; i < FONT_SIZE; i++) {
					res=pgm_read_byte(&myFont7x6[c - 32][i]);
					mybit=0x01;
					setPixel(col+i, 0, res & ( mybit << row)); 	
				}
				display();
				delay(delayTime);
			}
		}
		break;		
		case scrollDir::scrollLeft:
		{
			for ( int i= FONT_SIZE -1; i > -1; i--) {
				scrollDisplayPart(col, FONT_SIZE, dir);
				res=pgm_read_byte(&myFont7x6[c - 32][i]);
				for (int row=0; row<8; row++) {
					mybit=0x01;
					setPixel(col + FONT_SIZE -1, row, res & ( mybit << row)); 	
				}
				display();
				delay(delayTime);
			}
		}
		break;
		case scrollDir::scrollRight:
		{
			for ( int i= FONT_SIZE -1; i > -1; i--) {
				scrollDisplayPart(col, FONT_SIZE, dir);
				res=pgm_read_byte(&myFont7x6[c - 32][i]);
				for (int row=0; row<8; row++) {
					mybit=0x01;
					setPixel(col, row, res & ( mybit << row)); 	
				}
				display();
				delay(delayTime);
			}
		}
		break;
	}		
}
	
void LED_Matrix::scrollDisplay(LED_Matrix::scrollDir dir) {
	switch (dir) {
		case scrollDir::scrollUp:
		{
			for (int address=0; address < _numDevices; address++) {
				for (int row=0; row < 7; row++) {
					frameBuffer[8*address+row]=frameBuffer[8*address+row+1];
				}
				frameBuffer[8*address+7]=0x00;
			}
		}
		break;		
		case scrollDir::scrollDown:
		{
			for (int address=0; address < _numDevices; address++) {
				for (int row=6; row > -1; row--) {
					frameBuffer[8*address+row+1]=frameBuffer[8*address+row];
				}
				frameBuffer[8*address+0]=0x00;
			}
		}
		break;		
		case scrollDir::scrollLeft:
		case scrollDir::scrollRight:
			scrollDisplayPart(0, 8*_numDevices-1, dir);
		break;
	}
}

void LED_Matrix::scrollDisplayPart(int col, int width, LED_Matrix::scrollDir dir) {
	switch (dir) {
		case scrollDir::scrollUp:
		{
			for (int _col=col; _col < col+width; _col++) {
				for (int _row=0; _row < 7; _row++) {
					setPixel(_col,_row,getPixel(_col,_row+1));
				}
			}
		}
		break;		
		case scrollDir::scrollDown:
		{
			for (int _col=col; _col < col+width; _col++) {
				for (int _row=7; _row > 0; _row--) {
					setPixel(_col,_row,getPixel(_col,_row-1));
				}
			}
		}
		break;		
		case scrollDir::scrollLeft:
		{
			for (int _col=col; _col < col+width; _col++) {
				for (int _row=0; _row < 8; _row++) {
					setPixel(_col,_row,getPixel(_col+1,_row));
				}
			}
		}
		break;
		case scrollDir::scrollRight:
		{
			for (int _col=col+width-1; _col > col-1; _col--) {
				for (int _row=0; _row < 8; _row++) {
					setPixel(_col,_row,getPixel(_col-1,_row));
				}
			}
		}
		break;
	}	
}

void LED_Matrix::displayTest(int deviceNumber, bool state) {
  if (state) {	
	write2device(deviceNumber, MAX7219_DISPLAY_TEST, 0x01);	
  } else {
	write2device(deviceNumber, MAX7219_DISPLAY_TEST, 0x00);	
  }
}

void LED_Matrix::write2device(int deviceNumber, byte max_reg, byte max_data) {
  digitalWrite(_csPin, LOW);
   for (int address=0; address < _numDevices; address++) {
     if ( address == deviceNumber) {
       shiftOut(_dinPin, _clkPin, MSBFIRST, max_reg);
       shiftOut(_dinPin, _clkPin, MSBFIRST, max_data);      
     } else {
       shiftOut(_dinPin, _clkPin, MSBFIRST, MAX7219_NOP);
       shiftOut(_dinPin, _clkPin, MSBFIRST, 0x00);      
     }
   }
  digitalWrite(_csPin, HIGH);
}
