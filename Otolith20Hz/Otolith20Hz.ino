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
////////////////////////////////////////////////

// Init SAMD_ISR_Timer
// Each SAMD_ISR_Timer can service 16 different ISR-based timers
SAMD_ISR_Timer ISR_Timer;

#define TIMER_INTERVAL_50MS             50L

void TimerHandler(void)
{
  ISR_Timer.run();
}

// Get the IMU data from the 9-axis sensor and log it onto the SD card
void GetIMUData()
{
  // Generate a date using the RTC
  // TODO
  File dataFile = SD.open("DATA.txt", FILE_WRITE);
  sBmx160SensorData_t Mag, Gyro, Accel;

  bmx160.getAllData(&Mag, &Gyro, &Accel);

  // Print all the data
  Serial.print("Gyro: ");
  Serial.print(Gyro.x);
  Serial.print(", ");
  Serial.print(Gyro.y);
  Serial.print(", ");
  Serial.print(Gyro.z);
  
  Serial.print(" Accel: ");
  Serial.print(Accel.x);
  Serial.print(", ");
  Serial.print(Accel.y);
  Serial.print(", ");
  Serial.print(Accel.z);

    
  Serial.print(" Mag: ");
  Serial.print(Mag.x);
  Serial.print(", ");
  Serial.print(Mag.y);
  Serial.print(", ");
  Serial.println(Mag.z);

  // Write to the SD card
  if (dataFile) {
    dataFile.print("Time: ");
    dataFile.print(millis());
    
    dataFile.print(" Gyro: ");
    dataFile.print(Gyro.x);
    dataFile.print(", ");
    dataFile.print(Gyro.y);
    dataFile.print(", ");
    dataFile.print(Gyro.z);
    
    dataFile.print(" Accel: ");
    dataFile.print(Accel.x);
    dataFile.print(", ");
    dataFile.print(Accel.y);
    dataFile.print(", ");
    dataFile.print(Accel.z);

    dataFile.print(" Mag: ");
    dataFile.print(Mag.x);
    dataFile.print(", ");
    dataFile.print(Mag.y);
    dataFile.print(", ");
    dataFile.println(Mag.z);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt!");
  }
}

void setup()
{
  // sets the baud rate for serial data communication
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(100);

  // Initialize the 
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
  
  // can use up to 16 timer for each ISR_Timer
  ISR_Timer.setInterval(TIMER_INTERVAL_50MS,  GetIMUData);
}


void loop()
{
  /* Nothing to do all is done by hardware. */
}
