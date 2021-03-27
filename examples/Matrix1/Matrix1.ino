#define LEDMATRIX_CLK 6
#define LEDMATRIX_CS 4
#define LEDMATRIX_DIN 5
#define LEDMATRIX_DEVICES 4

#include <LED_Matrix.h>

const int ANIM_DELAY = 100;

byte sprite[][8]={ 
           {B00011000,
            B00100100,
            B00100100,
            B00011000,
            B01111110,
            B00011000,
            B00100100,
            B01000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B01111100,
            B00011000,
            B01100100,
            B00000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00110100,
            B00000100},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00011000,
            B00011000},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00010100,
            B00010000},

          { B00011000,
            B00100100,
            B00100100,
            B00011000,
            B00111110,
            B01011000,
            B00010100,
            B00010100}
};

LED_Matrix matrix(LEDMATRIX_DIN, LEDMATRIX_CLK, LEDMATRIX_CS, LEDMATRIX_DEVICES);


int                 std_value = 0;
int                 std_value_old = -1;
byte                _std, _min, std_h, std_l, min_h,min_l;

void setup() {
  matrix.begin();
  for (int address=0; address < LEDMATRIX_DEVICES; address++) {
    matrix.displayTest(address, true);
    delay(200);
    matrix.displayTest(address, false);
  }
  matrix.setIntensity(15);
  matrix.printChar(4,'I');
  matrix.printChar(10,'n');
  matrix.printChar(16,'i');
  matrix.printChar(22,'t');
  matrix.display();
  delay(1000);
  
  for (int x1 = 0; x1 < 6; x1++) {
    for (int x2 = 0; x2 < 6; x2++) {

    drawSprite( (byte*)&sprite[x2], x1 * 6 + x2, 0, 8, 8 );
    matrix.display();
    delay(ANIM_DELAY);
    }
  }

  matrix.setIntensity(0x0F);
//  delay(1000);
  matrix.clear();
  _std=0;
  _min=0;
}

void wipeCol(int x) {
   for (int y=0; y<8; y++) {
     matrix.setPixel(x,y,false);  
   }
}

void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  byte mask = B10000000;
  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      matrix.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));
      mask = mask >> 1;
    }
    mask = B10000000;
  }
}

void loop() {
    switch (min_l) {
      case 0:         
      {
          for (int n=0; n<8;n++) {
            matrix.scrollDisplay(matrix.scrollDir::scrollUp);
            matrix.display();
            delay(100);
          }
      }
      break;
      case 1:
      {
          for (int n=0; n<8;n++) {
            matrix.scrollDisplay(matrix.scrollDir::scrollDown);
            matrix.display();
            delay(100);
          }
      }
      break;
      case 2:
      {
          for (int n=0; n< LEDMATRIX_DEVICES * 8;n++) {
            matrix.scrollDisplay(matrix.scrollDir::scrollLeft);
            matrix.display();
            delay(100);
          }
      }
      break;
      case 3:
      {
          for (int n=0; n< LEDMATRIX_DEVICES * 8;n++) {
            matrix.scrollDisplay(matrix.scrollDir::scrollRight);
            matrix.display();
            delay(100);
          }
      }
      break;
      case 4:
      {
          for (int row=0; row<8; row++) {
            for (int col=25; col<31; col++) {
              matrix.setPixel(col,row,false);
              matrix.display();
              delay(10);
            }
          }
      }
      break;
      case 5:
      {
/*          for (int row=7; row>-1; row--) {
            for (int col=25; col<31; col++) {
              matrix.setPixel(col,row,false);
              matrix.display();
              delay(10);
            }
          } */
          matrix.swapChar(25,'6',matrix.scrollDir::scrollUp, 100);
      }
      break;
      case 6:
      {
          matrix.swapChar(25,'7',matrix.scrollDir::scrollDown, 100);
      }
      break;
      case 7:
      {
          matrix.swapChar(25,'8',matrix.scrollDir::scrollLeft, 100);
      }
      break;
      case 8:
      {
          matrix.swapChar(25,'9',matrix.scrollDir::scrollRight, 100);
      }
      break;
      case 9:
      {
          // Ausradieren per Sprite
          for (int x1 = 0; x1 < 6; x1++) {
            for (int x2 = 0; x2 < 6; x2++) {
              drawSprite( (byte*)&sprite[x2], x1 * 6 + x2, 0, 8, 8 );
              matrix.display();
              delay(100);
            }
          }
      }
      break;
    }
    _min++;
    if (_min>59) {
      _std++;
      _min=0;
    }
    if (_std>23) {
      _std=0;
    }
    std_h = _std/10;
    std_l = _std - std_h * 10;
    min_h = _min/10;
    min_l = _min - min_h * 10;
    matrix.printChar(2,'0'+std_h);
    matrix.printChar(8,'0'+std_l);
    matrix.printChar(14,':');
    matrix.printChar(19,'0'+min_h);
    matrix.printChar(25,'0'+min_l);
    matrix.display();
    delay(2000);
}
