# This is a data interpolation script
# Author: Grace Jin
# We interpolate the data at exact 20HZ to minimize the effect of minor timeshift in the data acquisition

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# This function imports the csv data and convert the data into a dataframe
def import_file(name):
    # Use pandas to read the CSV-formatted text file
    df = pd.read_csv(name)
    return df


# This function interpolates the data to exact 20HZ and output the interpolated file
def interpolate(df, output_filename):
    # Assuming 'Time' column is in milliseconds
    # Create a new time range from min to max of the existing time column with 50ms intervals (20Hz)
    new_time = np.arange(df['Time'].min(), df['Time'].max(), 50)
    new_df = pd.DataFrame(new_time, columns=['Time'])

    # Merge the new time frame with the original data and interpolate the missing values
    new_df = pd.merge_asof(new_df, df, on='Time', direction='nearest').interpolate()

    # Save the interpolated data to a new CSV file
    new_df.to_csv(output_filename, index=False)
    print(f"Interpolated data saved to {output_filename}")


# This function plots the 9-axis data trend
def visualization(df):
    time = df['Time']
    # check the column names of the data
    print(data.columns)
    # Plot accelerometer data
    plt.figure(figsize=(10, 6))
    plt.plot(time, df['AccelX'], label='AccelX')
    plt.plot(time, df['AccelY'], label='AccelY')
    plt.plot(time, df['AccelZ'], label='AccelZ')
    plt.title('Accelerometer Data Trend')
    plt.xlabel('Time (ms)')
    plt.ylabel('Acceleration')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Plot gyroscope data
    plt.figure(figsize=(10, 6))
    plt.plot(time, df['GyroX'], label='GyroX')
    plt.plot(time, df['GyroY'], label='GyroY')
    plt.plot(time, df['GyroZ'], label='GyroZ')
    plt.title('Gyroscope Data Trend')
    plt.xlabel('Time (ms)')
    plt.ylabel('Angular Velocity')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Plot magnetometer data
    plt.figure(figsize=(10, 6))
    plt.plot(time, df['MagX'], label='MagX')
    plt.plot(time, df['MagY'], label='MagY')
    plt.plot(time, df['MagZ'], label='MagZ')
    plt.title('Magnetometer Data Trend')
    plt.xlabel('Time (ms)')
    plt.ylabel('Magnetic Field Strength')
    plt.legend()
    plt.grid(True)
    plt.show()


if __name__ == '__main__':
    # TODO: Modify the filename input to be more flexible
    filename = "231006.txt"
    data = import_file(filename)
    interpolate(data, "interpolated"+filename+".csv")
    visualization(data)



