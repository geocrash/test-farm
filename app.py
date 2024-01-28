import glob
import os.path
import csv

from flask import Flask, render_template

app = Flask(__name__)

columns23=[
    'Time', 'Minutes Cycled', 'Inlet Pressure', 'Simulated Minutes', 'Percentage Complete']

columns14=[
    'Time', 'Time Pressure Profile',
    'Inlet Pressure', 'Water Temperature', 'Electrolyte Temperature', 'Heater On/Off', 'Pumping Power',
    'Pressure Cycles', 'Days Cycled', 'Days Simulated', 'Temperature Cycles', 'Time Arduino'
]

def get_data(path):
    """Path written in the form of "TestFarm1/" for example.
    Receives data from the newest file in the folder. """
    folder_path = r'/home/pi/Desktop/' + path
    file_type = '/*csv'
    files = glob.glob(folder_path + file_type)
    file = max(files, key=os.path.getctime)
    value = 0
    if path == 'TestFarm1' or (path == 'TestFarm4'):
        size_c = len(columns14)
    elif (path == 'TestFarm2') or (path == 'TestFarm3'):
        size_c = len(columns23)
    for row in open(file):
        value+= 1
    with open(file, "r") as file:
        reader_file = csv.reader(file)
        rows1 = file.readlines()[value - 20: value]
        rows2 = []
        for x in rows1:
            y = x.split(',')
            rows2 = rows2 + y
        rows = [rows2[i: i + size_c] for i in range(0, len(rows2), size_c)]
    return rows

@app.route("/")
def home():
    return render_template("index.html")

@app.route("/1")
def farm1():
    rows = get_data('TestFarm1')
    return render_template("farm1.html", headings=columns14, data=rows)

@app.route("/2")
def farm2():
    rows = get_data('TestFarm2')
    return render_template("farm2.html", headings=columns23, data=rows)

@app.route("/3")
def farm3():
    rows = get_data('TestFarm3')
    return render_template("farm3.html", headings=columns23, data=rows)

@app.route("/4")
def farm4():
    rows = get_data('TestFarm4')
    return render_template("farm4.html", headings=columns14, data=rows)

if __name__ == "__main__":
    app.debug = True
    app.run(host = '0.0.0.0', port = 4000)

    #192.168.1.117:4000 --> Raspberry Pi port
    #192.168.1.60:4000 --> Laptop port
