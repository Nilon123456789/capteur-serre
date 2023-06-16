import simplepyble
from aliot.aliot_obj import AliotObj
from queue import Queue

sensor_iot = AliotObj("serreiot")

adapters = simplepyble.Adapter.get_adapters()

if len(adapters) == 0:
    print("No adapters found")

adapter = adapters[0]

print(f"Selected adapter: {adapter.identifier()} [{adapter.address()}]")

adapter.set_callback_on_scan_start(lambda: print("Scan started."))
adapter.set_callback_on_scan_stop(lambda: print("Scan complete."))

def scan():
    # Scan for 1 seconds
    adapter.scan_for(1000)

    sensors = None
    peripherals = adapter.scan_get_results()
    for peripheral in peripherals:

        services = peripheral.services()
        for service in services:
            if (service.uuid() == "0000cdab-0000-1000-8000-00805f9b34fb"):
                sensors = peripheral
                connectable_str = "Connectable" if peripheral.is_connectable() else "Non-Connectable"
                print(f"{peripheral.identifier()} [{peripheral.address()}] - {connectable_str}")

    return sensors

def start():
    last_id = 0
    sensors_values = {
        1 : 99.99, # temp
        2 : 99.99, # hum
        3 : 99.99, # lum
        4 : 99.99, # gnd temp
        5 : 99.99, # gnd hum
        254 : 0.00 # bat
        }
    while(True):
        sensor = scan()
        if (sensor is None):
            print("\tNo sensor found")
            continue
    
        data = None
        services = sensor.services()
        for service in services:
            if (service.uuid() != "0000cdab-0000-1000-8000-00805f9b34fb"):
                print("\tNo valid service")
                continue
            data = service.data()
            if (data is None):
                print("\tData not found")
                continue
        
        print("Data found ", len(data))
        values = Queue(0)
        for d in data:
            values.put(d)
        
        if(values.get(0) != 0):
            print("\tError expected 0 but got other value")
            continue
        
        cur_id = values.get(0)
        if(cur_id == last_id):
            print("\tSame id as last time")
            continue
        last_id = cur_id
        print("\tNew id ", cur_id)
       
        string = "Values : "
        while not values.empty():
            id = values.get(0)
            whole = values.get(0)
            decimal = values.get(0)
            sensors_values[id] = (whole + (decimal / 100))
            string += f"{id} : {sensors_values[id]} | "
        print(string)

        sensor_iot.update_doc({
            '/doc/humidity' : sensors_values[2],
            '/doc/temperature' : sensors_values[1],
            '/doc/luminosite' : sensors_values[3],
            '/doc/gnd_temperature' : sensors_values[4],
            '/doc/gnd_humidity' : sensors_values[5],
            '/doc/batterie' : sensors_values[254],
            '/doc/id' : cur_id
            })
        continue

print("Starting")
sensor_iot.on_start(callback=start)
sensor_iot.on_end(callback=lambda: exit())
print("Running")
sensor_iot.run()
