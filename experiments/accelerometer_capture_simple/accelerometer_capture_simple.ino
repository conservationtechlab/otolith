#include <DFRobot_BMX160.h>
#include <SDLogger.h>
#include <SPI.h>

#define CHIP_SELECT 4
#define FILE_NAME "OTO"
String DELIM = ",";

DFRobot_BMX160 bmx160;
String flushableString = "\n";
SDLogger logger = NULL;

void setup() {
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, LOW);

  if (bmx160.begin() != true) {
    digitalWrite(LED_BUILTIN, HIGH);
    while (1)
      ;
  }

  logger = SDLogger(CHIP_SELECT);
  if (logger.beginLogFile(FILE_NAME) != true) {
    digitalWrite(LED_BUILTIN, HIGH);
    while (1)
      ;
  }

  String fieldHeader = "Time(ms)";
  fieldHeader +=
      DELIM + "ax(m/s^2)" + DELIM + "ay(m/s^2)" + DELIM + "az(m/s^2)";
  fieldHeader += DELIM + "wx(g)" + DELIM + "wy(g)" + DELIM + "wz(g)";
  fieldHeader += DELIM + "hx(uT)" + DELIM + "hy(uT)" + DELIM + "hz(uT)";
  logger.log(fieldHeader);
}

String getDataLog() {
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

  String dataString = millis() + DELIM;
  dataString += String(Oaccel.x) + DELIM + String(Oaccel.y) + DELIM +
                String(Oaccel.z) + DELIM;
  dataString += String(Ogyro.x) + DELIM + String(Ogyro.y) + DELIM +
                String(Ogyro.z) + DELIM;
  dataString +=
      String(Omagn.x) + DELIM + String(Omagn.y) + DELIM + String(Omagn.z);
  return dataString;
}

void loop() {
  for (int i = 0; i < 100; i++) {
    flushableString += getDataLog() + "\n";
    delay(45);
  }
  logger.log(flushableString);
  flushableString = "";
}
