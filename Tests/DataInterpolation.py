# This is a data interpolation script
# Author: Grace Jin
# Interpolate the data at exact 20HZ to minimize the effect of minor timeshift in the data acquisition

import pandas as pd
import matplotlib.pyplot as plt

# This function imports the csv data and convert the data into a dataframe
def import_file(name):
    df = pd.read_csv(name)
    print(df.columns)  # This will print all the column names
    return df


# This function interpolates the data to exact 20HZ and output the interpolated file
def interpolate(df, output_filename):
    # Convert 'RealTime' to datetime format
    df['RealTime'] = pd.to_datetime(df['RealTime'], format="%H:%M:%S")

    # Calculate the millisecond part using the time difference with the first row
    elapsed_time = df['SystemTime(ms)'] - df['SystemTime(ms)'].iloc[0]
    df['RealTime'] += pd.to_timedelta(elapsed_time, unit='ms')

    # Set the 'RealTime' column as index
    df.set_index('RealTime', inplace=True)

    # Create a datetime range for interpolation
    start_time = df.index[0]
    end_time = df.index[-1]
    time_range = pd.date_range(start=start_time, end=end_time, freq='50ms')

    # Resample the DataFrame to this datetime range and interpolate
    df_resampled = df.reindex(time_range).interpolate(method='index')

    # Convert the index back to the desired string format
    df_resampled.index = df_resampled.index.strftime('%H:%M:%S:%f').str[:-3]
    df_resampled.reset_index(inplace=True)
    df_resampled.rename(columns={"index": "RealTime"}, inplace=True)

    # Drop the SystemTime(ms) column
    df_resampled.drop(columns=['SystemTime(ms)'], errors='ignore', inplace=True)

    # Save the dataframe to a CSV file
    df_resampled.to_csv(output_filename, index=False)


# This function plots the 9-axis data trend
def visualization(df):
    columns = df.columns[2:]
    for column in columns:
        plt.figure(figsize=(10, 5)) # Create a new figure for each column
        plt.plot(df["SystemTime(ms)"], df[column], label=column)
        plt.xlabel('SystemTime(ms)')
        plt.ylabel('Value')
        plt.legend(loc='upper right')
        plt.title(f'{column} data trend')
        plt.show()


if __name__ == '__main__':
    # TODO: Modify the filename input to be more flexible
    filename = "231006.txt"
    data = import_file(filename)
    interpolate(data, "interpolated"+filename+".csv")
    visualization(data)
