#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----------- WiFi instellingen ------------
const char* ssid = "FRITZ!Box 7590 CM";
const char* password = "17313739020905744043";

// ----------- MQTT instellingen ------------
const char* mqtt_server = "192.168.178.52";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt";
const char* mqtt_pass = "mqtt";

// ----------- Domoticz instellingen --------
const char* domoticz_topic = "domoticz/in";
const int domoticz_idx = 304;  // IDX van virtuele deurbel

// ----------- Pin configuratie -------------
const int lightSensorPin = A0;
const int relayPin = D1; // GPIO5

// ----------- Relay timing -----------------
const unsigned long relayOnDuration = 500;   // 0.5 seconden
const unsigned long cooldownDuration = 5000; // 5 seconden cooldown

// ----------- Statusvariabelen -------------
unsigned long lastActivationTime = 0;
bool relayActive = false;
bool previousRelayState = false;
const int lightThreshold = 500; // Pas dit aan indien nodig

// ----------- MQTT client ------------------
WiFiClient espClient;
PubSubClient client(espClient);

// ----------- Functiedeclaraties -----------
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();
void maintainWiFi();
void activateRelay();
void deactivateRelay();
void sendDomoticzRelayStatus(bool status);

// ----------- Setup ------------------------
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // Verbinden met WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Verbinding maken met WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    yield();
  }
  Serial.println("\nWiFi verbonden, IP: " + WiFi.localIP().toString());

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

// ----------- Loop -------------------------
void loop() {
  maintainWiFi();

  if (!client.connected()) {
    reconnectMQTT();
  } else {
    client.loop();
  }

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
  // Eventueel inkomende berichten afhandelen
}

// ----------- NIEUWE Functie: WiFi reconnect ------------
void maintainWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi-verbinding verbroken, opnieuw verbinden...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
      yield();
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi opnieuw verbonden, IP: " + WiFi.localIP().toString());
    } else {
      Serial.println("\nWiFi niet hersteld, probeer later opnieuw.");
    }
  }
}

// ----------- NIEUWE Functie: niet-blokkerende MQTT reconnect ------------
void reconnectMQTT() {
  static unsigned long lastReconnectAttempt = 0;

  if (millis() - lastReconnectAttempt > 5000) { // elke 5s proberen
    lastReconnectAttempt = millis();
    Serial.print("Verbinding maken met MQTT... ");
    if (client.connect("WemosD1", mqtt_user, mqtt_pass)) {
      Serial.println("Verbonden!");
      client.subscribe(domoticz_topic);
    } else {
      Serial.print("Fout, code: ");
      Serial.println(client.state());
    }
  }
}
