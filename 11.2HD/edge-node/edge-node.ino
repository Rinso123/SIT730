#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define PIRPIN 3
#define LDRPIN A0

const char* SSID = "JioFiber-M5xGF5G";
const char* PASS = "Medini@1stJio";
const char* MQTT_SERVER = "192.168.29.133"; // Pi Broker IP

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastSend = 0;
const long interval = 5000; // Send interval (5s)

void setup() { 
  Serial.begin(9600);
  while (!Serial && (millis() < 5000)) {
    ; 
  }
  pinMode(PIRPIN, INPUT);
  dht.begin();
  connectWiFi();
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void connectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(SSID, PASS);
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\nConnected!" : "\nFailed.");
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    connectWiFi(); // Make sure WiFi is active first
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("NanoEdgeNode")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastSend >= interval) {
    lastSend = now;

    // Read Sensors with basic error handling
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int motion = digitalRead(PIRPIN);
    int light = analogRead(LDRPIN);


    bool sensorError = isnan(h) || isnan(t);

    StaticJsonDocument<200> doc;
    doc["humidity"] = sensorError ? -999.0 : h;
    doc["temperature"] = sensorError ? -999.0 : t;
    doc["motion"] = motion;
    doc["light"] = light;
    doc["status"] = sensorError ? "SENSOR_ERROR" : "OK";

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);

    // Serial.println(motion);

    Serial.println("Publishing stats on `home/sesnors`");
    mqttClient.publish("home/sensors", jsonBuffer);
  }
}