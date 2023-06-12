#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>

const char* ssid = "Mehdi";
const char* pass = "00000001";
const char* brokerUser = "WifiEsp32";
const char* brokerpass = "00000000";
const char* mqttServer = "192.168.43.203";
int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


#define MQTT_PUB_TEMP "esp32/dht/temp"
#define MQTT_PUB_HUM  "esp32/dht/humidity"

DHT my_sensor(15,DHT22,1);

void setupWifi(){
  delay(100);
  
  Serial.print("\nConnecting to Wifi : ");
  Serial.println(ssid);

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print("-");
  }

  Serial.print("\nConnected to ");
  Serial.println(ssid);

}

void reconnect (){
  while (!client.connected()){
    Serial.print("\nConnecting to ");
    Serial.println(mqttServer);
    if(client.connect("ed5019bbb69c40c2affdb0ba40e7074d.s2.eu.hivemq.cloud",brokerUser,brokerpass)){ // c367e12f5b7c43e4a9521205bd257bfe.s2.eu.hivemq.cloud
      Serial.print("\nConnected to ");
      Serial.println(mqttServer);
    } else {
      Serial.println("\nTrying to connect again");
      delay(5000);
    }
  }

}

void setup() {
  Serial.begin(115200);
  
  my_sensor.begin();

  pinMode(LED_BUILTIN,OUTPUT);

  setupWifi(); 
  client.setServer(mqttServer,mqttPort);

}

void loop() {
  digitalWrite(LED_BUILTIN,WiFi.status()== WL_CONNECTED);

  float humidity = my_sensor.readHumidity(0);
  float temperature = my_sensor.readTemperature(0,0);

  if(!client.connected()){
    reconnect();
  }
  
  client.loop();

  client.publish(MQTT_PUB_TEMP, String(temperature).c_str());                            
  Serial.printf("Publishing on topic %s at QoS 1 ", MQTT_PUB_TEMP);
  Serial.printf("Message: %.2f \n", temperature);


  // Publish an MQTT message on topic esp32/dht/humidity
  client.publish(MQTT_PUB_HUM, String(humidity).c_str());                            
  Serial.printf("Publishing on topic %s at QoS 1 ", MQTT_PUB_HUM);
  Serial.printf("Message: %.2f \n", humidity);

  Serial.printf("\n");

  delay(30000);

}