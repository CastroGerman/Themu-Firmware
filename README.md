# SmartGlove

- [Stable version](https://github.com/CastroGerman/SmartGlove)

Embedded software development workspace for ESP32

# Test Bluetooth LE

This test implements a Bluetooth Low Energy (BLE) Generic Attribute Profile (GATT) Server on the ESP32. It is designed around one application profile that manages 5 services with characteristics and descriptors within and without the [standard](https://www.bluetooth.com/specifications/gatt/services/).

In addition, this test handles read/write events, including read/write long characteristic request, which divides the outcoming/incoming data into chunks so that the data can fit in the Attribute Protocol (ATT) message.

## Running this test

Running this test will print in terminal most of the BLE events, such as service creation, connections/disconnections, read/write, etc.