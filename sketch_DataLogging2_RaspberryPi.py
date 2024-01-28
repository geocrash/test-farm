import os
import sys
import datetime
import serial

from sketch_DataLogging_RaspberryPi_OneScript import Logger

logger_arduino_2 = Logger(
    columns=[
        'Time', 'Minutes Cycled', 'Inlet Pressure', 'Simulated Minutes', 'Percentage Complete'
    ],
    save_dir='/home/pi/Desktop/TestFarm2',
    backup_dir='/home/pi/Desktop/TestFarm2-backup',  # '/media/SD CARD/TestFarm1',
    tty='/dev/ttyACM1'
)

logger_arduino_2.start()
