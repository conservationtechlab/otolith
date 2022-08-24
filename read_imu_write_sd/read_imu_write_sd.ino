#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>


#define TIMER_INTERVAL_20HZ       50L
#define TIMER_INTERVAL_1HZ        1000L
#define HW_TIMER_INTERVAL_MS      10
#define CHIP_SELECT               4
#define FILE_NAME                 "intrupt1.txt"

long microsStart = 0;
DFRobot_BMX160 bmx160;
SAMDTimer ITimer(TIMER_TC3);
SAMD_ISR_Timer ISR_Timer;
String flushableString = "";

void setup() {
  delay(100);
  
  // set up SD
  // see if the card is present and can be initialized:
  if (!SD.begin(CHIP_SELECT)) {
    while(1);
  }

  // set up IMU
  if (bmx160.begin() != true){
    while(1);
  }

  if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler)){
    ;
  }
  ISR_Timer.setInterval(TIMER_INTERVAL_20HZ,  readingImuData);
  ISR_Timer.setInterval(TIMER_INTERVAL_1HZ,  uploadingImuData);

  // open file
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    String timeStamp = "StartTime: TODO";
    String fieldHeader = "address\tTime(s)\tChipTime";
    fieldHeader += "\tax(m/s^2)\tay(m/s^2)\taz(m/s^2)";
    fieldHeader += "\twx(g)\twy(g)\twz(g)";
    fieldHeader += "\tAngleX(deg)\tAngleY(deg)\tAngleZ(deg)\tT(°)";
    fieldHeader += "\thx(uT)\thy(uT)\thz(uT)";
    dataFile.println(timeStamp);
    dataFile.println(fieldHeader);
    dataFile.close();
  }

  delay(100);
}

void loop() { }


void TimerHandler(void){
  ISR_Timer.run();
}


void readingImuData() {
  /* Initialize time start for data timestamps */
  if (microsStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    microsStart = micros();
  }
  flushableString += getDataLog() + "\n";
}


void uploadingImuData(){
  String tempString = flushableString;
  if (tempString.length() == flushableString.length()){
    flushableString = "";
  }
  else {
    uploadingImuData();
    return;
  }
  digitalWrite(LED_BUILTIN, HIGH);
  uploadString(tempString);
  digitalWrite(LED_BUILTIN, LOW);
}


String getDataLog() {
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  String dataString = "\t null\t" + readableDateTime() + "\t";
  dataString += String(Oaccel.x) + "\t" + String(Oaccel.y) + "\t" + String(Oaccel.z) + "\t";
  dataString += String(Ogyro.x) + "\t" + String(Ogyro.y) + "\t" + String(Ogyro.z) + "\t";
  dataString += "\t\t\t\t";
  dataString += String(Omagn.x) + "\t" + String(Omagn.y) + "\t" + String(Omagn.z) + "\t"; 
  return dataString;
}


void uploadString(String data) {
  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.print(data);
    dataFile.close();
  }
}


String readableDateTime() {
  double totalSeconds = (micros() - microsStart)/1000000.0;
  int totalMinutes = floor(totalSeconds)/60;
  int totalHours = floor(totalSeconds)/3600;
  int totalDays = floor(totalSeconds)/86400;
  double secondsDouble = totalSeconds - totalMinutes*60;
  int seconds = floor(secondsDouble);
  int minutes = totalMinutes - totalHours*60;
  int hours = totalHours - totalDays*24;
  String yearMonth = " 1970-01-";
  char dateTimeCharArray[19];
  int deciseconds = round((secondsDouble - seconds)*1000000);
  sprintf(dateTimeCharArray, "%02d %02d:%02d:%02d.%06d", totalDays, hours, minutes, seconds, deciseconds);
  
  return yearMonth + String(dateTimeCharArray); 
}
