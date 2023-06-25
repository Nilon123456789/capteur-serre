from threading import Thread
import serial
import re
import codecs

from device import Device

class Reader():

    def __init__(self, port, baudrate, send_data_cb, send_logs_cb) -> None:
        self.__ser = serial.Serial(port, baudrate)
        self.__send_data_cb = send_data_cb
        self.__send_logs_cb = send_logs_cb
        self.__devices = {}

        self.__thread = Thread(target=self.__read)
        self.__thread.start()

    def __read(self):
        while(True):
            line = ""
            err = ""
            if  self.__ser .in_waiting > 0:
                try:
                    line =  self.clean_str(self.__ser .readline().decode('utf-8').rstrip())
                except UnicodeDecodeError as e:
                    line =  self.clean_str(self.__ser .readline().decode('utf-8', 'replace').rstrip())
                    error_position = e.args[2]
                    err = f"[Error] Decoding error occurred at {error_position} for line: {line}"
                    
            if line == "": # Check if the line is empty
                continue

            if err != "": # Check if there is an error
                self.__send_logs_cb(err) # Send the error
            
            if line[0] == "[" : # Check if the line is a log
                self.__send_logs_cb(line)
                continue
            
            if not self.__is_valid(line): # Check if the data is valid
                continue
            
            print("\033[32mDATA: {}\033[0m".format(line))

            device = Device(line) # Create a new device from the data

            if device.id == -1: # Check if the device is valid
                continue
            
            if device.addr not in self.__devices: # Check if the device is already in the list
                self.__devices[device.addr] = device # Add the device to the list
                self.__send_data_cb(device) # Send the data
                continue
            
            # Check if the device has not the same id as last time
            if device.id > self.__devices[device.addr].id or abs(device.id - self.__devices[device.addr].id) > 5:
                self.__send_data_cb(device) # Send the data
                self.__devices[device.addr] = device # Update the device
                continue

            continue

    def __is_valid(self, data) -> bool:
        '''Check if the data is valid in this format ({name,addr,data})'''
        if data[0] != "{" or data[-1] != "}": # Check if the data has the right format
            self.__send_logs_cb(f"[Error] The data is not in the right format for line: {data}")
            return False
        
        if data.count("{") > 1 or data.count("}") > 1: # Check if there is more than one {} in the data
            self.__send_logs_cb(f"[Error] There is more than one {{}} in the data for line: {data}")
            return False
        
        if data.count(",") != 2: # Check if there is the right amount of commas
            self.__send_logs_cb(f"[Error] There is not the right amount of commas for line: {data}")
            return False
        
        return True
    

    def clean_str(self, string) -> str:
        '''Remove all ANSI escape sequences and the prompt'''
        string = re.sub(r'\x1b\[.*?[@-~]', '', string) # Remove all ANSI escape sequences
        string = re.sub(r'uart:~\$ ', '', string) # Remove the prompt
        return string
            

            
