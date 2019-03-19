# senseBox-Project
This contains the arduino coding for Road profiler application.
This was created during my study project where I got to know about the SenseBox MCU.
After completion of the coding in Arduino IDE, I implemented
the code in the board through the wire transmission. Then with my team members, I
implemented the output and displayed the vertical axis fluctuations of the accelerometer in the
two types of the road in the graph. We took almost four kilometers to get the values by mounting
the senseBox MCU over the bike. As it was static data it will not give the live stream in the
application. So, objects for the Adafruit IO had been created where the data from the board are
visualized in the server. Adafruit MQTT broker setup has been made then the data from the
board got where the server port connected, then the data had been displayed then.
