#include "DHT.h"
#include "ThingSpeak.h"
#include <SPI.h>
#include <WiFiNINA.h>

#define SECRET_CH_ID 3382998  
#define SECRET_WRITE_APIKEY "PI7O5W9JG99CS0J0"

#define DHTPIN 2
#define DHTTYPE DHT11

#define LDRPIN A0

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "Pixel_9715";

WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;



void setup() {
  Serial.begin(9600);  // Initialize serial
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  dht.begin();

  ThingSpeak.begin(client);  // Initialize ThingSpeak
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


  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C "));


  int lightSensorValue = analogRead(LDRPIN);
  Serial.print(" | Light Level: ");
  Serial.println(lightSensorValue);

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, lightSensorValue);

   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(15000);
}
