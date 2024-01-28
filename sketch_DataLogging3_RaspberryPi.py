import os
import sys
import datetime
import serial

from sketch_DataLogging_RaspberryPi_OneScript import Logger

logger_arduino_3 = Logger(
    columns=[
        'Time', 'Minutes Cycled', 'Inlet Pressure', 'Simulated Minutes', 'Percentage Complete'
    ],
    save_dir='/home/pi/Desktop/TestFarm3',
    backup_dir='/home/pi/Desktop/TestFarm3-backup',  # '/media/SD CARD/TestFarm1',
    tty='/dev/ttyACM2'
)

logger_arduino_3.start()
