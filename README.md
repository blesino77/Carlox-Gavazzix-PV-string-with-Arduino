# Carlox-Gavazzix-PV-string-with-Arduino
Datalogger with Arduino for PV string measurement via Carlox Gavazzix device.

The "config.txt" file is used to set the datetime in the moment you swchith on the Arduino. In this way the datalegger register the timestamp.
The Arduino sketch send a modbus RTU (by the serial port) to de Carlox Gavazzix device, in order to request: the tempearture, voltage and current values. And it stores them into a SD card with the timestamp.

This work was made by David Calvo, and has a GLP license.
It has been developed only for education purposes. The author do not accept any responsibility by the use of this code. 
