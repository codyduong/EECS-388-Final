# For bonus milestone
import serial
import time
import sched
import csv
from typing import List


def send_command(name: str, row: List[str], serial: serial.Serial) -> None:
    command = f"a:{row[1]};s:{row[2]};"
    print(f"{name} sending command: {command}")
    serial.write(command.encode())  # type: ignore


def schedule_commands(
    filename: str, scheduler: sched.scheduler, serial: serial.Serial
) -> None:
    with open(filename, newline="") as csvfile:
        delay: float = 0
        # remove first row
        rows: List[List[str]] = list(csv.reader(csvfile))[1:]
        for index, row in enumerate(rows):
            scheduler.enter(
                delay, index, send_command, argument=(filename, row, serial)
            )
            # add new delay to scheduler
            delay += float(row[3])


# Schedule the tasks async so we can run side-by-side and handle different durations correctly.
def main(s1: serial.Serial, s2: serial.Serial) -> None:
    sch: sched.scheduler = sched.scheduler(time.monotonic, time.sleep)
    schedule_commands("Bonus_ML_Data/Data1.csv", sch, s1)
    schedule_commands("Bonus_ML_Data/Data2.csv", sch, s2)
    sch.run()


if __name__ == "__main__":
    s1: serial.Serial = serial.Serial("/dev/ttyAMA1", 115200)
    s2: serial.Serial = serial.Serial("/dev/ttyAMA2", 115200)
    try:
        main(s1, s2)
    finally:
        s1.close()
        s2.close()
