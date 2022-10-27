#include <SD.h>
#include <SPI.h>

const int chipSelect = 4;
const String FILE_INIT = "config.txt";
String FILE_NAME = "";

// FILE_WRITE is set to append mode (flag is 0x04) -> check SD documentation, SD.h and SdFat.h
#define FILE_WRITE_ANY FILE_WRITE & 0XFB
 

void setup() {
  if (!SD.begin(chipSelect)){
    while(1);
  }
  if (SD.exists(FILE_INIT)) {
    File logInit = SD.open(FILE_INIT, FILE_WRITE_ANY);
    if (logInit) {
      char buff[16];
      logInit.seek(0);
      int logPos = logInit.position();
      logInit.read(buff, 16);
      String s = String(buff);
      String fName = getFileName(s);
      byte num = getFileNum(s);
      FILE_NAME = fName + num + ".txt";
      num += 1;
      writeOverSD(&logInit, fName, num);
      
      logInit.close();
    }
    
  }
  else {
    File logInit = SD.open(FILE_INIT, FILE_WRITE);
    if (logInit) {
      logInit.println("new,1");
      FILE_NAME = "new0.txt";
      logInit.close();
    }
    
  }

  File data = SD.open(FILE_NAME, FILE_WRITE);
  if (data){
    data.println("initialized");
    data.close();
  }
  
  

}

void loop() {
  // put your main code here, to run repeatedly:

}

String getFileName(String s){
  int i = s.indexOf(',');
  return s.substring(0,i);
}

byte getFileNum(String s){
  int i = s.indexOf(',');
  return (byte) (s.substring(i+1).toInt());
}

void writeOverSD(File* file, String fileName, byte num){
  file->seek(0);
  String fileConfig = String(fileName + "," + String(num));
  //char buff[16];
  //fileConfig.toCharArray(buff, 16);
  file->print(fileConfig);
}
