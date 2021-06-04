# Covid-Room-Occupancy o keep a check on the maximum number of people in a single room.
We will first detect the human presence on the main entrance  via Ultrasonic sensors in the doorway.
The idea will be to have two such sensors kept apart to distinguish whether a person has entered a room or left the room.

To increase the precision we will also try to keep a check by measuring the CO2-level.
Further as needed we may also use a Pressure sensor on the mat or a PIR sensor or any other applicable sensors. We are going to implement a buzzer alarm or something similar to indicate that the number people present in a room have exceeded a certain maximum limit.

COMPONENTS USED:
1.ESP32 Pico D4 Development Board
2.Ultrasonic sensors
3.CO-2, Humidity, Temperature sensors
4.Buzzer
5.Connecting wires and supporting components.

Timeline:

May end : Procurement of hardware and finalization of small details and initial work.
June end : Partial software realization.
July Mid: Initial draft of the prototype.
July end : Actual implementation of the functionality.

Test :
Basic test would be to approximately detect the number of people in a room and raise an alarm or
something similar when the number of persons exceed a certain threshold value.

The initial draft of the parts needed can be found in the below link :
https://octopart.com/bom-tool/yxiaWwUz
