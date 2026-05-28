#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>

BH1750 lightMeter;

char ssid[] = "Pixel_9715";

WiFiClient client;

char HOST_NAME[] = "maker.ifttt.com";
String RECIEVED_SUNLIGHT_PATH_NAME = "/trigger/received_sunlight/with/key/5pwrVvgV7MTNS00sAB-yG";
String LOST_SUNLIGHT_PATH_NAME = "/trigger/lost_sunlight/with/key/5pwrVvgV7MTNS00sAB-yG";


#define DARKNESS_THRESHOLD 30

bool was_dark = false;

void send_received_sunlight_event(WiFiClient c) {
  Serial.println("Sending sunlight received event to IFFT");
  c.connect(HOST_NAME, 80);

  c.println("GET " + RECIEVED_SUNLIGHT_PATH_NAME + " HTTP/1.1");
  c.println("Host: " + String(HOST_NAME));
  c.println("Connection: close");
  c.println();


  while (c.connected()) {
    if (c.available()) {
      char ch = c.read();
      Serial.print(ch);
    }
  }
}


void send_lost_sunlight_event(WiFiClient c) {
  Serial.println("Sending sunlight lost event to IFFT");

  c.connect(HOST_NAME, 80);

  c.println("GET " + LOST_SUNLIGHT_PATH_NAME + " HTTP/1.1");
  c.println("Host: " + String(HOST_NAME));
  c.println("Connection: close");
  c.println();


  while (c.connected()) {
    if (c.available()) {
      char ch = c.read();
      Serial.print(ch);
    }
  }
}



void setup() {
  Serial.begin(9600);

  Wire.begin();

  lightMeter.begin(BH1750::CONTINUOUS_LOW_RES_MODE);
}

void loop() {
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

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (lux <= DARKNESS_THRESHOLD) {
    if (!was_dark) {
      send_lost_sunlight_event(client);
      was_dark = true;
    }
  } else {
    if (was_dark) {
      send_received_sunlight_event(client);
      was_dark = false;
    }
  }


  delay(1000);
}