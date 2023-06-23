.. _bluetooth_central:

Bluetooth: Central
##################

Overview
********

Application that passively scans for BLE devices, to after
send them via UART to a host with the following format:
{name,address,service_data}


Requirements
************

* BlueZ running on the host, or
* A board with BLE support

Building and Running
********************
This project is made using nrf Connect SDK version 2.3.0
