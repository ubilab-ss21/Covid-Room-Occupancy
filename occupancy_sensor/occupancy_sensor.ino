/************************************************COVID ROOM OCCUPANCY******************************************/
/* This program uses HC-SR04 sensor to detect the person and the direction of movement.
/* This program integrates the SCD sensor to improve the accuracy of HC-SR04 sensor.

/***********************************************************************************************************/

#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
#include "SCD30.h"

/************************************************ Macros for MQTT******************************************/
//MQTT
#define wifi_ssid "xxx"
#define wifi_password "xxx"

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define occupancy_topic "ubiblab/sensor/occupancy"
#define uv_topic1 "ubiblab/sensor/uvone"
#define uv_topic2 "ubiblab/sensor/uvtwo"

/***********************************************************************************************************/

//local variables
float newTemp;
// buffer array
char cbuffer[200];
// Allocate the JSON document
// Inside the brackets, 200 is the capacity of the memory pool in bytes.
WiFiClient espClient;
PubSubClient client(espClient);
 //JSON
StaticJsonDocument<200> doc;

/* Macros for UV sensor                                                      */
//UV sensors
#define NO_OF_UVSENS 2
#define UV_ONE 0
#define UV_TWO 1

/*************************************** PIN NUMBERS*****************************************/
// defines pins numbers
const int trigPin[] = {14,33}; // define trigger pin for 2 HC-SR04 sensors
const int echoPin[] = {12,32}; // define echo pin for 2 HC-SR04 sensors
/*******************************************************************************************/

//global variables
float distance[NO_OF_UVSENS]; // variable to hold the distance from the UV sensor
float UVONE[100]; //array to hold the distance for sensor 1
float UVTWO[100]; //array to hold the distance for sensor 2

// varaible used to convert the distance into binary values
float sensor1_lowerbound = 0.0;
float sensor1_upperbound = 0.0;
float sensor2_lowerbound = 0.0;
float sensor2_upperbound = 0.0;

float time_a1,time_a2,time_b1,time_b2; // variables to hold the time at the direction of movement
int people_in = 0, people_out = 0, people_total = 0; // variables to hold the direction of movement

// bool varaibles
bool in_cond1,in_cond2 = false;
bool out_cond1,out_cond2 = false;

