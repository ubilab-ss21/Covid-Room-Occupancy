Covid-Room-Occupancy 

Main Idea :
To keep a check on the maximum number of people in a single room.

Implementation :
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
Week 20-2021 Procurement of hardware and finalization of small details and initial work.

Week 21-2021 Literature analysis and research work for the threshold values for different sensors and their efficiency

Week 22-2021 Integration and compatibility check of sensor moduleon hardware

Week 23-2021 Initial code implementation

Week 24-2021 Initial testing of the output genarated

Week 25-2021 Integration of sensor modules and testing the corresponding combined output

Week 26-2021 Improvements in the overall functionality and bug fixes

Week-27-2021 Basic demo of the initial prototype implementation 

Week-28-2021 Final integration and validating the results

Week-29-2021 Final presentation


Evaluation plan :
Milestone 1 : Initial integration of sensor modules to hardware

Milestone 2 : Code Implementation for fectching values from sensors

Milestone 3 : To approximately detect the number of people in a room 

Milestone 4 : Based on sensor values detection of the scenario when the number of persons in a room exceed a certain threshold value.

BOM Link :
https://octopart.com/bom-tool/yxiaWwUz
