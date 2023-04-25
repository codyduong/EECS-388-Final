import serial
# import time
import csv

ser1 = serial.Serial("/dev/ttyAMA1", 115200)

with open("commands.csv", newline="") as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        print(row)
        # angle, speed, duration
        command = f"\na:{row[0]};s:{row[1]};d:{row[2]};\n"
        ser1.write(command.encode())
        # time.sleep(float(row[2]))

ser1.close()