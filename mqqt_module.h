
#include <PubSubClient.h>
#include <WiFiClient.h>


/********* MQTT specific ****************************/
const char* MQTT_DEVICENAME;
const char* MQTT_SERVER = "davies.livsnyter1.no";
const int MQTT_PORT = 1883;



void mqqtSetup(){
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT-server");
    if (mqttClient.connect(MQTT_DEVICENAME)) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed with state :"); Serial.println(mqttClient.state());
      delay(2000);
      return mqttClient.state();
    }
  }
}


int broadCastMQTTmsg(String devicename, String topic, String message) {

  const char *sendMessage = message.c_str();
  const char *sendTopic = topic.c_str();
  MQTT_DEVICENAME = devicname;
  
  mqttClient.publish(sendTopic, sendMessage);
  Serial.print("Message sent to broker "); Serial.print(MQTT_SERVER +":"+MQTT_PORT);
  return mqttClient.state();

}

