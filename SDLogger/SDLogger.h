
#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class SDLogger {
private:
    uint8_t cs;
    String logFileName;
    String configFileName;
    bool spiStarted;

    String getLogFileName(String s);
    byte getLogFileNum(String s);
    bool writeOverConfig(File* confFile, String fileName, byte num);
    bool newConfig(String logName);



public:
    SDLogger(uint8_t);


    bool log(String message);
    bool beginLogFile(String logName);
    
};

#endif