# Otolith

## Introduction

Code for animal-borne devices and bench post-processing associated with logging and analyzing inertial measurements, including for activity recognition.

## Table of Contents

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Hardware Setup](#hardware-setup)
- [Configuration](#configuration)
- [Usage](#usage)
- [Data Collection](#data-collection)
- [Troubleshooting](#troubleshooting)

## Dependencies

To use Otolith, you need to install the following libraries:

- `DFRobot_BMX160 v1.0.1`: A library for the BMX160 9-axis sensor.
- `SAMD_TimerInterrupt v1.9.0`: Timer Interrupt library for SAMD boards.
- `SD v1.2.4`: Library for reading and writing to SD cards.
- `SAMD_ISR_Timer`: A library for multi-tasking on SAMD boards.
- `TCA9548A`: Library for controlling the TCA9548A I2C Multiplexer.
- `RTClib`: A library for interfacing with real-time clocks.

## Installation

To set up your Otolith device, follow these steps:

1. Ensure you have the Arduino IDE installed on your computer.
2. Open the Arduino Library Manager and install the dependencies listed above.
3. Download the `Otolith20Hz` code from this repository.
4. Open the downloaded code with your Arduino IDE.

## Hardware Setup

Assemble your hardware as follows:

1. **Arduino MKR 1310**: The microcontroller board.
2. **Arduino MKR SD MEM Shield**: For SD card data logging.
3. **Arduino Real Time Clock**: To keep track of time across reboots and provide real-time data for machine learning information.
4. **TCA9548A I2C Multiplexer**: Allows for multiple I2C devices on the same address.
5. **DFRobot BMX160 IMU Sensor**: A 9-axis sensor for inertial measurements.

Ensure all parts are correctly connected and secured. Current implementation had the RTC on the I2C multiplexer's channel 1, and the IMU sensor on channel 2. Please connect accordingly or adjust the code based on the actual connection.

## Configuration

### RTC Setup

To set up the Real Time Clock:

1. Uncomment the RTC adjust code in `setup()`.
2. Upload the code to your Arduino.
3. Comment out the RTC adjust code and upload again.

This process synchronizes the RTC to your computer's current time. Missing this step would cause the RTC to not work properly.

## Usage

Once the hardware is set up and the software is uploaded:

1. The device begins logging IMU data at intervals defined in the script, which is currently 50ms (20Hz).
2. Data is stored on the SD card with timestamps for post-processing.

The main loop of the program is managed by hardware interrupts, requiring no additional input from the user during operation.

## Data Collection

### Data Acquisition

The system logs data at 50ms intervals (20Hz), including accelerometer, gyroscope, and magnetometer readings, alongside timestamps from the system clock and a real-time clock (RTC). This setup ensures that each data point is associated with an accurate time reference, crucial for later analysis.

### Data Validation

A Python script, `Tests/data_validation.py`, is employed to validate the collected data, ensuring its integrity and highlighting any potential issues. The validation checks include:

- Column name verification against expected names.
- Average time interval consistency with the expected 50ms.
- Identification of the largest timeshift, indicating potential timing inconsistencies.
- Duplication check for data rows to ensure unique data points.
- Real and system time clock incrementation verification, ensuring time data integrity.
- Missing values and data points check, confirming complete data capture.
- Frequency distribution analysis of time intervals, identifying any anomalies in data timing.

### Data Interpolation

Another Python script, `Tests/data_interpolation.py`, is used to interpolate the data to a precise 20Hz frequency. This step is crucial for minimizing the effects of minor timeshifts in data acquisition, ensuring uniform data intervals for analysis. The interpolation process involves:

- Converting 'RealTime' timestamps to datetime format.
- Reindexing the DataFrame to a uniform 50ms interval datetime range.
- Interpolating missing values based on existing data points.
- Saving the interpolated data for further analysis.

### Visualization and Analysis

The interpolation script also includes functionality for visualizing the trends of the 9-axis sensor data. This visualization helps in preliminary data analysis, identifying any obvious errors or patterns in the sensor readings.

### Steps for Data Collection and Processing

1. **Data Acquisition**: Run the Arduino code on the MKR 1310 board with the configured hardware setup to start collecting data.
2. **Data Validation**: Use the `data_validation.py` script to validate the integrity and consistency of the collected data.
3. **Data Interpolation**: Apply the `data_interpolation.py` script to interpolate the data to a consistent 20Hz frequency.
4. **Analysis and Visualization**: Analyze the interpolated data and utilize the visualization functionality in the interpolation script to examine sensor data trends.

## Troubleshooting

- **SD Card Initialization Failed**: Ensure the SD card is formatted correctly and properly inserted into the MKR SD MEM Shield. Incorrectly formatted SD card would not log data properly.
- **BMX160 Not Detected**: Check the wiring and connections between the BMX160 sensor and the multiplexer.
- **RTC Not Detected**: Check the wiring and connections between the RTC sensor and the multiplexer. Note that if the wiring is confirmed, the RTC is properly powered with the red power light on, but the serial output still indicates that the RTC is not detected, please upload the code to the MKR 1310 board without plugging it into the RTC and other components, and then power it up again with all the components connecting to it.