# This is a data validation test script
# Author: Grace Jin
# Input the acquired csv file (not the interpolated) and test if the data is correctly collected
# The script will check the following:
# 1. Column names
# 2. Average time intervals
# 3. Largest timeshift
# 4. Duplicated rows
# 5. Real time clock
# 6. System time clock
# 7. Missing values
# 8. Missing data points
# 9. Frequency distribution of time intervals
# The script will print out the issues found and a summary of the validation test
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime as dt

EXPECTED_COLUMNS = ['SystemTime(ms)', 'RealTime', 'AccelX', 'AccelY', 'AccelZ', 'GyroX', 'GyroY', 'GyroZ', 'MagX', 'MagY', 'MagZ']
EXPECTED_INTERVAL = 50  # in ms
FMT = "%H:%M:%S"


def validate_data(filename):
    # Read the csv file and insert an index column
    df = pd.read_csv(filename)
    issues = []
    print("----------Validating Data----------")
    # 1. Validate column names
    if not all(col in df.columns for col in EXPECTED_COLUMNS):
        issues.append(f"Column names mismatch! Expected: {EXPECTED_COLUMNS} but got: {list(df.columns)}")
    else:
        print("Correct - All the column names matches expectation.")
    # 2. Validate average time intervals
    time_diffs = df['SystemTime(ms)'].diff().dropna()
    avg_interval = time_diffs.mean()
    if avg_interval != EXPECTED_INTERVAL:
        issues.append(f"Average time interval is {avg_interval}ms, expected {EXPECTED_INTERVAL}ms.")

    # 3. Report the largest timeshift
    timeshifts = abs(time_diffs - EXPECTED_INTERVAL)
    max_timeshift = timeshifts.max()
    if max_timeshift > 0:
        issues.append(f"Largest timeshift is {max_timeshift}ms.")

    # 4. Check for duplicated rows
    if df.duplicated().any():
        issues.append(f"There are {df.duplicated().sum()} duplicated rows in the data.")
    else:
        print("Correct - No duplicated rows found.")

    # 5. Check if the real time is correctly incremented
    df['RealTime'] = pd.to_datetime(df['RealTime'], format=FMT).dt.time
    # convert the real time to seconds
    realtime_seconds = df['RealTime'].apply(lambda x: dt.strptime(str(x), FMT).timestamp())
    # print(realtime_seconds)
    timedeltas = realtime_seconds.diff().dropna()
    # count how many negative time intervals happened
    failed_count = sum(timedeltas < 0)
    if failed_count > 0:
        issues.append("Real time clock is not correctly incremented. System might had: " + str(failed_count) + " fails.")
    else:
        print("Correct - Real time clock is correctly incremented.")
    # print(df['RealTime'])

    # 6. Check if the system time is correctly incremented
    timedeltas = df['SystemTime(ms)'].diff().dropna()
    # count how many negative time intervals happened
    reset_count = sum(timedeltas < 0)
    if reset_count > 0:
        issues.append("System time is not correctly incremented. System might had: " + str(reset_count) + " resets.")
    else:
        print("Correct - System time is correctly incremented.")

    # 6. Check for missing values
    missing_values_count = df.isna().sum().sum()
    if missing_values_count > 0:
        issues.append(f"There are {missing_values_count} missing values in the data.")
    else:
        print("Correct - No timestamps are logged without sensor data.")

    # 7. Report missing datapoints
    duration = df['SystemTime(ms)'].iloc[-1] - df['SystemTime(ms)'].iloc[0]
    expected_data_points = duration / EXPECTED_INTERVAL
    missing_data_points = expected_data_points - len(df)
    if missing_data_points > 0:
        issues.append(
            f"Missing {int(missing_data_points)} data points. Expected {int(expected_data_points)} but got {len(df)}.")

    # 8. Frequency distribution of time intervals
    frequency_dist = time_diffs.value_counts()
    # Histogram for frequency distribution
    plt.figure(figsize=(10, 6))
    plt.hist(df['SystemTime(ms)'].diff().dropna(), bins=30, edgecolor='k', alpha=0.65)
    plt.axvline(EXPECTED_INTERVAL, color='r', linestyle='dashed', linewidth=1)
    plt.title('Frequency Distribution of Time Intervals')
    plt.xlabel('Time Intervals (ms)')
    plt.ylabel('Frequency')
    plt.legend(['Expected Interval', 'Time Intervals'])
    plt.show()

    # Print out all issues
    if issues:
        print("-----SUMMARY-----")
        print("Issues found:")
        for issue in issues:
            print("- " + issue)
    else:
        print("No issues found. Data validated successfully!")


if __name__ == '__main__':
    # TODO: Make the filename input more flexible
    validate_data('231006.txt')
