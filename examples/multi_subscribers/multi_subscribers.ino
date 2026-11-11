/*
  Multiple Subscribers Example
  
  Demonstrates subscribing multiple callbacks to a single BitRegisterIOAdapter.
  Useful for coordinating multiple systems reacting to the same event.
*/

#include <BitRegisterIOAdapter.h>

// Create a virtual bit to simulate a door sensor
BitRegisterIOAdapter doorSensor(false);

// Callback 1: Log door events
void OnDoorStateChange_Logger(bool newState, uint8_t pin) {
  Serial.print("[Logger] Door is now: ");
  Serial.println(newState ? "OPEN" : "CLOSED");
}

// Callback 2: Control light based on door
void OnDoorStateChange_Light(bool newState, uint8_t pin) {
  Serial.print("[Light] Turning ");
  Serial.println(newState ? "ON (door open)" : "OFF (door closed)");
}

// Callback 3: Trigger alarm if door open at night
void OnDoorStateChange_Alarm(bool newState, uint8_t pin) {
  if (newState) {
    Serial.println("[Alarm] WARNING: Door opened!");
  }
}

// Callback 4: Update house status
void OnDoorStateChange_Status(bool newState, uint8_t pin) {
  Serial.print("[Status] House status: ");
  Serial.println(newState ? "DOORS OPEN" : "DOORS CLOSED");
}

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("BitRegisterIOAdapter Multiple Subscribers Example");
  Serial.println("Subscribe multiple callbacks to door sensor...\n");
  
  // Subscribe 4 different callbacks to the same bit
  doorSensor.OnStateChange(OnDoorStateChange_Logger, "OnDoorStateChange_Logger");
  doorSensor.OnStateChange(OnDoorStateChange_Light, "OnDoorStateChange_Light");
  doorSensor.OnStateChange(OnDoorStateChange_Alarm, "OnDoorStateChange_Alarm");
  doorSensor.OnStateChange(OnDoorStateChange_Status, "OnDoorStateChange_Status");
  
  Serial.println("All callbacks registered.\n");
}

void loop() {
  // Toggle door state every 4 seconds
  static unsigned long lastToggleTime = 0;
  
  if (millis() - lastToggleTime > 4000) {
    lastToggleTime = millis();
    
    Serial.println("\n>>> TOGGLING DOOR SENSOR <<<");
    doorSensor.Toggle();
    
    Serial.print("Door state: ");
    Serial.println(doorSensor.Get() ? "OPEN" : "CLOSED");
    Serial.println();
  }
}
