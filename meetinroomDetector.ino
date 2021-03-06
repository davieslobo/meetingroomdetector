#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

/********* WIFI *************************************/
#define WIFI_PWD "12345678"
#define WIFI_SSID "Telenor_Guest"



/********* MCU specific variables ****************************/
String DEVICENAME = "Kaffekroken";
const int photoDetector = D1;
int led = 0;
bool roomOccupied;
const int BROADCASTDELAY = 3; //Timeinterval of MQTT-broadcastmessage = BROADCASTDELAY * 1 second
int broadcastCounter = 0;

/********* MQTT specific ****************************/
const char* MQTT_DEVICENAME = DEVICENAME.c_str();
const char* MQTT_SERVER = "davies.livsnyter1.no";
const int MQTT_PORT = 1883;



//Set up MQTT-client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  pinMode(photoDetector, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(photoDetector), motionStateChange, CHANGE);

  Serial.begin(115200);

  //setup wifi_connection
  connectWifi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);


  //Setup MQTT-server connection
  mqttConnectBroker();
  getRoomStatus();
}

void loop() {

  while (WiFi.status() == WL_CONNECTED) {
    led = !digitalRead(photoDetector);
    digitalWrite(LED_BUILTIN, led);

    if (broadcastCounter > BROADCASTDELAY) {
      Serial.println(mqttClient.connected());
      if (mqttClient.connected()) {
        mqttConnectBroker();
      }
      broadCastMQTTmsg(DEVICENAME , getRoomStatus());
      broadcastCounter = 0;
    }
    broadcastCounter++;

    Serial.print(MQTT_DEVICENAME); Serial.println(getRoomStatus());
    mqttClient.loop();
    delay(1000);
  }
  connectWifi();

}


/************* Interrupt functions *****************************/

void motionStateChange() {

  roomOccupied = (digitalRead(photoDetector)) ? true : false ;
  Serial.print( ". Statechange @"); Serial.print(DEVICENAME + roomOccupied);

}


/************* MQTT broadcast *****************************/
int broadCastMQTTmsg(String topic, String message) {

  const char *sendMessage = message.c_str();
  const char *sendTopic = topic.c_str();
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT-server");
    if (mqttClient.connect(MQTT_DEVICENAME)) {
      Serial.println("Connected");
    } else {
      mqttConnectBroker();
    }
  }
  mqttClient.publish(sendTopic, sendMessage);
  Serial.print("Message sent to broker "); Serial.print(MQTT_SERVER); Serial.print(":"); Serial.println(MQTT_PORT);
  return mqttClient.state();
}

/************* MQTT callback *****************************/

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("Message recieved:");

  for (int i = 0; i < payloadLength; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


String getRoomStatus() {

  String a = (roomOccupied) ? "Opptatt" : "Ledig";
  return a;
}

void connectWifi() {
  //setup wifi_connection
  WiFi.begin(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }

  Serial.print("Wifi connected : "); Serial.println(WiFi.localIP());
  //  timeClient.begin();
}

void mqttConnectBroker() {
  while (!mqttClient.connected()) {

    if (mqttClient.connect(MQTT_SERVER)) {
      Serial.println("MQTT_Server connected");
    }
    else {
      Serial.print("MQTT-client failed to connect with state "); Serial.println(mqttClient.state());
      delay(200);
    }


  }

}

