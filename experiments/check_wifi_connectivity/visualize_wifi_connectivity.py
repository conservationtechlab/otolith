"""Script to evaluate ping connectivity check

Example of a way to ingest and visualize the output from our quick and
dirty connectivity logging.

Designed to work on the output of a run of the following command on
the CLI (or some variant thereof, i.e. you might want to change the
ping rate):

ping -i 5 8.8.8.8 | ts "%Y %m %d %H:%M:%S" | tee ping_record.txt

Usage:

    python check_connectivity_of_host ping_record.txt

"""
import re

import argparse
from datetime import datetime
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser()
parser.add_argument("input_filename")
args = parser.parse_args()

with open(args.input_filename, 'r', encoding='utf-8') as input_file:
    contents = input_file.readlines()

# find IP address of pinged host in first line of ping record
pattern = re.compile(r'(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})')
host_ip = pattern.search(contents[0])[0]

# populate a list of all the timestamps when ping was successful
timestamps = []
for line in contents[1:]:  # cut first line: not meaningful for this
    datetime_str = line[:19]  # consider more elegant parsing here
    timestamps.append(datetime.strptime(datetime_str, '%Y %m %d %H:%M:%S'))

y = [1] * len(timestamps)
plt.bar(timestamps, y, width=.000001)
plt.title(f'"Pingability" over time of host at {host_ip}')
plt.xlabel('Timestamp')
plt.ylabel('1 = Connected. Blank = Unconnected.')
plt.show()
