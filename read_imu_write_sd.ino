
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>

const int chipSelect = 4;
DFRobot_BMX160 bmx160;
const String FILE_NAME = "imudata.txt";

void setup() {
  delay(100);
  
  // set up SD
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    while (1);
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
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  /* Get a new sensor event */
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  String dataString = "\t null\t null\t";
  dataString += String(Oaccel.x) + "\t" + String(Oaccel.y) + "\t" + String(Oaccel.z) + "\t";
  dataString += String(Ogyro.x) + "\t" + String(Ogyro.y) + "\t" + String(Ogyro.z) + "\t";
  dataString += "\t\t\t\t";
  dataString += String(Omagn.x) + "\t" + String(Omagn.y) + "\t" + String(Omagn.z) + "\t";
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              

  File dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
  
}
