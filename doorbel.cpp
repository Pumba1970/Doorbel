#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----------- WiFi instellingen ------------
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxxxxxx";

// ----------- MQTT instellingen ------------
const char* mqtt_server = "192.168.xxx.xxx";
const int mqtt_port = 1883;
const char* mqtt_user = "xxxxxxxx";
const char* mqtt_pass = "xxxxxxxxt";

// ----------- Domoticz instellingen --------
const char* domoticz_topic = "domoticz/in";
const int domoticz_idx = 304;  // IDX van virtuele deurbel

// ----------- Pin configuratie -------------
const int lightSensorPin = A0;
const int relayPin = D1; // GPIO5

// ----------- Relay timing -----------------
const unsigned long relayOnDuration = 500;   // 0.5 seconden
const unsigned long cooldownDuration = 5000; // 5 seconden cooldown

// ----------- Statusvariabelen ------------
unsigned long lastActivationTime = 0;
bool relayActive = false;
bool previousRelayState = false;
const int lightThreshold = 500; // Pas dit aan indien nodig

// ----------- MQTT client ------------------
WiFiClient espClient;
PubSubClient client(espClient);

// ----------- Functiedeclaraties -----------
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnect();
void activateRelay();
void deactivateRelay();
void sendDomoticzRelayStatus(bool status);

// ----------- Setup ------------------------
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // Verbinden met WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi verbonden");

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  reconnect();
}

// ----------- Loop -------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int lightValue = analogRead(lightSensorPin);

  if (lightValue < lightThreshold && (millis() - lastActivationTime > cooldownDuration)) {
    activateRelay();
  }

  if (relayActive && millis() - lastActivationTime >= relayOnDuration) {
    deactivateRelay();
  }

  if (relayActive != previousRelayState) {
    sendDomoticzRelayStatus(relayActive);
    previousRelayState = relayActive;
  }
}

// ----------- Functies ---------------------
void activateRelay() {
  digitalWrite(relayPin, HIGH);
  lastActivationTime = millis();
  relayActive = true;
  Serial.println("Relais AAN");
}

void deactivateRelay() {
  digitalWrite(relayPin, LOW);
  relayActive = false;
  Serial.println("Relais UIT");
}

void sendDomoticzRelayStatus(bool status) {
  String payload = String("{\"idx\":") + domoticz_idx +
                   ",\"nvalue\":" + (status ? "1" : "0") + ",\"svalue\":\"\"}";
  client.publish(domoticz_topic, payload.c_str());
  Serial.print("Naar Domoticz gestuurd: ");
  Serial.println(payload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Inkomende berichten (optioneel te gebruiken)
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Verbinding maken met MQTT...");
    if (client.connect("WemosD1", mqtt_user, mqtt_pass)) {
      Serial.println("Verbonden!");
      client.subscribe(domoticz_topic); // Abonneren (indien nodig)
    } else {
      Serial.print("Fout, code: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
