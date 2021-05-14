# Covid-Room-Occupancy
The goal of this project is to keep in check the maximum number of people in a single room.
We will first detect the human presence on the main entrance  via IR-break beam/Ultrasonic sensor on the doorway.
The idea will be to have two such sensors kept apart to distinguish whether a person has entered a room or left the room.

To increase the precision we will also try to keep a check by measuring the CO2-level.
Further as needed we may also use a  Pressure sensor on the mat , PIR sensor,
maybe and break-beam, or other sensors.A modified version of a PIR sensor if used will be able to also detect the temperature 
of humans and this can also be used to set an alarm or something similar when a human with increased body temerature
is detected in the room.

These are planned to be integrated on a Arduino board with a wifi module and eventually making it an IOT based project.

General test would be to approximately detect the number of people in a room and raise an alarm or 
something similar when the number of persons exceed a certain threshold value.

The initial draft of the parts needed can be found in the below link :
https://octopart.com/bom-tool/yxiaWwUz
