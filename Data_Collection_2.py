import serial
import csv
import time

class Prototype:
    data = [0, 0, 0, 0, 0, 0]
    transferStatus = [False, False, False, False, False, False]
    
    def __init__(self):
        pass
    
    def retrieveData(self, transferData):
        self.data[int(transferData[1])] = float(transferData[2::])
        self.transferStatus[int(transferData[1])] = True
    
    def checkComplete(self):
        if all(status == True for status in self.transferStatus):
            for i in range(len(self.transferStatus)):
                self.transferStatus[i] = False
            return True
        return False

# Configure the serial port
try:
    ser = serial.Serial('COM3', 115200)  # Change to the correct port for your setup
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit()

print("Serial port opened successfully")

prototypeArray = []
nameArray = ["0", "1", "2", "3", "4", "5"]
NAMES = {"0" : 0, "1" : 1, "2" : 2, "3" : 3, "4" : 4, "5" : 5}
for i in range(len(NAMES)):
    prototypeArray.append(Prototype())

# Open a CSV file to save the data
with open('received_data.csv', 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    
    # Write headers (adjust depending on the structure of your data)
    csv_writer.writerow(['Time', 'ID', 'Latitude', 'Longitude', 'Wind Angle', 'Pressure', 'Humidity', 'Temperature'])  # You can change these headers based on your needs
    
    csv_file.close()
    
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8').strip()  # Reading line of data and decoding
            print(line)
            if len(line) == 0:
                continue
            
            i = NAMES[line[0]]
            name = nameArray[i]
            prototype = prototypeArray[i]
            prototype.retrieveData(line)
                
            if prototype.checkComplete():
                local_time = time.localtime()
                formatted_time = time.strftime("%Y-%m-%d %H:%M:%S", local_time)
                csv_file = open('received_data.csv', 'a', newline='')
                csv_writer = csv.writer(csv_file)
                csv_writer.writerow([formatted_time, name, prototype.data[0], prototype.data[1], prototype.data[2], prototype.data[3], prototype.data[4], prototype.data[5]])
                csv_file.close()

ser.close()