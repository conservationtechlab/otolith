#include <otolithLogger.h>


// FILE_WRITE is set to append mode (flag is 0x04) -> check SD documentation, SD.h and SdFat.h
#define FILE_WRITE_ANY FILE_WRITE & 0XFB

otolithLogger::otolithLogger(uint8_t chipS){
    cs = chipS;
    configFileName = "CONF_LOG.txt";
    spiStarted = false;
}

bool otolithLogger::beginLogFile(String logName){

    spiStarted = SD.begin(cs);
    if (!spiStarted){
        return false;
    }

    if (logName.length() > 3){
        logName = logName.substring(0,3);
    }

    configFileName = "CONF_"+ logName + ".txt";

    if (SD.exists(configFileName)) {
        File logInit = SD.open(configFileName, FILE_WRITE_ANY);
        if (logInit) {
            char buff[16];
            logInit.seek(0);
            logInit.read(buff, 16);
            String s = String(buff);
            String fName = getLogFileName(s);
            byte num = getLogFileNum(s);
            logFileName = fName + num + ".txt";
            num += 1;  //append so next file will be 1 after
            bool isNewFile = writeOverConfig(&logInit, fName, num); 
      
            logInit.close();
            return isNewFile;
        }
    }
    else{
        return newConfig(logName);
    }

    return false;

}


bool otolithLogger::log(String message){
    File logFile = SD.open(logFileName, FILE_WRITE);
    if (logFile) {
        logFile.println(message);
        logFile.close();
        return true;
    }
    return false;
}


bool otolithLogger::newConfig(String logName){
    if (!spiStarted){
        return false;
    }

    File logInit = SD.open(configFileName, FILE_WRITE);
    if (logInit) {
      logInit.println(logName+",1");
      logFileName = logName + "0.txt";
      logInit.close();
      return true;
    }
    return false;
    
}


bool otolithLogger::writeOverConfig(File* configFile, String fileName, byte num){
    if (num == 0){  // overflow protection
        return false;
    }
    configFile->seek(0);
    String fileConfig = String(fileName + "," + String(num));
    configFile->print(fileConfig);
    return true;
}


byte otolithLogger::getLogFileNum(String s){
    int i = s.indexOf(',');
    return (byte) (s.substring(i+1).toInt());
}


String otolithLogger::getLogFileName(String s){
    int i = s.indexOf(',');
    return s.substring(0,i);
}
