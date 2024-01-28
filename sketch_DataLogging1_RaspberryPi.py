import os
import sys
import datetime
import serial
from sketch_DataLogging_RaspberryPi_OneScript import Logger

logger_arduino_1 = Logger(
    columns=[
        'Time', 'Time Pressure Profile',
        'Inlet Pressure', 'Water Temperature', 'Electrolyte Temperature', 'Heater On/Off', 'Pumping Power',
        'Pressure Cycles', 'Days Cycled', 'Days Simulated', 'Temperature Cycles', 'Time Arduino'
    ],
    save_dir='/home/pi/Desktop/TestFarm1',
    backup_dir='/home/pi/Desktop/TestFarm1-backup',  # '/media/SD CARD/TestFarm1',
    tty='/dev/ttyACM0'
)

logger_arduino_1.start()
