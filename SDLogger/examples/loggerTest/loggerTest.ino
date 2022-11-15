#include <SDLogger.h>

void setup() {
  uint8_t cs = 4;
  SDLogger logger = SDLogger(cs);
  while(!logger.beginLogFile("LOG"));
  logger.log("Hello World");

}

void loop() {
  // put your main code here, to run repeatedly:

}
