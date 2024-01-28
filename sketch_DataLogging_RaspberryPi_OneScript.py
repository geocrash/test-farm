import os
import sys
import datetime
import serial


class Logger:
    """Logs incoming data of an Arduino to csv files."""

    def __init__(self, columns, save_dir, backup_dir, tty):
        """Constructor."""
        self.columns = columns
        self.save_dir = save_dir
        self.backup_dir = backup_dir
        self.tty = tty

        # state initialization
        self.main_file = None
        self.backup_file = None
        self.serial = None
        self.todays_date = None

        self.create_files()

    def data_save(self, file, sensor_data):
        """Save the data into a certain location (file and a backup file)."""
        # used as an additional input in excel sheet
        now = datetime.datetime.now()
        file.write(f'{str(now)},{",".join(sensor_data)}')
        file.flush()

    def create_files(self):
        """Create a new file for the respected TestFarm."""
        # redefines today's date
        self.todays_date = datetime.date.today()

        # creates a new file with today's date, for the first round of logging of the file
        filename = f'TestFarm_log_{self.todays_date}.csv'

        if self.main_file is not None:
            self.main_file.close()

        if self.backup_file is not None:
            self.backup_file.close()

        main_file_path = os.path.join(self.save_dir, filename)
        os.makedirs(self.save_dir, exist_ok=True)
        self.main_file = open(main_file_path,'a+')
        self.main_file.write(f"{','.join(self.columns)}\n")

        backup_file_path = os.path.join(self.backup_dir, filename)
        os.makedirs(self.backup_dir, exist_ok=True)
        self.backup_file = open(backup_file_path,'a+')
        self.backup_file.write(f"{','.join(self.columns)}\n")

    def check_arduino(self):
        """Check if the arduino is connected."""
        try:
            self.serial = serial.Serial(self.tty, 9600)
        except:
            print('No Arduino found')
            sys.exit(1)

    def log(self):
        """Log data for the specified TestFarm."""
        # resets the date each loop and compares it to "todays_date"
        current_date = datetime.date.today()

        # as the read_serial will be in the form of '1.234;1.3423;...', it is
        # split and the ';' are removed
        sensor_data = self.serial.readline().decode().split(';')

        # if it is not the same day, must create a new file with the name of today's date
        if current_date > self.todays_date:
            self.create_files()
            self.todays_date = datetime.date.today()

        # logs data in a file
        self.data_save(self.main_file, sensor_data)
        # saves data in a backup
        self.data_save(self.backup_file, sensor_data)

    def start(self):
        """Start to continuously log data to .csv files."""
        self.todays_date = datetime.date.today()

        # Recieves information from Arduino, reads it and saves it
        while True:
            self.check_arduino()
            self.log()
