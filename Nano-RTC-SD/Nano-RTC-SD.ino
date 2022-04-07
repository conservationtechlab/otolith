/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */

#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <Arduino_LSM9DS1.h>
#include "RTClib.h"

RTC_DS3231 rtc;

const int chipSelect = 10; 

File datafile;

float x, y, z;

int xbuffer [25];
int ybuffer [25];
int zbuffer [25];

int tracker = 25;


void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }
  
  if(!SD.begin(10)){
    Serial.println("SD initialization failed!");
    while (1);
  }
    
  //open file
  datafile=SD.open("DATA.txt", FILE_WRITE);
  if (datafile) {
    Serial.println("File opened ok");
    datafile.println("Date,Time,X,Y,Z,");
  }
  datafile.close();

  rtc.adjust(DateTime(2022, 4, 1, 12, 00, 0));
  DateTime now = rtc.now();
}

void addbuffer(){
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
  }
  xbuffer[tracker] = x;
  ybuffer[tracker] = y; 
  zbuffer[tracker] = z;
}


void savelog(DateTime now) {
  //Serial.println("save");
  datafile = SD.open("DATA.txt", FILE_WRITE);
  if (datafile) {
    for(int i=0; i < 25; i++){
      datafile.print(now.year(), DEC);
      datafile.print('/');
      datafile.print(now.month(), DEC);
      datafile.print('/');
      datafile.print(now.day(), DEC);
      datafile.print(',');
      datafile.print(now.hour(), DEC);
      datafile.print(':');
      datafile.print(now.minute(), DEC);
      datafile.print(':');
      datafile.print(now.second(), DEC);
      datafile.print(",");
      datafile.print(xbuffer[i]);
      datafile.print(",");
      datafile.print(ybuffer[i]);
      datafile.print(",");
      datafile.println(zbuffer[i]);
    }
  }
  datafile.close();
}

void loop() {
  addbuffer();
  if(tracker < 25){
    tracker += 1;
    delay(40);
  }
  else{
    DateTime now = rtc.now();
    savelog(now);
    tracker = 0;    
   }
}
