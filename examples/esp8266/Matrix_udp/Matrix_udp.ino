//Arduino
//#define LEDMATRIX_CLK 7
//#define LEDMATRIX_CS 6
//#define LEDMATRIX_DIN 5
#define LEDMATRIX_CLK D2
#define LEDMATRIX_CS D3
#define LEDMATRIX_DIN D4
#define LEDMATRIX_DEVICES_X 4
#define LEDMATRIX_DEVICES_Y 3

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LED_Matrix.h>
#include "printf.h"
#include "wifi_secrets.h"

LED_Matrix matrix(LEDMATRIX_DIN, LEDMATRIX_CLK, LEDMATRIX_CS, LEDMATRIX_DEVICES_X, LEDMATRIX_DEVICES_Y);

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char udpdata[10];  // buffer for incoming packets

void setup() {
  matrix.begin();
  matrix.setIntensity(1);
  for (unsigned int address=0; address < LEDMATRIX_DEVICES_X * LEDMATRIX_DEVICES_Y ; address++) {
    matrix.displayTest(address, true);
    delay(200);
    matrix.displayTest(address, false);
  }
  matrix.setIntensity(1);
  matrix.clear();
  Serial.begin(115200);
  Serial.println("Programmstart Matrix_test1");
  printf_begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Udp.begin(localUdpPort);
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("nodeMatrix");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"1234");
  ArduinoOTA.begin();
  matrix.println(WiFi.localIP());
  matrix.display();
  delay(2000);
}

void loop() {
  ArduinoOTA.handle();
  matrix.setIntensity(0);
  int packetSize = Udp.parsePacket();
  // Da ist was da
  if (packetSize) {
    Serial.print("Empfangen "); Serial.print(packetSize);
    Serial.print(" von IP "); Serial.print(Udp.remoteIP());
    Serial.print(" Port "); Serial.println(Udp.remotePort());
    int len = Udp.read(udpdata, 10);
    matrix.clear();
    matrix.setCursor(0,0);
    matrix.print(udpdata);
    matrix.display();
  }    
/*  Serial.println("Loop start");
  matrix.setPixel(0,0,true);
  matrix.setPixel(9,1,true);
  matrix.setPixel(18,2,true);
  matrix.setPixel(27,4,true);
//  matrix.setPixel(10,10,true);
//  matrix.setPixel(x,y,true);
//  matrix.setPixel(x,y,true);
  matrix.display();
  delay(1000);
  Serial.println("Punkte sichtbar");
  
  matrix.clear();
  for(byte x=0; x<5; x++) {
    for (byte y=0; y<5; y++) {
      matrix.setPixel(x,y,true);
    }
  }
  matrix.display();
  delay(5000);
  for(byte i=0; i < LEDMATRIX_DEVICES_X*8-5; i++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollRight);
     matrix.display();
     delay(10);
  }
  for(byte i=0; i < LEDMATRIX_DEVICES_Y*8-5; i++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollDown);
     matrix.display();
     delay(10);
  }
  for(byte i=0; i < LEDMATRIX_DEVICES_X*8-5; i++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollLeft);
     matrix.display();
     delay(10);
  }
  for(byte i=0; i < LEDMATRIX_DEVICES_Y*8-5; i++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollUp);
     matrix.display();
     delay(10);
  }
  delay(1000);
  matrix.clear();
  matrix.display(); 
  matrix.setCursor(0,0);
  matrix.println("Test");
  matrix.print(12.82);
  matrix.display();
  delay(2000);
  for (unsigned int x=0; x < LEDMATRIX_DEVICES_X*8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollRight);
     matrix.scrollDisplay(matrix.scrollDir::scrollDown);
     matrix.display();
     delay(10);
  }
  delay(2000);
  matrix.clear();
  matrix.display();
  matrix.setCursor(0,0);
  matrix.print('H');
  matrix.display();
  matrix.setCursor(6,3);
  matrix.print('a');
  matrix.display();
  matrix.setCursor(12,6);
  matrix.print('l');
  matrix.display();
  matrix.setCursor(18,9);
  matrix.print('l');
  matrix.display();
  matrix.setCursor(24,12);
  matrix.print('o');
  matrix.display();
  delay(2000);
  for (unsigned int x=0; x < LEDMATRIX_DEVICES_X*8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollLeft);
     matrix.scrollDisplay(matrix.scrollDir::scrollDown);
     matrix.display();
     delay(10);
  }
  matrix.clear();
  matrix.setCursor(0,0);
  matrix.print("T: 17°");
  matrix.setCursor(0,16);
  matrix.print("------");
  
  matrix.setCursor(4,8);
  matrix.print(18);
  matrix.setCursor(15,8);
  matrix.print(":");
  matrix.setCursor(19,8);
  matrix.print(25);
  matrix.display();
  delay(2000);
  for (unsigned int x=0; x<8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollDown, 17, 31, 8, 15);
     matrix.display();
     delay(10);
  }
  matrix.setCursor(19,8);
  matrix.print(26);
  matrix.display();
  delay(2000);
  for (unsigned int x=0; x < 8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollDown, 17, 31, 8, 15);
     matrix.display();
     delay(10);
  }
  matrix.setCursor(19,8);
  matrix.print(27);
  matrix.display();
  delay(2000);
  delay(2000);
  for (unsigned int x=0; x<8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollDown, 17, 31, 8, 15);
     matrix.display();
     delay(10);
  }
  matrix.setCursor(19,8);
  matrix.print(28);
  matrix.display();
  delay(2000);
  for (unsigned int x=0; x<8; x++) {
     matrix.scrollDisplay(matrix.scrollDir::scrollDown, 17, 31, 8, 15);
     matrix.display();
     delay(10);
  }
  matrix.setCursor(19,8);
  matrix.print(29);
  matrix.display();
*/  
}
