# Carlox-Gavazzix-PV-string-with-Arduino
Datalogger with Arduino for PV string measurement via Carlox Gavazzix (EOS array device).

The "confi.txt" file is used to set the datetime in the moment you swchith on the Arduino. In this way the datalogger registers the timestamp. The file has only one line and data must be 2-digits, the format is: "day;month;year;hour;minute;".

The Arduino sketch send a modbus-RTU frame (by the serial port) to de Carlox Gavazzix device, for that it is necessary a MAX485 shield connected to the Rx-Tx serial port. Data requested are: tempearture, voltage and current values. And they are stored into a SD card with the timestamp. Loop is repeated every 3 minutes.

This work was made by David Calvo, and has a GLP license.
It has been developed only for education purposes. The author do not accept any responsibility by the use of this code. 
