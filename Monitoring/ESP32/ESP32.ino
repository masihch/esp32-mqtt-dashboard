/* If you need any guidance, be sure to message me
 My Website : https://masihch.com 

 The address of the test dashboard for this project is:
 https://lab.masihch.com/monitor-esp32-from-anywhere-in-the-world/index.html
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// --- Sensor config ---
#define DHTPIN 4
#define DHTTYPE DHT11  // or DHT22

// --- WiFi & MQTT config ---
const char* ssid       = "*******"; //your  AP(router) ssid
const char* password   = "*******";; //your  AP(router) pass

const char* mqtt_host  = "broker.masihch.com"; // my free public mqtt broker : domain
const uint16_t mqtt_port = 18883; // my free public mqtt broker : port
const char* mqtt_user  = "freeplan"; // my free public mqtt broker : user
const char* mqtt_pass  = "12345678"; // my free public mqtt broker : pass
const char* mqtt_client_id = "ESP32Client-SSL"; // Device Client ID for Mqtt Connection(Most be unique)

// --- Globals ---
WiFiClientSecure secureClient;   // TLS client
PubSubClient mqttClient(secureClient);
DHT dht(DHTPIN, DHTTYPE);

// Optional: tune keepalive
const uint16_t mqtt_keepalive = 60;  // seconds

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

bool connectMQTT() {
  // Note: setInsecure disables certificate validation (connects via SSL without CA)
  secureClient.setInsecure();

  mqttClient.setServer(mqtt_host, mqtt_port);
  mqttClient.setKeepAlive(mqtt_keepalive);

  Serial.print("Connecting to MQTT (TLS)...");
  // If your broker requires a will message, you can set it here
  bool ok = mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_pass);
  Serial.println(ok ? " connected." : " failed.");
  return ok;
}

void ensureConnections() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqttClient.connected()) {
    connectMQTT();
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin();

  connectWiFi();
  connectMQTT();
}

void loop() {
  ensureConnections();
  mqttClient.loop();

  // Read sensor values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {
    // Build JSON using DynamicJsonDocument
    DynamicJsonDocument doc(256);
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;

    // Optional metadata
    doc["device"] = "esp32";
    doc["sensor"] = (DHTTYPE == DHT22) ? "DHT22" : "DHT11";

    // Serialize to string
    String payload;
    serializeJson(doc, payload);

    // Publish to the secure MQTT broker
    const char* topic = "esp32/sensor";
    bool published = mqttClient.publish(topic, payload.c_str());
    Serial.print("Publish to ");
    Serial.print(topic);
    Serial.print(published ? " OK: " : " FAILED: ");
    Serial.println(payload);
  } else {
    Serial.println("Sensor read failed (NaN).");
  }

  delay(2000); // Send every 2 seconds
}

