# For bonus milestone
import serial
import time
import sched
import csv


def send_data(filename: str, s: serial.Serial) -> None:
    with open("commands.csv", newline="") as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            # angle, speed, duration
            # command = f"a:{row[0]};s:{row[1]};d:{row[2]};"
            command = f"a:{row[1]};s:{row[2]};"
            print(f"{filename} sending command: {command}")
            s.write(command.encode())  # type: ignore
            time.sleep(float(row[3]))


# Schedule the tasks async so we can run side-by-side and handle different durations correctly.
def main(s1: serial.Serial, s2: serial.Serial) -> None:
    sch: sched.scheduler = sched.scheduler(time.monotonic, time.sleep)
    sch.enter(0, 1, send_data, argument=("BONUS_ML_DATA/Data1.csv", s1))
    sch.enter(0, 1, send_data, argument=("BONUS_ML_DATA/Data1.csv", s2))


if __name__ == "__main":
    s1: serial.Serial = serial.Serial("/dev/ttyAMA1", 115200)
    s2: serial.Serial = serial.Serial("/dev/ttyAMA3", 115200)
    main(s1, s2)
    s1.close()
    s2.close()
