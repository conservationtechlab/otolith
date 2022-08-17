
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>

const int chipSelect = 4;

const String FILE_NAME = "imudata.txt";

long microsStart = 0;
long logCount = 0;
DFRobot_BMX160 bmx160;

void setup() {
  delay(100);
  
  // set up SD
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    while(1);
  }

  // set up IMU
  if (bmx160.begin() != true){
    while(1);
  }

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

void loop() {
  /* Initialize time start for data timestamps */
  if (microsStart == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    microsStart = micros();
    String dataLog = getDataLog();
    uploadString(dataLog);
    logCount++;
  }

  if ((micros() - microsStart)/100000 >= logCount) {
    String dataLog = getDataLog();
    uploadString(dataLog);
    logCount++;
  }

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
    dataFile.println(data);
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