int counter=0; //  varaible to count the number of iterations
float curr1=-255,curr2=-255,data1,data2; //
int NUMITER = 100 ; // number of iterations
float averageDistance1,averageDistance2; // average distance
int i = 0; 
// variable to check if the distance is within the bounds 
int avgCheck = 0;
// array to hold the temperature,humidity and ppm values
float ccc[3] = {0};
float ppm;
float temperature;
float humidity;
float soundSpeed;

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//setup wifi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// reconnection after disconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    if (client.connect("espClient")) {
      Serial.println("connected");
      client.subscribe(occupancy_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//setup
void setup() {
  //connect to broker
  setup_wifi();
  client.setServer(mqtt_server, 1883);
//	client.setCallback(callback);
  
  Wire.begin();
  Serial.println("SCD30 Raw Data");
  scd30.initialize();

	for(i = 0; i<NO_OF_UVSENS;i++){
		pinMode(trigPin[i], OUTPUT); // Sets the trigPin as an Output
		pinMode(echoPin[i], INPUT); // Sets the echoPin as an Input
	}
	Serial.begin(115200); // Starts the serial communication

}

//This function returns the distance
float getDistance(int sensorNumber)
{
	// defines variables
	long pulseDuration;
	long distance;
	int tries = 0;

	// Clears the trigPin
	digitalWrite(trigPin[sensorNumber], LOW);
	delayMicroseconds(2);

	// Sets the trigPin on HIGH state for 10 micro seconds
	digitalWrite(trigPin[sensorNumber], HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin[sensorNumber], LOW);
	do
	{
		// set the echo pin HIGH 
		pulseDuration = pulseIn( echoPin[sensorNumber], HIGH);
    soundSpeed =  331.3 + (0.606 * temperature);
		distance = ((float)pulseDuration*(soundSpeed / 10000)) / 2;
		if ( pulseDuration == 0 ) {
			delay(100);
			pinMode(echoPin[sensorNumber], OUTPUT);
			digitalWrite(echoPin[sensorNumber], LOW);
			delay(100);
			pinMode(echoPin[sensorNumber], INPUT);
		}
	} while (pulseDuration == 0 && ++tries < 3);

	// Reads the echoPin, returns the sound wave travel time in microseconds
	if(pulseDuration == 0){
    Serial.print(sensorNumber);
		Serial.println("Error");
	}else {
		return distance;
	}
	return -1;
}

//This function implements the algorithm used to detect people and the direction of movement
void countPeople(float dis1, float dis2) { 

 // if the dis1 is not in the range and time is not set for a1
 if((!in_cond1) && (!in_cond2) && (time_a1 == 0.0) && (dis1 < sensor1_lowerbound || dis1 > sensor1_upperbound))
 {  
   in_cond1 = true;
   // fetch the time when the sensor 1 goes out of range
   time_a1 = millis();
   Serial.print("time_a1");
   Serial.print(" ");
   Serial.println(time_a1);
 }
 
 // if the dis1 in the range after going out of range
 if((in_cond1) && (!in_cond2) && (time_a2 == 0.0) && (dis1 > sensor1_lowerbound && dis1 < sensor1_upperbound))
 {
    // fetch the time when the sensor 1 is in the range
   time_a2 = millis();
   Serial.print("time_a2");
   Serial.print(" ");
   Serial.println(time_a2);
   in_cond2 = true;

 }

// if the dis2 is not in the range and time is not set for a1
 if((!out_cond1) && (!out_cond2) && (time_b1 == 0.0)&&(dis2 < sensor2_lowerbound || dis2 > sensor2_upperbound))
 {
   out_cond1 = true;
   // fetch the time when the sensor2 goes out of range
   time_b1 = millis();
   Serial.print("time_b1");
   Serial.print(" ");
    Serial.println(time_b1);
 }
  
   // if the dis2 in the range after going out of range
 if((out_cond1) && (!out_cond2) && (time_b2 == 0.0) && (dis2 > sensor2_lowerbound && dis2 < sensor2_upperbound))
 {
    // fetch the time when the sensor 2 is in the range
   time_b2 = millis();
   Serial.print("time_b2");
   Serial.print(" ");
   Serial.println(time_b2);
   out_cond2 = true; 
 } 
 
 // if all the sensors have triggered calculate the count
 if((time_a1 > 0.0 && time_a2 > 0.0) && (time_b1 > 0.0 && time_b2 > 0.0 )) 
 {
    //Formula
    double r = (abs(time_a1 - time_b1))/(time_a1 - time_b1) + (abs(time_a2 - time_b2))/(time_a2 - time_b2);
     
	 // count is decreased if r is less than 0
     if(r < 0.0){
        people_out++;
  
     }
    // count is increased if r is greater than 0	 
     if(r > 0.0){ 
          people_in++;
     }
	    // reset all flags
        time_a1 = 0;time_a2 = 0;time_b1=0;time_b2 = 0;
        in_cond1 = false;
        in_cond2 = false;
        out_cond1 = false;
        out_cond2 = false;
 }
  
  // reset all flags if the distance of both sensors in the range for 4 number of iterations
  if (dis1 < sensor1_upperbound && dis1 > sensor1_lowerbound && dis2 < sensor2_upperbound && dis2 > sensor2_lowerbound) 
    avgCheck++;
  else
    avgCheck = 0;
  if (avgCheck == 4){
    avgCheck = 0;
    time_a1 = 0;time_a2 = 0;time_b1=0;time_b2 = 0;
    in_cond1 = false;
    in_cond2 = false;
    out_cond1 = false;
    out_cond2 = false;
  }

}

void loop() {
	
  // connect client to MQTT server
  if (!client.connected()) {
    reconnect();
 }
  client.loop();

	//set the current equal to the data
	curr1 = data1;
	curr2 = data2;

  // read the distance from the UV sensor
  data1 = getDistance(UV_ONE);
  delay(60);
  data2 = getDistance(UV_TWO);
  
  // store the distance into array for all iterations less than NUMITER
  if (counter < NUMITER) { 
    UVONE[counter] = data1;
    UVTWO[counter] = data2;
  }
  // calculate average distance if the iterations is equal to NUMITER
  else if (counter == NUMITER) { 
    float total1 = 0.0, total2 = 0.0;
	// sum all the distances
    for (int i = 0; i < NUMITER; i++) {
      total1 += UVONE[i];
      total2 += UVTWO[i];
    }
	// calculate average distances
    averageDistance1 = total1 / NUMITER;
    averageDistance2 = total2 / NUMITER;
    
     //calculate standardDeviation and set the lower and upper limit
    total1 = 0.0;
    total2 = 0.0;
    for (int i = 0; i < NUMITER; i++) {
      total1 += pow(UVONE[i] - averageDistance1, 2);
      total2 += pow(UVTWO[i] - averageDistance2, 2);
    }
    float standardDeviation1 = sqrt(total2 / NUMITER) * 6;
    float standardDeviation2 = sqrt(total2 / NUMITER) * 6;
    float standardDeviation;
    if (standardDeviation1 > standardDeviation2)
      standardDeviation = standardDeviation1;
    else
      standardDeviation = standardDeviation2;
    sensor1_lowerbound = averageDistance1 - standardDeviation;
    sensor1_upperbound = averageDistance1 + standardDeviation;
    sensor2_lowerbound = averageDistance2 - standardDeviation;
    sensor2_upperbound = averageDistance2 + standardDeviation;

  }else
	  // for all iterations greater than NUMITER
  {       
         countPeople(data1,data2);
         people_total = people_out - people_in;       
  }
   	counter++;
    // data is error prone
    if(data1>400) {
		data1 = 0;
		}
    if(data2>400) {
		data2 = 0;
		}

if(counter % 25 == 0){
  int occ_count = (people_out - people_in);
  String value = String((occ_count)); 
   
  if(scd30.isAvailable()){
	scd30.getCarbonDioxideConcentration(ccc); 
    ppm =  ccc[0];
    temperature = ccc[1];
    humidity = ccc[2];   

    doc["ppm"] =  ppm;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;   
  }
 
  doc["values"] = occ_count;
  doc["uvone"] = data1;
  doc["uvtwo"] = data2;
  serializeJson(doc, cbuffer);
  client.publish(occupancy_topic, cbuffer,true);
   Serial.print("No.of people");
   Serial.print(" ");
   Serial.println((people_out - people_in));
}

// so that we dont flood the serial port
if(counter == 125){
  Serial.print(averageDistance1);
  Serial.print(" ");
  Serial.print(averageDistance2);
  Serial.print(" ");
  Serial.print(sensor1_lowerbound);
  Serial.print(" ");
  Serial.print(sensor1_upperbound);
  Serial.print(" ");
  Serial.print(sensor2_lowerbound);
  Serial.print(" ");
  Serial.print(sensor2_upperbound);
  Serial.print(" ");
}

  
}
