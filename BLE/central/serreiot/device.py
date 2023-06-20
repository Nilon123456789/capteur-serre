from queue import Queue
import simplepyble
from abc import ABC

class Device(ABC):

    def __init__(self, peripheral) -> None:

        self.__name = peripheral.identifier()
        self.__addr = peripheral.address()
        self.__index = "" 
        self.__data = None
        self.__last_id = -1
        self.__id = -1
       
        if len(self.__name) < 2:
            return None

        self.__index = self.__name[-1] #Get last char of the name
        
        for service in peripheral.services():
            if service.uuid() == "0000cdab-0000-1000-8000-00805f9b34fb":
                self.__data = service.data
        
        if self.__data is None:
            return None
       
        if self.__data[0] != 0:
            return None

        self.__id = self.__data[1]
    
    @property
    def index(self) -> str:
        """Get the index of the sensor"""
        return self.__index
    @property
    def last_id(self) -> int:
        '''Get the last id set'''
        return self.__last_id

    @property
    def id(self) -> int:
        '''Get the current id'''
        return self.__id

    @property
    def addr(self) -> str:
        '''Get the mac address of the Device'''
        return self.__addr

    @property
    def name(self) -> str:
        '''Get the name of the device'''
        return self.__name

    @property
    def data(self) -> bytes:
        '''Get the data'''
        return self.__data

    @data.setter
    def data(self, data:bytes):
        '''Set the new data and update the id'''
        self.__data = data

        if self.__data[0] != 0:
            raise Unexpectvalue('first data byte should be 0 and not %d' % self.__data[0])
        
        self.__last_id = self.__id
        self.__id = self.__data[1]

    def getDataQueue(self) -> Queue:
        """Get the queue with the data (the id is removed)"""
        if len(self.__data) <= 2:
            return None
        
        queue = Queue(0)
        # Loop for each ellement except the 2 first
        for d in self.__data[2:]:
            queue.put(d)

        return queue


    def __eq__(self, __value: object) -> bool:
        """Compare if the two devices are identical"""
        if not isinstance(object, Device):
            return False

        return self.__addr == object.addr and self.__name == object.name
