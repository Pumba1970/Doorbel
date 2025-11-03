#include <Arduino.h>

// Define pins
const int lightSensorPin = A0; // Analog pin for LM393 sensor
const int relayPin = 7;        // Digital pin for relay

// Define timing variables
const unsigned long relayOnDuration = 500;   // Relay ON time in milliseconds (0.5 sec)
const unsigned long cooldownDuration = 4000; // Cooldown time in milliseconds (4 sec)

// Variables to manage state
unsigned long lastActivationTime = 0; // Last time relay was activated
bool relayActive = false;             // Relay state

// Threshold for the LM393 light sensor
const int lightThreshold = 500; // Adjust based on your sensor and environment

// Function declarations
void activateRelay();
void deactivateRelay();

void setup() {
  pinMode(relayPin, OUTPUT); // Set relay pin as output
  digitalWrite(relayPin, LOW); // Start with the relay OFF
  Serial.begin(9600);         // Initialize serial monitor for debugging
}

void loop() {
  int lightValue = analogRead(lightSensorPin); // Read light sensor value

  // Debugging: Print light sensor value
  Serial.println(lightValue);

  // Check if light level is below the threshold and cooldown is over
  if (lightValue < lightThreshold && (millis() - lastActivationTime > cooldownDuration)) {
    activateRelay(); // Activate the relay
  }

  // Manage relay deactivation after the ON duration
  if (relayActive && millis() - lastActivationTime >= relayOnDuration) {
    deactivateRelay(); // Turn off the relay
  }
}

void activateRelay() {
  digitalWrite(relayPin, HIGH); // Turn ON the relay
  lastActivationTime = millis(); // Record the activation time
  relayActive = true;            // Mark relay as active
  Serial.println("Relay activated");
}

void deactivateRelay() {
  digitalWrite(relayPin, LOW); // Turn OFF the relay
  relayActive = false;         // Mark relay as inactive
  Serial.println("Relay deactivated");
}
