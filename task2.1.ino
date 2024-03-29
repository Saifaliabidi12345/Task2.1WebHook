#include <WiFiNINA.h>
#include <DHT.h>
#include "secrets.h"
#include "ThingSpeak.h"

#define DHTPIN 2     // Pin connected to DHT sensor
#define DHTTYPE DHT11   // Type of DHT sensor
#define TEMP_ALARM_LOW 20  // Lower temperature threshold for alarm
#define TEMP_ALARM_HIGH 30 // Upper temperature threshold for alarm

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SECRET_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WiFi connection result: ");
    Serial.println(WiFi.begin(SECRET_SSID, SECRET_PASS));

    delay(5000); // Wait 5 seconds before retrying
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  ThingSpeak.begin(client);
  connectWiFi();

  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
}

void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Check if temperature is outside the alarm range
  if (temperature < TEMP_ALARM_LOW || temperature > TEMP_ALARM_HIGH) {
    // Temperature is outside the alarm range, trigger alarm
    Serial.println("Temperature is outside the alarm range!");
    // Here you can add code to trigger an alarm, such as sending an email or turning on a buzzer
  }

  // Set the fields with temperature and humidity data
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

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
