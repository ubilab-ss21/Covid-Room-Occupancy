#include <WiFi.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>

#define wifi_ssid "TP-LINK_37BC"
#define wifi_password "50672851"

#define mqtt_server "earth.informatik.uni-freiburg.de"
//#define mqtt_user "your_username"
//#define mqtt_password "your_password"

#define humidity_topic "ubiblab/sensor/humidity"
#define temperature_topic "ubiblab/sensor/temperature"
#define occupancy_topic "ubiblab/sensor/occupancy"

//#define DHTTYPE DHT22
//#define DHTPIN  14

float newTemp;
float newHum;
char data[64];
int buzzPin =  5;    //Connect Buzzer on Digital Pin5
int occ_counter = 0;

// Allocate the JSON document
  //
  // Inside the brackets, 200 is the capacity of the memory pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<64> jsonBuffer;
WiFiClient espClient;
PubSubClient client(espClient);
//DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

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
  //deserializeJson(doc, payload, length);
  //deserializeJson(doc,);
  //occ_counter = doc["payload"][0];
  //JsonObject& root = jsonBuffer.parseObject(payload);
  //String project_ = doc["project"];
  //Serial.print(project_);
  occ_counter = doc["values"];

  // Print values.
  Serial.print(occ_counter);
}

void setup() {
  Serial.begin(115200);
 // dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

   pinMode(buzzPin, OUTPUT);
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
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(humidity_topic);
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
  if (now - lastMsg > 2000) {
    lastMsg = now; 
    
    if(occ_counter < -5 || occ_counter > 5 ){
      digitalWrite(buzzPin, HIGH);
      delay(100);
      digitalWrite(buzzPin, LOW);
      delay(100);
    }


  }


}