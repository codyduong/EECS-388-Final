# For bonus milestone
import queue
import serial
import time
import sched
import csv
from typing import List


def send_command(name: str, row: List[str], serial: serial.Serial) -> None:
    command = f"a:{row[1]};s:{row[2]};"
    print(f"{name} sending command: {command}")
    serial.write(command.encode())  # type: ignore


def schedule_commands(filename: str, scheduler: sched.scheduler, serial: serial.Serial) -> None:
    with open(filename, newline="") as csvfile:
        # remove first row
        rows: List[List[str]] = list(csv.reader(csvfile))[1:]
        for index, row in enumerate(rows):
            # if the first index, don't delay at all, otherwise use previous delay
            delay: float = float(rows[index - 1][3]) if index > 0 else 0
            print(f"scheduling {row} with delay {delay}")

            scheduler.enter(delay, index, send_command, argument=(filename, row, serial))


# Schedule the tasks async so we can run side-by-side and handle different durations correctly.
def main(s1: serial.Serial, s2: serial.Serial) -> None:
    sch: sched.scheduler = sched.scheduler(time.monotonic, time.sleep)
    schedule_commands("Bonus_ML_Data/Data1.csv", sch, s1)
    schedule_commands("Bonus_ML_Data/Data2.csv", sch, s2)
    print(sch.queue)
    sch.run()


if __name__ == "__main__":
    s1: serial.Serial = serial.Serial("/dev/ttyAMA1", 115200)
    s2: serial.Serial = serial.Serial("/dev/ttyAMA0", 115200)
    main(s1, s2)
    s1.close()
    s2.close()
