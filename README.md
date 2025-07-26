# ESP8266 Lichtgestuurde Relais met MQTT (Domoticz-integratie)

Dit project gebruikt een **ESP8266** (bijvoorbeeld Wemos D1 mini) om een relais te schakelen op basis van de lichtintensiteit.  
Daarnaast stuurt het apparaat de status via **MQTT** naar **Domoticz**.

## Functies

- Verbindt automatisch met WiFi en MQTT-broker.
- Meet lichtwaarde via analoge pin **A0**.
- Schakelt relais aan als lichtwaarde **onder een drempel** komt.
- Houdt een **aan-duur** en een **cooldown** aan om te voorkomen dat het relais te vaak schakelt.
- Stuurt relaisstatus naar **Domoticz** via MQTT.
- Herverbindt automatisch bij WiFi- of MQTT-verlies.

## Hardware

- **ESP8266** (Wemos D1 mini / NodeMCU).
- LDR + spanningsdeler (max 1V op A0!).
- Relaismodule geschikt voor 3.3 V aansturing.

| Pin ESP8266 | Functie       |
|-------------|--------------|
| A0          | Lichtsensor  |
| D1 (GPIO5)  | Relais       |

## MQTT & Domoticz

Het script publiceert JSON-berichten naar Domoticz:

```json
{
  "idx": 304,
  "nvalue": 1,
  "svalue": ""
}




const char* ssid = "JouwWiFiSSID";
const char* password = "JouwWiFiWachtwoord";

const char* mqtt_server = "192.168.x.x";
const int mqtt_port = 1883;
const char* mqtt_user = "xxxxxxxx";
const char* mqtt_pass = "xxxxxxxx";

const int lightThreshold = 500;

Compileren & Uploaden
Installeer Arduino IDE of PlatformIO.

Selecteer ESP8266 board (Wemos D1 mini).

Voeg libraries toe:

PubSubClient

Compileer en upload het .ino-bestand.
