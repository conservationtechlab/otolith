import datetime as dt
#TODO: Remove this file, its not necessary and replace its references here with constants defined within LOG_Parser.py
import LogConstants as const 
import numpy as np
import LOGData_Plotter as ldp

DATA_IDX_START = 2
DATE_TIME_COL = 2


#Takes string line and separates chunks into array items (chunking based on delimiters, set to tabs for LOG data from WT901SD)
def extractRawData(text_line, delim = "\t"):
    raw_data_arr = []
    raw_data = ""
    for c in text_line:
        if c == delim or c == "\n": #if newline character is included, that is discounted (seems to be case with .csv file)
            raw_data_arr.append(raw_data)
            raw_data = ""
            continue;
        else:
            raw_data += c
    if (raw_data != ''):   #added for reading csv; may be useful for others as well
        raw_data_arr.append(raw_data)
    return raw_data_arr




#Converts datetime into float based on WT901SD log data style, outputs 0 if error
def getTimeStamp(str):
    try:
        dt_obj = dt.datetime.strptime(str, ' %Y-%m-%d %H:%M:%S.%f')  # the wit thinks 0th day is acceptable, so day is not longer considered
        dt_timestamp = dt_obj.timestamp()
        return dt_timestamp
    except: 
        try:  # TODO: There's probably a better way to do this, and that should be done instead
            dt_time = dt.time.strptime(str[10:], ' %H:%M:%S.%f')
            print(str[10:])
            dt_timestamp = dt_time.time()
            return dt_timestamp
        except: 
            return 0  

#for printing non-numpy arrays in a way that's easier to visualize
def printByLine(data_array):
    for line in data_array:
        print(line) 
    

#Conversion of WT901SD log data line from string to integers and floats (including log time)
def rawToRealData(data_arr):
    newDataArr = [] #new array is created because the first time column is not useful

    if len(data_arr) <= const.DATETIME_COL:
        print("Too Short Array")
        return 0

    timestamp = getTimeStamp(data_arr[const.DATETIME_COL]) #DATETIME_COL = 2
    if timestamp == 0: #0 is the error output for get timestamp. Otherwise, it outputs a float-based time
        print("Timestamp is zero")
        return 0 
    
    newDataArr.append(timestamp)
    for data in data_arr[ const.DATETIME_COL+1: ]: 
        if (data != ""):
            newDataArr.append(float(data))
        else:
            print("Null Data")
    return newDataArr

#Input a WT901SD log file (.txt) and this will parse the data based on tab spacing, skipping first two lines and first column
def getLogData(file_name):
    log_file = open(file_name,"r")
    
    motion_data = []
    file_data = []
    
    for line in log_file:
        raw_line = extractRawData(line)
        print(raw_line)
        real_line = rawToRealData(raw_line)
        print(real_line)
        file_data.append(raw_line)
        if real_line != 0:
            motion_data.append(real_line)

    
    log_file.close()
    return motion_data, file_data



#TODO: data labelling will need to be standardized and once thats done a better readActivityLog function can be done
#This is the "main" function within this file, it completes all the parsing
def readActivityLog(csv_name):
    #TODO: This does not account for headers. Need to work on that edge case
    csv_file = open(csv_name)
    chip_time_arr = []
    activity_arr = []
    for line in csv_file:
        line_arr = extractRawData(line,delim = ',')
        print(line_arr)
        chip_time_arr.append(int(line_arr[1]))
        activity_arr.append(line_arr[2])
    return (activity_arr, chip_time_arr)



#Test function
def extractRawData_TEST_LOOP():
    text_in = input("Type sentence to parse (STOP to stop): ")
    while (text_in != "STOP"):
        text_array = extractRawData(text_in)
        print(text_array)
        text_in = input("Type sentence to parse: ")

def makeStandingAndWalkingArrays(actv_arr):
    standing_arr = np.zeros(len(actv_arr))
    walking_arr = np.zeros(len(actv_arr))
    for i, activity in enumerate(actv_arr):
        if (activity == "Walk"):
            walking_arr[i] = 1
        elif ( activity == "Stand"):
            standing_arr[i] = 1
    return standing_arr, walking_arr

#TODO: test to make sure this will work for stand walk example
def generateActivityArraysForPlotting(actv_arr, labels):
    activityByLabels = np.zeros((len(actv_arr),len(labels))) #labels = row, T/F = col, shape = (row, col)
    
    for row, actv_data in enumerate(actv_arr):
        for col, label in enumerate(labels):
            if (actv_data == label):
                activityByLabels[row,col] = 1
                continue; #there shouldn't be any duplicate labels in "labels" so there should only be one 1 per column
     
    return activityByLabels #arranged in a way such that labels could work as a header file for the data underneath



if __name__ == "__main__":
    new_log = "LOG00022_New.txt"
    log_4 = "LOG00004_FirstDataCapture.txt"
    old_log = "2022-04-15_15h59m17s.txt"
    file_name = new_log;
    csv_name = "2022-04-15_15h59m17s.csv"
    
    (motion_data, raw_data) = getLogData(file_name)

    #printByLine(motion_data)
    motion_data = np.array(motion_data)
    #printByLine(motion_data)

    #actv_arr, log_time = readActivityLog(csv_name)

    #standing_arr, walking_arr = makeStandingAndWalkingArrays(actv_arr)
    #label_data = [log_time, standing_arr, walking_arr]


    #ldp.plotDataAndLabel_SubPlots(motion_data,label_data)
    #ldp.plotAccelerationDataOnly(motion_data)
    #ldp.plotData_SubPlots(motion_data)
    #ldp.plotData_vs_Labels_SubPlots(motion_data,label_data)

    #plt.show()



    


