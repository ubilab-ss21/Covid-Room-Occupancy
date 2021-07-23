# Covid-Room-Occupancy 

## Main Idea :
To keep a check on the maximum number of people in a single room.

## Implementation :
We will first detect the human presence on the main entrance  via Ultrasonic sensors in the doorway.
The idea will be to have two such sensors kept apart to distinguish whether a person has entered a room or left the room.

To increase the precision we will also try to keep a check by measuring the CO2-level. We are going to implement a audio warning through a buzzer alarm and a visuial alter by LED to indicate that the number people present in a room have exceeded a certain maximum limit.

## COMPONENTS USED:
1.ESP32 Pico D4 Development Board

2.Ultrasonic and CO-2 sensors 

3.Buzzer

4.LEDs

5.Connecting wires and supporting components.


## Timeline:
Week 20-2021 Procurement of hardware and finalization of small details and initial work.

Week 21-2021 Literature analysis and research work for the threshold values for sensors and their efficiency

Week 22-2021 Integration and compatibility check of sensor module on hardware

Week 23-2021 Initial code implementation

Week 24-2021 Initial testing of the output genarated

Week 25-2021 Integration of sensor module and testing the corresponding combined output

Week 26-2021 Improvements in the overall functionality and bug fixes

Week-27-2021 Basic demo of the initial prototype implementation 

Week-28-2021 Final integration and validating the results

Week-29-2021 Final presentation and final demo.


## Evaluation plan :
Milestone 1 : Initial integration of sensor modules to hardware

Milestone 2 : Code Implementation for fectching values from sensors

Milestone 3 : To approximately detect the number of people in a room 

Milestone 4 : Based on sensor values detection of the scenario when the number of persons in a room exceed a certain threshold value.

## BOM Link:
https://octopart.com/bom-tool/yxiaWwUz

## Challenges faced and Solutions:

Accuracy: Sensor HC-SR04 is economical but with limited accuracy. In order to overcome this limitation, we used the temperature values read by C02 sensor. Sound of speed depends on the temperature value and we are getting accurate temperature values with the help of C02 sensor.

Sensor limitation:  UV Sensor has a limitation for accurate distance measurement and also positioning of sensors contribute to the accuracy.  Initially during the setup we had not positioned the sensors properly which resulted in improper detection. After referring to papers and resources we positioned the sensors accordingly and hence overcame this limitation.

## What we could do differently :
We would have used image processing techniques in order to get more accurate results.
 
## References:

1.Datasheet for ESP32-PICO-D4   <br />
2.Datasheet for UV sensor HC SR-O4  <br />
3.Datasheet Sensirion SCD30 Sensor Module <br />
4.Implementation Of Temperature Compensation Technique With Ultrasonic ranging For Obstacle Identification  <br />
   B.Nalini1 , B.Nandhini2 , E.Kavitha3 , Mrs R.Chandralekha4 <br />
5.Evaluation of an Arduino-based IoT Person Counter<br />
  Bruno F. Carvalho, Caio C. M. Silva, Alessandra M. Silva, Fábio Buiati and Rafael Timóteo <br />
6.A Real Time people counting algorithm using Ultrasonic sensors <br />
   Sung-Min Yang, Young-Song Kim, Seo-Hyung Cho,Jae-Hyuk Choi  <br />
7. On the Design of a Cost-Effective and Lightweight People Counting Sensor  <br />
   Sanjana Kadaba Viswanath1, Sai Ram Gubba2, Balasundram Arunn3,Chandra Sekar Veerappan1, and Chau Yuen  <br />



