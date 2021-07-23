/************************************************COVID ROOM OCCUPANCY buzzer program******************************************/
/*This program fetch the value of the occupancy counter through MQTT and blows the buzzer if the number of people
/* in the room have exceeded 5

/***********************************************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>

#define wifi_ssid ""
#define wifi_password ""

#define mqtt_server "earth.informatik.uni-freiburg.de"
#define occupancy_topic "ubiblab/sensor/occupancy"

// variables
char data[64];
int buzzPin =  5;    //Connect Buzzer on Digital Pin5
int occ_counter = 0;

// Allocate the JSON document
StaticJsonDocument<64> jsonBuffer;
//Wifi client
WiFiClient espClient;
// MQTT client
PubSubClient client(espClient);

//MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

   // Test if parsing succeeds.
  if (error) {
    Serial.print(("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  // fetch the count value
  occ_counter = doc["values"];
  
  // Print values.
  Serial.println(occ_counter);

}

// setup
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // make the buzzer as output
   pinMode(buzzPin, OUTPUT);
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

// reconnect on disconnection
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
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

long now;
long lastMsg;


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // for every 2 sec
  if (now - lastMsg > 2000) {
    
    lastMsg = now; 
    // make the buzzer pin high if the number of people exceeded 5
    if( occ_counter > 5 ){
      digitalWrite(buzzPin, HIGH);
      delay(100);
      digitalWrite(buzzPin, LOW);
      delay(100);

    }

  }

}
