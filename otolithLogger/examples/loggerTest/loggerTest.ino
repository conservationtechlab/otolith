#include <otolithLogger.h>

void setup() {
  uint8_t cs = 4;
  otolithLogger logger = otolithLogger(cs);
  while(!logger.beginLogFile("LOG"));
  logger.log("Hello World");

}

void loop() {
  // put your main code here, to run repeatedly:

}
