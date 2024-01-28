from sketch_DataLogging1_RaspberryPi import sensor_data # sensor data received from the arduino and provided to the Raspberry Pi


def checkdata(newdata, sensor_data):
    if not newdata == sensor_data:
        return True
    else:
        return False

newdata = sensor_data

for x in newdata:
    print(x, end = '')

print("\n\n")

while True:
    from sketch_DataLogging_RaspberryPi_OneScript import sensor_data
    checking = checkdata(newdata, sensor_data)
    if checking == True:
        newdata =  sensor_data
        for count in range(0, len(newdata)):
            print(newdata[count])
        print("\n\n")
