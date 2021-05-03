# ActiveWindows
This is the combined repository of Team 6's Senior Design Project for Active Windows. There are three distinct subsystems:

1) Android Application - The repository here is blank, and instead redirects you to the app's own repository due to it's size (https://github.com/JonathanClifford/SDP2021_Team6_AndroidApplication)
-> Maintained by Jonathan Clifford
2) The Network Master Controller (NMC) - This contains the source code for the Network Master Controller Board. This is currently running a version of FreeRTOS and is communicating
with AWS via the MQTT protocol, TCP/IP mode for direct connection to phone APP and handles communication to the motor module via a CANBus interface. 
-> Maintained by Connor Moore
3) The Motor Module - Communicates with the NMC via a CANBus interface and actuates a motor based on a user's command.
-> Maintained by Frank Cremonini
