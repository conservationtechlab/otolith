#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>
#include <SDLogger.h>

// Contants, macros
#define TIMER_INTERVAL_READ       50L
#define TIMER_INTERVAL_WRITE      1000L
#define HW_TIMER_INTERVAL_MS      10
#define CHIP_SELECT               4
#define FILE_NAME                 "OTO"
String  DELIM =                   "\t";

// Globals
long millisStart = 0;
DFRobot_BMX160 bmx160;
SAMDTimer ITimer(TIMER_TC3);
SAMD_ISR_Timer ISR_Timer;
String flushableString = "";
SDLogger logger = NULL;


void setup() {
  delay(100);

  // set up IMU
  if (bmx160.begin() != true){
    digitalWrite(LED_BUILTIN, HIGH);
    while(1);
  }

  // Create SDLogger object to handle file creation and logging
  logger = SDLogger(CHIP_SELECT);
  if (logger.beginLogFile(FILE_NAME) != true){
    digitalWrite(LED_BUILTIN, HIGH);
    while(1);
  }

  // Create header for data log
  String timeStamp = "StartTime: TODO";
  logger.log(timeStamp);
  String fieldHeader = "address" + DELIM + "Time(s)" + DELIM + "ChipTime";
  fieldHeader += DELIM + "ax(m/s^2)" + DELIM + "ay(m/s^2)" + DELIM + "az(m/s^2)";
  fieldHeader += DELIM + "wx(g)" + DELIM + "wy(g)" + DELIM + "wz(g)";
  fieldHeader += DELIM + "AngleX(deg)" + DELIM + "AngleY(deg)" + DELIM + "AngleZ(deg)" + DELIM + "T(°)";
  fieldHeader += DELIM + "hx(uT)" + DELIM + "hy(uT)" + DELIM + "hz(uT)";
  logger.log(fieldHeader);

  // Enable board for interrupts
  if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler)){
    ;
  }

  // Initialize time start for data timestamps
  if (millisStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    millisStart = millis();
  }

  // Begin interrupts
  ISR_Timer.setInterval(TIMER_INTERVAL_READ,  readingImuData);
  ISR_Timer.setInterval(TIMER_INTERVAL_WRITE, uploadingImuData);

  delay(100);
}


void loop() { }


void TimerHandler(void){
  ISR_Timer.run();
}


void readingImuData() {
  flushableString += getDataLog() + "\n";
}


void uploadingImuData(){
  String tempString = flushableString;
  if (tempString.length() == flushableString.length()){
    flushableString = "";  // Without this check, race conditions were more likely
  }
  else {
    uploadingImuData();
    return;
  }
  logger.log(tempString.substring(0,tempString.length()-1)); // shave off the last newline char
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
      //after one month, the date stops being valid -- perhaps include long of time along with human-readable
  
  return yearMonth + String(dateTimeCharArray); 
}
