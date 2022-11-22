import datetime as dt

def writeActivityLog(label_data):
    actv_log = open("activity_log.txt","w+")
    
    actv_log.write("Start Time: "+dt.now()+"\n")
    actv_log.write("Real Time\tChip Time\tActivity\n")
    for label_row in label_data:
        for item in label_row:
            actv_log.write(item+"\t")
        actv_log.write("\n")
    #1 second separated time


if __name__ == "__main__":
    dt_obj = dt.datetime(2022,4,15,15,59,29)
    dt_timestamp = dt_obj.timestamp()
    print(dt_obj)
    print (dt_timestamp)
