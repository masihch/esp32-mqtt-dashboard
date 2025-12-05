/* If you need any guidance, be sure to message me
 My Website : https://masihch.com 

 The address of the test dashboard for this project is:
 https://lab.masihch.com/control-esp32-from-anywhere-in-the-world/index.html
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define BUILTIN_LED 2

const char* ssid = "*****"; //your  AP(router) ssid
const char* password = "*****"; //your  AP(router) pass

const char* mqtt_server = "broker.masihch.com"; // my free public mqtt broker : domain
const int   mqtt_port   = 18883; // my free public mqtt broker : port
const char* mqtt_user   = "freeplan"; // my free public mqtt broker : user
const char* mqtt_pass   = "12345678"; // my free public mqtt broker : pass
const char* mqtt_topic  = "esp32/led"; // subscribed topic 

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (msg == "on") {
    digitalWrite(BUILTIN_LED, HIGH);
  } else if (msg == "off") {
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
