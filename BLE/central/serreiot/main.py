import simplepyble
from aliot.aliot_obj import AliotObj
from device import Device
from queue import Queue

sensor_iot = AliotObj("serreiot")

def scan(adapter):
    print("scan:")
    # Scan for 1 seconds
    adapter.scan_for(1000)

    devices = []
    peripherals = adapter.scan_get_results()
    for peripheral in peripherals:
        print(f"\t{peripheral.identifier()} [{peripheral.address()}]")
        device = Device(peripheral)
        if device.last_id == -1:
            continue

        devices.append(device)
        print(f"{peripheral.identifier()} [{peripheral.address()}]")

    return devices

def start():
    '''Main function'''

    #Selecting the Bluethoot adapter
    adapters = simplepyble.Adapter.get_adapters()

    if len(adapters) == 0:
        print("No adapters found")

    adapter = adapters[0] # Selecting by default the 0
    print(f"Selected adapter: {adapter.identifier()} [{adapter.address()}]")
    
    devices = []
    sensors_values = {
        1 : 99.99, # temp
        2 : 99.99, # hum
        3 : 99.99, # lum
        4 : 99.99, # gnd temp
        5 : 99.99, # gnd hum
        254 : 0.00 # bat
    }

    while(True):
        scaned_devices = scan(adapter)
        if (scaned_devices is None):
            print("\tNo devices found")
            continue

        # Update the list of devices
        for device in devices:
            for new_device in scaned_devices:
                if device == new_device:
                    device.data = new_device.data
                    continue
            device.data = device.data #Set the device data to his data to update the last id
        
        for device in devices: 
            if device.id == device.last_id: #Skip if data is the same
                continue

            string = f"{device.name} Values : "
            while not values.empty():
                id = values.get(0)
                whole = values.get(0)
                decimal = values.get(0)
                sensors_values[id] = (whole + (decimal / 100))
                string += f"{id} : {sensors_values[id]} | "
            print(string)
            
            path = f'/doc/{device.index}'
            sensor_iot.update_doc({
                f'{path}/humidity' : sensors_values[2],
                f'{path}/temperature' : sensors_values[1],
                f'{path}/luminosite' : sensors_values[3],
                f'{path}/gnd_temperature' : sensors_values[4],
                f'{path}/gnd_humidity' : sensors_values[5],
                f'{path}/batterie' : sensors_values[254],
                '/doc/id' : cur_id
                })
            continue

sensor_iot.on_start(callback=start)
sensor_iot.on_end(callback=lambda: exit())
sensor_iot.run()
