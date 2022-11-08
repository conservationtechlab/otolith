/******************************************************************************************************
 * Purpose: 20 Hz IMU data collection using an Arduino solution for elephant collar
 * Organization: San Diego Zoo Wildlife Association: Conservation Technology Lab
 * Author: Trent Moca 
 * Date: 11/8
 * 
 * This code collects timestamped IMU data at a frequency of 20 Hz for the elephant collar
 * project of the SDZWA Conservation Tech Lab (CTL). Data is collected using an interrupt then 
 * stored on an SD card. The total system uses a SAMD based Arduino board (e.g. MKRWAN1310)
 * with a DFRobotics BMX160 breakout board (https://www.dfrobot.com/product-2141.html) 
 * connected over the I2C headers and an SD card connecte using the SPI headers (or using
 * the SD shield for the MKR series Arduino boards). 
 * 
 * Methods: Data is collected using a 20Hz timer, in which is appended to a String global 
 * variable. Data cannot be immediately added to the SD card because the write operation 
 * takes more than 50 ms. Another interrupt, at 1Hz, handles the write operation, flushing 
 * the global String (flushableString) and writing to the SD card. 
 * 
 */


#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>


#define TIMER_INTERVAL_READ       50L             // 20Hz : IMU Read operation
#define TIMER_INTERVAL_WRITE      1000L           // 1Hz: SD Write operation
#define HW_TIMER_INTERVAL_MS      10
#define CHIP_SELECT               4
#define FILE_NAME                 "intrupt3.txt"
String  DELIM =                   "\t";           //used to change output datatype from 

long millisStart = 0;
DFRobot_BMX160 bmx160;
SAMDTimer ITimer(TIMER_TC3);
SAMD_ISR_Timer ISR_Timer;
String flushableString = "";

void setup() {
  delay(100);

  /* Initialize SD card */
  if (!SD.begin(CHIP_SELECT)) {
    while(1);
  }

  /* Initialize IMU */
  if (bmx160.begin() != true){
    while(1);
  }

  /* Initialize interrupt */
  if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler)){
    ;
  }

  /* Initialize time start with LED blink for data timestamps */
  if (millisStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);   // blink operation allows datalog timestamps to be synced with video timestamp
    digitalWrite(LED_BUILTIN, LOW);
    millisStart = millis();
  }

  /* Begin interrupts for IMU read and SD write operations */
  ISR_Timer.setInterval(TIMER_INTERVAL_READ,  readingImuData);
  ISR_Timer.setInterval(TIMER_INTERVAL_WRITE, uploadingImuData);

  /* Set up header for data file on SD */
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    String timeStamp = "StartTime: TODO";
    String fieldHeader = "address" + DELIM + "Time(s)" + DELIM + "ChipTime";
    fieldHeader += DELIM + "ax(m/s^2)" + DELIM + "ay(m/s^2)" + DELIM + "az(m/s^2)";
    fieldHeader += DELIM + "wx(g)" + DELIM + "wy(g)" + DELIM + "wz(g)";
    fieldHeader += DELIM + "AngleX(deg)" + DELIM + "AngleY(deg)" + DELIM + "AngleZ(deg)" + DELIM + "T(°)";
    fieldHeader += DELIM + "hx(uT)" + DELIM + "hy(uT)" + DELIM + "hz(uT)";
    dataFile.println(timeStamp);
    dataFile.println(fieldHeader);
    dataFile.close();
  }

  delay(100);
}

/* All functionality handled in interrupts */
void loop() { }


void TimerHandler(void){
  ISR_Timer.run();
}


void readingImuData() {
  flushableString += getDataLog() + "\n";
}


void uploadingImuData(){
  String tempString = flushableString;
  /* ensure no new data has been added, and thus no data will be lost--decrease race conditions */
  if (tempString.length() == flushableString.length()){  
    flushableString = "";  
  }
  else {
    uploadingImuData(); 
    return;
  }
  uploadString(tempString);
}


String getDataLog() {
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  String dataString = DELIM + " null" + DELIM + readableDateTime() + DELIM;
  dataString += String(Oaccel.x) + DELIM + String(Oaccel.y) + DELIM + String(Oaccel.z) + DELIM;
  dataString += String(Ogyro.x) + DELIM + String(Ogyro.y) + DELIM + String(Ogyro.z) + DELIM;
  dataString += DELIM + DELIM + DELIM + DELIM;
  dataString += String(Omagn.x) + DELIM + String(Omagn.y) + DELIM + String(Omagn.z) + DELIM; 
  return dataString;
}


void uploadString(String data) {
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.print(data);
    dataFile.close();
  }
}


/**
 * Function converts the running millis() time since time start into a readable time stamp
 * beginning at the epoch: 12:00 AM January 1st 1970. At some point either a library will 
 * want to be made for the CTL to perform time stamp conversion, or an RTC will want to be 
 * integrated with the system.
*/
String readableDateTime() {
  double totalSeconds = (millis() - millisStart)/100.0;
  int totalMinutes = floor(totalSeconds)/60;
  int totalHours = floor(totalSeconds)/3600;
  int totalDays = floor(totalSeconds)/86400;  // add 1 after because January 0th isn't valid
  double secondsDouble = totalSeconds - totalMinutes*60;
  int seconds = floor(secondsDouble);
  int minutes = totalMinutes - totalHours*60;
  int hours = totalHours - totalDays*24;
  String yearMonth = " 1970-01-";
  char dateTimeCharArray[19];
  int deciseconds = round((secondsDouble - seconds)*1000000);
  sprintf(dateTimeCharArray, "%02d %02d:%02d:%02d.%06d", totalDays+1, hours, minutes, seconds, deciseconds);
  return yearMonth + String(dateTimeCharArray); 
}
