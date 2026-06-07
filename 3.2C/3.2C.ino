#include <WiFiNINA.h>
#include <PubSubClient.h>

const char* myName = "rinsorinco";

char ssid[] = "Pixel_9715";

char MQTT_SERVER[] = "broker.emqx.io";
int MQTT_PORT = 1883;
char* MQTT_TOPIC_WAVE = "ES/Wave";
char* MQTT_TOPIC_PAT = "ES/Pat";

char IFFT_HOST[] = "maker.ifttt.com";
String IFFT_PATH = "/trigger/3wave/with/key/5pwrVvgV7MTNS00sAB-yG";  // Event name set in IFTTT

#define TRIGGER 5
#define ECHO 6

#define BATHROOM_LED 2
#define HALLWAY_LED 3

#define DIST_THRESHOLD_PAT_MAX 12
#define DIST_THRESHOLD_WAVE_MAX 30

bool gestureTriggered = false;
int waveCount = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiSSLClient sslClient;

void send_3wave_event() {
  Serial.println("Sending 3 wave received event to IFFT");
  wifiClient.connect(IFFT_HOST, 80);

  wifiClient.println("GET " + IFFT_PATH + " HTTP/1.1");
  wifiClient.println("Host: " + String(IFFT_HOST));
  wifiClient.println("Connection: close");
  wifiClient.println();


  while (wifiClient.connected()) {
    if (wifiClient.available()) {
      char ch = wifiClient.read();
      Serial.print(ch);
    }
  }
}

void set_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == MQTT_TOPIC_WAVE) {
    digitalWrite(BATHROOM_LED, HIGH);
    digitalWrite(HALLWAY_LED, HIGH);
    Serial.println("Action: Both LEDs turned ON");

    waveCount++;
    Serial.print("SIT730 Tracker: Wave detected. State sequence: (");
    Serial.print(waveCount);
    Serial.println("/3)");

    if (waveCount >= 3) {
      send_3wave_event();
      waveCount = 0;  // Clear state machine for safety
    }
  } else if (String(topic) == MQTT_TOPIC_PAT) {
    digitalWrite(BATHROOM_LED, LOW);
    digitalWrite(HALLWAY_LED, LOW);
    Serial.println("Action: Both LEDs turned OFF");
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "SIT730-Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe(MQTT_TOPIC_WAVE);
      mqttClient.subscribe(MQTT_TOPIC_PAT);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

long getDistance() {
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  if (duration == 0) return 999;

  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);


  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BATHROOM_LED, OUTPUT);
  pinMode(HALLWAY_LED, OUTPUT);

  digitalWrite(BATHROOM_LED, LOW);
  digitalWrite(HALLWAY_LED, LOW);

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void loop() {

  set_wifi();

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  long distance = getDistance();
  Serial.println(distance);
  if (distance >= 2 && distance <= DIST_THRESHOLD_WAVE_MAX) {
    if (!gestureTriggered) {
      gestureTriggered = true;

      if (distance <= DIST_THRESHOLD_PAT_MAX) {
        Serial.print("Local Gesture Detected: PAT (Distance: ");
        Serial.print(distance);
        Serial.println("cm)");
        mqttClient.publish(MQTT_TOPIC_PAT, myName);
      } else {
        Serial.print("Local Gesture Detected: WAVE (Distance: ");
        Serial.print(distance);
        Serial.println("cm)");
        mqttClient.publish(MQTT_TOPIC_WAVE, myName);
      }
    }
  } else {
    gestureTriggered = false;
  }

  delay(50);
}