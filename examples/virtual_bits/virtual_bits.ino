/*
  Virtual Bits Example
  
  Demonstrates using BitRegisterIOAdapter for virtual (software) bits.
  Useful for:
  - Simulating hardware without actual pins
  - Testing logic without hardware
  - Logical bits that don't map to pins
  - Debugging and development
*/

#include <BitRegisterIOAdapter.h>

// Create virtual bits (no hardware)
BitRegisterIOAdapter systemArmed(false);        // Virtual bit: system armed state
BitRegisterIOAdapter alarmTriggered(false);     // Virtual bit: alarm state
BitRegisterIOAdapter systemReady(true);         // Virtual bit: system ready

// Callback: Log when system armed state changes
void OnSystemArmedChange(bool newState, uint8_t pin) {
  Serial.print("System armed changed to: ");
  Serial.println(newState ? "ARMED" : "DISARMED");
}

// Callback: Log when alarm triggered
void OnAlarmTriggeredChange(bool newState, uint8_t pin) {
  if (newState) {
    Serial.println("!!! ALARM TRIGGERED !!!");
  } else {
    Serial.println("Alarm cleared");
  }
}

// Callback: Log system ready state
void OnSystemReadyChange(bool newState, uint8_t pin) {
  Serial.print("System ready: ");
  Serial.println(newState ? "YES" : "NO");
}

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("BitRegisterIOAdapter Virtual Bits Example Started");
  
  // Subscribe to state changes
  systemArmed.OnStateChange(OnSystemArmedChange);
  alarmTriggered.OnStateChange(OnAlarmTriggeredChange);
  systemReady.OnStateChange(OnSystemReadyChange);
  
  Serial.println("Initial state:");
  Serial.print("  System Armed: ");
  Serial.println(systemArmed.Get() ? "YES" : "NO");
  Serial.print("  Alarm Triggered: ");
  Serial.println(alarmTriggered.Get() ? "YES" : "NO");
  Serial.print("  System Ready: ");
  Serial.println(systemReady.Get() ? "YES" : "NO");
}

void loop() {
  // Simulate state changes every 5 seconds
  static unsigned long lastChangeTime = 0;
  static uint8_t state = 0;
  
  if (millis() - lastChangeTime > 5000) {
    lastChangeTime = millis();
    
    Serial.println("\n--- State Change ---");
    
    switch (state) {
      case 0:
        systemArmed.Set(true);
        break;
      case 1:
        systemReady.Set(true);
        break;
      case 2:
        alarmTriggered.Set(true);
        break;
      case 3:
        alarmTriggered.Set(false);
        break;
      case 4:
        systemArmed.Set(false);
        break;
      case 5:
        systemReady.Set(false);
        break;
    }

    state = (state + 1) % 6;

  }
}
