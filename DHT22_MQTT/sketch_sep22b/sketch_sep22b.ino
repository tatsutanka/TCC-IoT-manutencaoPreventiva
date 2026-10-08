/*
 * This ESP8266 NodeMCU code was developed by newbiely.com
 *
 * This ESP8266 NodeMCU code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/esp8266/esp8266-mqtt
 */

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#define DHT_SENSOR_PIN  D7 // The ESP8266 pin D7 connected to DHT22 sensor
#define DHT_SENSOR_TYPE DHT22

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

const char WIFI_SSID[] = "Tanaka";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "talumu53";  // CHANGE TO YOUR WIFI PASSWORD

 const char MQTT_BROKER_ADRRESS[] = "192.168.1.135";  // CHANGE TO MQTT BROKER'S ADDRESS
//const char MQTT_BROKER_ADRRESS[] = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "dht22-esp8266-001";  // CHANGE IT AS YOU DESIRE
const char MQTT_USERNAME[] = "esp_user";                        // CHANGE IT IF REQUIRED, empty if not required
const char MQTT_PASSWORD[] = "talumu53";                        // CHANGE IT IF REQUIRED, empty if not required

// The MQTT topics that ESP8266 should publish/subscribe
const char PUBLISH_TOPIC[] = "dht22-esp8266-001/loopback";    // CHANGE IT AS YOU DESIRE
 const char SUBSCRIBE_TOPIC[] = "dht22-esp8266-001/loopback";  // CHANGE IT AS YOU DESIRE

const int PUBLISH_INTERVAL = 5000;  // 5 seconds

const int KEEP_ALIVE = 5; // tempoem que ele ficara mandando informações ate que mande um ping para averiguar se existe conexão

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

unsigned long lastPublishTime = 0;

void setup() {

  Serial.begin(9600);

  dht_sensor.begin(); // initialize the DHT sensor

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("ESP8266 - Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  connectToMQTT();
}

void loop() {
  mqtt.loop();
  //if (!mqtt.connected() || !mqtt.loop()) {
   // Serial.println("Queda de conexão detectada pelo loop()!");
    //(); // Força a rotina de reconexão
  //}
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }
}


void connectToMQTT() {
  // Connect to the MQTT broker

  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  // Create a handler for incoming messages
  mqtt.onMessage(messageHandler);

  Serial.print("ESP8266 - Connecting to MQTT broker");

  // keep alive
  mqtt.setKeepAlive(KEEP_ALIVE);

  while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (!mqtt.connected()) {
    Serial.println("ESP8266 - MQTT broker Timeout!");
    return;
  }

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  if (mqtt.subscribe(SUBSCRIBE_TOPIC))
    Serial.print("ESP8266 - Subscribed to the topic: ");
  else
    Serial.print("ESP8266 - Failed to subscribe to the topic: ");

  Serial.println(SUBSCRIBE_TOPIC);
  Serial.println("ESP8266 - MQTT broker Connected!");
}

void sendToMQTT() {
  float humi  = dht_sensor.readHumidity();
  // read temperature in Celsius
  float temperature_C = dht_sensor.readTemperature();
  // read temperature in Fahrenheit
  float temperature_F = dht_sensor.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(temperature_C) || isnan(temperature_F) || isnan(humi)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temperature_C);
    Serial.print("°C  ~  ");
    Serial.print(temperature_F);
    Serial.println("°F");
  }



  StaticJsonDocument<200> message;
  message["timestamp"] = millis();
  //message["data"] = analogRead(0);  // Or you can read data from other sensors
  message["humi"] = humi;
  message["temperature_c"] = temperature_C;
  message["temperatura_f"] = temperature_F;
  message["cor"] = "azul";
  char messageBuffer[512];
  serializeJson(message, messageBuffer); // transforma na mensagem do buffer que era mandada

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);
  // output of info 
  Serial.println("ESP8266 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("ESP8266 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);
}
