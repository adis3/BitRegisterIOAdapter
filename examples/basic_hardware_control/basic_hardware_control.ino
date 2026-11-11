/*
  Basic Physical Digital Bit Example
  
  Demonstrates using BitRegisterIOAdapter with actual hardware pins.
  Toggles an LED when a button is pressed.
*/

#include <BitRegisterIOAdapter.h>

// Create physical digital bits
BitRegisterIOAdapter ledPin(13, BitRegisterIOAdapter::OUTPUT_MODE, nullptr, "ledPin");
BitRegisterIOAdapter buttonPin(2, BitRegisterIOAdapter::INPUT_PULLUP_MODE, nullptr, "buttonPin");

// Callback function for state changes
void OnButtonStateChange(bool newState, uint8_t pin) {
  if (!newState) {  // Button pressed (active LOW with pullup)
    Serial.print("Button pressed! Toggling LED. Pin: ");
    Serial.println(pin);
    ledPin.Toggle();
  }
}

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("BitRegisterIOAdapter Basic Example Started");
  
  // Subscribe to button state changes
  buttonPin.OnStateChange(OnButtonStateChange, "OnButtonStateChange");
  
}

void loop() {
  bool currentButtonState = buttonPin.Get();
  
  delay(10);  // Small delay for responsiveness
}
