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

            if line[0] == "{" and line[-1] != "}" or line[0] != "{" and line[-1] == "}": # Check if data is complete
                err = f"[Error] Data is not complete for line: {line}"
                self.__send_logs_cb(err)
                continue

            if line[0] != "{" and line[-1] != "}": # Check if it's data
                self.__send_logs_cb(line) # Send the logs
                continue
            
            print("\033[32mDATA: {}\033[0m".format(line))

            device = Device(line) # Create a new device from the data

            if device.id == -1: # Check if the device is valid
                continue
            
            if device.addr not in self.__devices: # Check if the device is already in the list
                self.__devices[device.addr] = device # Add the device to the list
                self.__send_data_cb(device) # Send the data
                continue
            
            # Check if the device is the same 
            if device.id > self.__devices[device.addr].id or abs(device.id - self.__devices[device.addr].id) > 5:
                self.__send_data_cb(device) # Send the data
                self.__devices[device.addr] = device # Update the device
                continue

            continue
        
    def clean_str(self, string) -> str:
        '''Remove all ANSI escape sequences and the prompt'''
        string = re.sub(r'\x1b\[.*?[@-~]', '', string) # Remove all ANSI escape sequences
        string = re.sub(r'uart:~\$ ', '', string) # Remove the prompt
        return string
            

            
