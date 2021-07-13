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

LED_Matrix::LED_Matrix(unsigned int dinPin, unsigned int clkPin, unsigned int csPin, unsigned int devicesX, unsigned int devicesY) { 
	_dinPin = dinPin;
	_clkPin = clkPin;
	_csPin = csPin;
    _devicesX = devicesX;
    _devicesY = devicesY;
	_numDevices = devicesX * devicesY;
    _font = 1;
    maxRow = devicesY * 8 - 1;
    maxCol = devicesX * 8 - 1;
	frameBuffer = new byte[_numDevices * 8];
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

void LED_Matrix::setFont(byte font) {
    _font = font;
}

void LED_Matrix::setIntensity(byte intensity) {
  _intensity = intensity;  
  for (int address=0; address < _numDevices; address++) {
    write2device(address, MAX7219_INTENSITY, _intensity);
  }	
}

unsigned int LED_Matrix::getNumDevicesX(void) {
	return _devicesX; 
}

unsigned int LED_Matrix::getNumDevicesY(void) {
	return _devicesY; 
}
 
void LED_Matrix::clear(void) {
//  delete framebuffer
    for (int fb_addr=0; fb_addr<(_numDevices*8); fb_addr++) {
       frameBuffer[fb_addr]=0;
    }	
}

void LED_Matrix::clear(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
  for (int address=0; address < _numDevices; address++) {
	displayTest(address, false);  
    write2device(address, MAX7219_SHUTDOWN, 0x01);
    write2device(address, MAX7219_DECODE_MODE, 0x00);
    write2device(address, MAX7219_SCAN_LIMIT, 0x07); 
    write2device(address, MAX7219_INTENSITY, _intensity);
  }  
  for(unsigned int x=x0; x<=x1; x++) {
    for(unsigned int y=y0; y<=y1; y++) {
        setPixel(x,y,false);
    }
  }
}

void LED_Matrix::display(void) {
  for (unsigned int address=0; address < _numDevices; address++) {
    for (byte row=1; row < 9; row++) {
			write2device(address, row, frameBuffer[ (8*address) + row - 1 ]); 
#ifdef ESP8266
            delay(0);
#endif    
	}
  }  
}	

unsigned long LED_Matrix::getFBaddr(unsigned int col, unsigned int row) {
    if ( col <= maxCol && row <= maxRow ) {
        unsigned int segmentX=(unsigned int)col/8;
        unsigned int segmentY=(unsigned int)row/8;
        unsigned int segment=(segmentY*_devicesX)+segmentX;
        byte colInSegment=(segmentX*8)+7-col;
        byte rowInSegment=row-(segmentY*8);
        // frameBuffer address is 8*Segment+rowInSegment
        return (((unsigned int)8*segment+rowInSegment)<<8) | colInSegment;
    } else {
        return 0xFFFFFFFF;
    }
}

void LED_Matrix::setCursor(unsigned int col, unsigned int row) {
    if ( col <= maxCol && row <= maxRow ) {
        aktRow = row;
        aktCol = col;
    }
}

void LED_Matrix::setPixel(unsigned int col, unsigned int row, bool val) {
    unsigned long FB_addr_raw=getFBaddr(col, row);
    if (FB_addr_raw < 0xFFFFFFFF) {
        byte colInSegment = (byte)(FB_addr_raw & 0xFF);
        unsigned int FB_addr = (FB_addr_raw >> 8);
        if (val) {
        // set a pixel
            frameBuffer[FB_addr]=frameBuffer[FB_addr] | (0x01 << colInSegment);
        } else {
        //delete a pixel	
            frameBuffer[FB_addr]=frameBuffer[FB_addr] & ~(0x01 << colInSegment);
        }
    }
}

bool LED_Matrix::getPixel(unsigned int col, unsigned int row) {
    unsigned int FB_addr_raw=getFBaddr(col, row);
    if (FB_addr_raw < 0xFFFFFFFF) {
        byte colInSegment = (byte)(FB_addr_raw & 0x000000FF);
        unsigned int FB_addr = (unsigned int)(FB_addr_raw >> 8);
        return frameBuffer[FB_addr] & (0x01 << colInSegment);
    }
}

size_t LED_Matrix::write(const uint8_t *buffer, size_t size) {
    for (int i=0; i<size; i++) {
        write(buffer[i]);
    }
}

size_t LED_Matrix::write(uint8_t c) {
    size_t retval=0;
	byte res;
	byte mybit;
    byte letter_size;
    byte font_size;
	// CF und LF sorgen für Zeilenumbruch
//	if (c == 10 || c == 13) {
	if (c == 10 ) {
        setCursor(0,aktRow+8);
    }
	if (c > 31 && c < 127) {
        letter_size = pgm_read_byte(&myFont7x6[c - 32][FONT_SIZE-1]);
        switch (_font) {
            case 1:    
                font_size = letter_size;
                break;
            case 2:
                font_size = 5;
                break;
        }
		retval = 1;
		if (aktCol <= maxCol && aktRow <= maxRow) {
            if (letter_size < 4) {
                for ( int srow=0; srow < 8; srow++) {
                    setPixel(aktCol, aktRow + srow, false); 	
                }                    
                aktCol++;
                font_size--;
            }                
            if (letter_size < 3) {
                for ( int srow=0; srow < 8; srow++) {
                    setPixel(aktCol, aktRow + srow, false); 	
                }                    
                aktCol++;
                font_size--;
            }                
            for ( int i=0; i < letter_size; i++) {
                if (i < letter_size) {
                    res=pgm_read_byte(&myFont7x6[c - 32][i]);
                } else {
                    res=0;
                }
                mybit=0x01;
                for ( int srow=0; srow < 8; srow++) {
                    setPixel(aktCol+i, aktRow + srow, res & ( mybit << srow)); 	
                }                    
            }
        }
        aktCol = aktCol + font_size + 1;
        if ( aktCol >= maxCol ) {
            aktCol=0;
            aktRow=aktRow+8;
        }
	}
#ifdef ESP8266
    delay(0);
#endif    
	return retval;
}

void LED_Matrix::scrollDisplay(scrollDir dir) {
	switch (dir) {
		case scrollDir::scrollUp:
		case scrollDir::scrollDown:
            scrollDisplay(dir, 0, maxCol);
		break;		
		case scrollDir::scrollLeft:
		case scrollDir::scrollRight:
            scrollDisplay(dir, 0, maxRow);
        break;
    }
}

void LED_Matrix::scrollDisplay(scrollDir dir, unsigned int first, unsigned int last) {
	switch (dir) {
		case scrollDir::scrollUp:
		case scrollDir::scrollDown:
            scrollDisplay(dir, first, 0, last, maxRow);
		break;		
		case scrollDir::scrollLeft:
		case scrollDir::scrollRight:
            scrollDisplay(dir, 0, first, maxCol, last);
        break;
    }
}

void LED_Matrix::scrollDisplay(scrollDir dir, unsigned int firstX, unsigned int firstY, unsigned int lastX, unsigned int lastY) {
    if (firstX > maxCol) firstX = maxCol;
    if (lastX > maxCol) lastX = maxCol;
    if (firstY > maxRow) firstY = maxRow;
    if (lastY > maxRow) lastY = maxRow;
	switch (dir) {
		case scrollDir::scrollUp:
		{
            for (unsigned int y = firstY; y <= lastY ; y++) {
                for (unsigned int x = firstX; x <= lastX; x++) {
                    setPixel(x,y,getPixel(x,y+1));
#ifdef ESP8266
                    delay(0);
#endif    
                }
			}
            for (unsigned int x = firstX; x <= lastX; x++) {
                setPixel(x,lastY,false);
            }
		}
		break;		
		case scrollDir::scrollDown:
		{
            for (unsigned int y = lastY; y > firstY; y--) {
                for (unsigned int x = firstX; x <= lastX; x++) {
                    setPixel(x,y,getPixel(x,y-1));
#ifdef ESP8266
                    delay(0);
#endif    
                }
			}
            for (unsigned int x=firstX; x <= lastX; x++) {
                setPixel(x,firstY,false);
            }
		}
		break;		
		case scrollDir::scrollLeft:
		{
            for (unsigned int y = firstY; y <= lastY; y++) {
                for (unsigned int x = firstX; x < lastX; x++) {
                    setPixel(x,y,getPixel(x+1,y));
#ifdef ESP8266
                    delay(0);
#endif    
                }
			}
            for (unsigned int y=firstY; y <= lastY; y++) {
                setPixel(lastX,y,false);
            }
		}
		break;		
		case scrollDir::scrollRight:
		{
            for (unsigned int y = firstY; y <= lastY; y++) {
                for (unsigned int x = lastX; x > firstX; x--) {
                    setPixel(x,y,getPixel(x-1,y));
#ifdef ESP8266
                    delay(0);
#endif    
                }
			}
            for (unsigned int y = firstY; y <= lastY; y++) {
                setPixel(firstX,y,false);
            }
		}
		break;		
	}
}


void LED_Matrix::displayTest(unsigned int deviceNumber, bool state) {
  if (state) {	
	write2device(deviceNumber, MAX7219_DISPLAY_TEST, 0x01);	
  } else {
	write2device(deviceNumber, MAX7219_DISPLAY_TEST, 0x00);	
  }
}

void LED_Matrix::write2device(unsigned int deviceNumber, byte max_reg, byte max_data) {
   digitalWrite(_csPin, LOW);
   for (unsigned int address=0; address < _numDevices; address++) {
     if ( address == deviceNumber) {
       shiftOut(_dinPin, _clkPin, MSBFIRST, max_reg);
       shiftOut(_dinPin, _clkPin, MSBFIRST, max_data);      
     } else {
       shiftOut(_dinPin, _clkPin, MSBFIRST, MAX7219_NOP);
       shiftOut(_dinPin, _clkPin, MSBFIRST, 0x00);      
     }
   }
//   delay(1); 
   digitalWrite(_csPin, HIGH);
}

