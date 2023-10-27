/* 
 *  This is a simplified version for reading IMU and writing to SD card.
*/

/////////////////////////////////////////////////////////////////

// These define's must be placed at the beginning before #include "SAMDTimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select only one to be true for SAMD21. Must must be placed at the beginning before #include "SAMDTimerInterrupt.h"
#define USING_TIMER_TC3         true      // Only TC3 can be used for SAMD51
#define USING_TIMER_TC4         false     // Not to use with Servo library
#define USING_TIMER_TC5         false
#define USING_TIMER_TCC         false
#define USING_TIMER_TCC1        false
#define USING_TIMER_TCC2        false     // Don't use this, can crash on some boards
   
#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <DFRobot_BMX160.h>
#include <TCA9548A.h>
#include <RTClib.h>

// TC3, TC4, TC5 max permissible HW_TIMER_INTERVAL_MS is 1398.101 ms, larger will overflow, therefore not permitted
// Use TCC, TCC1, TCC2 for longer HW_TIMER_INTERVAL_MS
#define HW_TIMER_INTERVAL_MS      10
#define ChipSelect      4

#if (TIMER_INTERRUPT_USING_SAMD21)

  #if USING_TIMER_TC3
    #define SELECTED_TIMER      TIMER_TC3
  #elif USING_TIMER_TC4
    #define SELECTED_TIMER      TIMER_TC4
  #elif USING_TIMER_TC5
    #define SELECTED_TIMER      TIMER_TC5
  #elif USING_TIMER_TCC
    #define SELECTED_TIMER      TIMER_TCC
  #elif USING_TIMER_TCC1
    #define SELECTED_TIMER      TIMER_TCC1
  #elif USING_TIMER_TCC2
    #define SELECTED_TIMER      TIMER_TCC
  #else
    #error You have to select 1 Timer  
  #endif

#else
  #if !(USING_TIMER_TC3)
    #error You must select TC3 for SAMD51
  #endif
  #define SELECTED_TIMER      TIMER_TC3
#endif  

// Init selected SAMD timer
SAMDTimer ITimer(SELECTED_TIMER);
DFRobot_BMX160 bmx160;
TCA9548A I2CMux;
RTC_DS3231 rtc; // Create an instance of the DS3231 RTC
////////////////////////////////////////////////

// Init SAMD_ISR_Timer
// Each SAMD_ISR_Timer can service 16 different ISR-based timers
SAMD_ISR_Timer ISR_Timer;

// Init a flashable string
String flushableString = "";
String DELIM = ",";
DateTime now;

#define TIMER_INTERVAL_50MS             50L
#define TIMER_INTERVAL_1S             1000L
#define IMU_CHANNEL                      2
#define RTC_CHANNEL                      3

void TimerHandler(void)
{
  ISR_Timer.run();
}

// Get the IMU data from the 9-axis sensor and log it onto the SD card
void GetIMUData() {
    // get the RTC time
    I2CMux.openChannel(RTC_CHANNEL);
    now = rtc.now();
    flushableString += String(millis()) + DELIM;
    Serial.println(String(millis()));
    String formattedDateTime = twoDigits(now.hour()) + ":" + twoDigits(now.minute()) + ":" + twoDigits(now.second()) + DELIM;
    flushableString += formattedDateTime;
    I2CMux.closeChannel(RTC_CHANNEL);

    // get the IMU data
    I2CMux.openChannel(IMU_CHANNEL);
    sBmx160SensorData_t Mag, Gyro, Accel;
    bmx160.getAllData(&Mag, &Gyro, &Accel);

    flushableString += String(Accel.x) + DELIM + String(Accel.y) + DELIM + String(Accel.z) + DELIM; // write 3 axis accelerometer
    flushableString += String(Gyro.x) + DELIM + String(Gyro.y) + DELIM + String(Gyro.z) + DELIM;  // write 3 axis gyroscope
    flushableString += String(Mag.x) + DELIM + String(Mag.y) + DELIM + String(Mag.z) + "\n";  // write 3 axis magnetometer
    
    I2CMux.closeChannel(IMU_CHANNEL);
}

// Write the IMU data to the SD card
void WriteIMUData() {
    String fileName = twoDigits(now.year() % 100) + twoDigits(now.month()) + twoDigits(now.day()) + ".txt";
    File dataFile = SD.open(fileName, FILE_WRITE);
    
    if (dataFile) {
        // If file is empty or just created, write the headers
        if (dataFile.size() == 0) {
            dataFile.println("SystemTime(ms), RealTime, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ, MagX, MagY, MagZ");
        }
        
        // Now write the actual data
        dataFile.print(flushableString);
        dataFile.close();
        
        // reset the flushableString
        flushableString = "";
        
    } else {
        Serial.println("Error opening datalog.txt!");
    }
}

String twoDigits(int number) {
  if (number < 10) {
    return "0" + String(number);
  }
  return String(number);
}

void setup()
{
  // sets the baud rate for serial data communication
  // DEBUG PURPOSE
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(100);

  // Set up the I2C multiplexer TCA9548A
  I2CMux.begin(Wire);
  I2CMux.closeAll();

  I2CMux.openChannel(IMU_CHANNEL);
  
  // Initialize the 9-axis sensor
  if (bmx160.begin() != true) {
    Serial.println("BMX160 not detected. Please check your wiring.");
    while (1);
  }

  // Interval in millisecs
  if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
    
  // Initialize the SD card
  if (!SD.begin(ChipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized successfully!");

  // open the channel for RTC
  I2CMux.closeChannel(IMU_CHANNEL);
  I2CMux.openChannel(RTC_CHANNEL);
  
  // set up the DS3231 RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Uncomment the next line if want to set the RTC to a specific date and time
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
  // can use up to 16 timer for each ISR_Timer
  ISR_Timer.setInterval(TIMER_INTERVAL_50MS,  GetIMUData);
  ISR_Timer.setInterval(TIMER_INTERVAL_1S,  WriteIMUData);
}


void loop()
{
  /* Nothing to do all is done by hardware. */
}
