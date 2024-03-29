#include <WiFiNINA.h>
#include <DHT.h>
#include "secrets.h"
#include "ThingSpeak.h"

#define DHTPIN 2     
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;


void setup() {
  Serial.begin(115200);
  dht.begin();
  ThingSpeak.begin(client);
  connectWiFi();
}
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(5000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  float temperature = dht.readTemperature();
  // float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature)) {
    //|| isnan(humidity)
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  ThingSpeak.setField(1, temperature);
  // ThingSpeak.setField(2, humidity);

  // Write to ThingSpeak channel
  int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Wait for 20 seconds before updating again
  delay(20000);
}
