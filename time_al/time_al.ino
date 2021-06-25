#include <math.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>

/************************************************ Macros for MQTT******************************************/
//MQTT
#define wifi_ssid "TP-LINK_37BC"
#define wifi_password "50672851"

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define occupancy_topic "ubiblab/sensor/occupancy"
#define uv_topic1 "ubiblab/sensor/uvone"
#define uv_topic2 "ubiblab/sensor/uvtwo"
#define humidity_topic "ubiblab/sensor/humidity"
/***********************************************************************************************************/

//local variables
float newTemp;
char cbuffer[200];
// Allocate the JSON document
// Inside the brackets, 64 is the capacity of the memory pool in bytes.
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
const int trigPin[] = {2,25};
const int echoPin[] = {4,26};
/*******************************************************************************************/

//global variables
float distance[NO_OF_UVSENS];
float UVONE[100];
float UVTWO[100];
float dis1_lowerbound = 0.0;
float dis1_upperbound = 0.0;
float dis2_lowerbound = 0.0;
float dis2_upperbound = 0.0;
int in_times,out_times = 0;
float time_a1,time_a2,time_b1,time_b2;
int people_in = 0, people_out = 0, people_total = 0;
bool in_cond1,in_cond2 = false;
bool out_cond1,out_cond2 = false;
bool eq_sw = false;
int counter=0;
float curr1=-255,curr2=-255,data1,data2;
int N = 100 ;
float avg_dis1,avg_dis2;
int i = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
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

void setup() {
  //connect to broker
  setup_wifi();
  client.setServer(mqtt_server, 1883);
//	client.setCallback(callback);

	for(i = 0; i<NO_OF_UVSENS;i++){
		pinMode(trigPin[i], OUTPUT); // Sets the trigPin as an Output
		pinMode(echoPin[i], INPUT); // Sets the echoPin as an Input
	}
	Serial.begin(115200); // Starts the serial communication

}

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
		pulseDuration = pulseIn( echoPin[sensorNumber], HIGH);
		distance = ((float)(340 * pulseDuration) / 10000) / 2;
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

void isEqual(float dis1, float dis2, float p_dis1, float p_dis2) { 
  if (p_dis1 < dis1_upperbound && p_dis1 > dis1_lowerbound && p_dis2 < dis2_upperbound && p_dis2 > dis2_lowerbound ) {
       eq_sw = true;
  }
  else {
     eq_sw = false;
  }
}
void countPeople(float dis1, float dis2) { 

 if((!in_cond1) && (!in_cond2) && (time_a1 == 0.0) && (dis1 < dis1_lowerbound || dis1 > dis1_upperbound))
 {
   in_cond1 = true;
   time_a1 = millis();
   Serial.print("time_a1");
   Serial.print(" ");
   Serial.println(time_a1);
 }
 
 if((in_cond1) && (!in_cond2) && (time_a2 == 0.0) && (dis1 > dis1_lowerbound && dis1 < dis1_upperbound))
 {
   
   time_a2 = millis();
   Serial.print("time_a2");
   Serial.print(" ");
   Serial.println(time_a2);
   in_cond2 = true;

 }

 if((!out_cond1) && (!out_cond2) && (time_b1 == 0.0)&&(dis2 < dis2_lowerbound || dis2 > dis2_upperbound))
 {
   out_cond1 = true;
   time_b1 = millis();
   Serial.print("time_b1");
   Serial.print(" ");
    Serial.println(time_b1);
 }
  
 if((out_cond1) && (!out_cond2) && (time_b2 == 0.0) && (dis2 > dis2_lowerbound && dis2 < dis2_upperbound))
 {
   
   time_b2 = millis();
   Serial.print("time_b2");
   Serial.print(" ");
   Serial.println(time_b2);
   out_cond2 = true; 
 } 
 
 if( (time_a1 > 0.0 && time_a2 > 0.0) && (time_b1 > 0.0 && time_b2 > 0.0 ) && (!eq_sw)) {
    //Formula
    double r = (abs(time_a1 - time_b1))/(time_a1 - time_b1) + (abs(time_a2 - time_b2))/(time_a2 - time_b2);
   // Serial.print("r");
   // Serial.print(" ");
   // Serial.println(r);
     if(r < 0.0){
        people_out++;
  
     }  
     if(r > 0.0){ 
          people_in++;
     }
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
  data2 = getDistance(UV_TWO);
  
  // read the average distance
  if (counter < N) { 
    UVONE[counter] = data1;
    UVTWO[counter] = data2;
  }

  else if (counter == N) { 
    float sum1 = 0.0, sum2 = 0.0;
    for (int i = 0; i < N; i++) {
      sum1 += UVONE[i];
      sum2 += UVTWO[i];
    }
    avg_dis1 = sum1 / N;
    avg_dis2 = sum2 / N;
    sum1 = 0.0;
    sum2 = 0.0;
    for (int i = 0; i < N; i++) {
      sum1 += pow(UVONE[i] - avg_dis1, 2);
      sum2 += pow(UVTWO[i] - avg_dis2, 2);
    }
    float sd1 = sqrt(sum1 / N) * 6;
    float sd2 = sqrt(sum2 / N) * 6;
    float sd;
    if (sd1 > sd2)
      sd = sd1;
    else
      sd = sd2;
    dis1_lowerbound = avg_dis1 - sd;
    dis1_upperbound = avg_dis1 + sd;
    dis2_lowerbound = avg_dis2 - sd;
    dis2_upperbound = avg_dis2 + sd;

  }else{       
         countPeople(data1,data2);
         isEqual(data1,data2,curr1,curr2);
         people_total = people_out - people_in;
  }
   	counter++;
    // data is error prone
    if(data1>400) {data1 = 0;}
    if(data2>400) {data2 = 0;}

if(counter % 200 == 0){
  Serial.print(data1);
  Serial.print(" ");
  Serial.print(data2);
  Serial.print(" ");
  Serial.println((people_out - people_in));

  int occ_count = (people_out - people_in);
  String value = String((occ_count)); 
  //String payload_ = "{ \"project\": \"covid_room_occupancy\",\"payload\": {" + value + "}}";
  //payload_.toCharArray(cbuffer, (payload_.length() + 1));
  //doc = "{ \"project\": \"covid_room_occupancy\",\"values\": {" + value + "}}";
  doc["values"] = occ_count;
  serializeJson(doc, cbuffer);
  client.publish(occupancy_topic, cbuffer,true);
  // i = i + 5  ; 
  // client.publish(humidity_topic, String(i).c_str(), true);
}


   
  /*
  if(abs(people_out -people_in) > 0){
    Serial.print("No.of people");
    Serial.print(" ");
    Serial.println((people_out - people_in));
  }
 
  Serial.print(avg_dis1);
  Serial.print(" ");
  Serial.print(avg_dis2);
  Serial.print(" ");
  Serial.print(dis1_lowerbound);
  Serial.print(" ");
  Serial.print(dis1_upperbound);
  Serial.print(" ");
  Serial.print(dis2_lowerbound);
  Serial.print(" ");
  Serial.print(dis2_upperbound);
  Serial.print(" ");
  */
  //delay(100);
}