# BitRegisterIOAdapter Quick Reference

## Creation

```cpp
// Physical OUTPUT pin (relay, LED, etc.)
BitRegisterIOAdapter ledPin(13, BitRegisterIOAdapter::OUTPUT_MODE);

// Physical INPUT pin (sensor, button, etc.)
BitRegisterIOAdapter buttonPin(2, BitRegisterIOAdapter::INPUT_MODE);

// Physical INPUT with pullup (typical for buttons)
BitRegisterIOAdapter buttonPin(2, BitRegisterIOAdapter::INPUT_PULLUP_MODE);

// Virtual bit (no hardware, for testing or logical state)
BitRegisterIOAdapter systemArmed(false);
```

## Basic Operations

```cpp
ledPin.Set(true);           // Turn ON
ledPin.Set(false);          // Turn OFF
ledPin.Toggle();            // Switch state
bool state = ledPin.Get();  // Read state
```

## Subscribing to Events

```cpp
// Define callback function
void OnStateChange(bool newState, uint8_t pin) {
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.println(newState ? " HIGH" : " LOW");
}

// Subscribe
ledPin.OnStateChange(OnStateChange);

// Multiple subscribers to same bit
valve1.OnStateChange(LogStateChange);      // Logger
valve1.OnStateChange(UpdateDisplay);       // UI
valve1.OnStateChange(NotifyRemote);        // Network
```

## Information Methods

```cpp
uint8_t pin = ledPin.GetPin();                    // Get pin number
bool isVirtual = ledPin.IsVirtual();              // Is it virtual?
```

## Clearing Callbacks

```cpp
ledPin.ClearCallbacks();  // Remove all subscribers
```

## Pin Numbers

```cpp
#define VIRTUAL_PIN 255  // Special value for virtual bits
```

## Common Patterns

### Pattern 1: Simple Toggle (LED)
```cpp
BitRegisterIOAdapter led(13, BitRegisterIOAdapter::OUTPUT_MODE);

void loop() {
  led.Toggle();
  delay(1000);
}
```

### Pattern 2: Button with Callback
```cpp
BitRegisterIOAdapter button(2, BitRegisterIOAdapter::INPUT_PULLUP_MODE);

void OnButtonChange(bool newState, uint8_t pin) {
  if (!newState) {  // Button pressed (active LOW)
    Serial.println("Button pressed!");
  }
}

void setup() {
  button.OnStateChange(OnButtonChange);
}

void loop() {
  button.Get();
}

```

### Pattern 3: Multiple Valves
```cpp
BitRegisterIOAdapter valve1(22, BitRegisterIOAdapter::OUTPUT_MODE);
BitRegisterIOAdapter valve2(23, BitRegisterIOAdapter::OUTPUT_MODE);
BitRegisterIOAdapter valve3(24, BitRegisterIOAdapter::OUTPUT_MODE);

void OpenAll() {
  valve1.Set(true);
  valve2.Set(true);
  valve3.Set(true);
}

void CloseAll() {
  valve1.Set(false);
  valve2.Set(false);
  valve3.Set(false);
}
```

### Pattern 4: Virtual State Machine
```cpp
// Virtual bits representing system state
BitRegisterIOAdapter systemArmed(false);
BitRegisterIOAdapter systemReady(true);
BitRegisterIOAdapter alarmActive(false);

void OnArmChanged(bool newState, uint8_t pin) {
  if (newState) {
    systemReady.Set(true);
  }
}

void OnAlarmChanged(bool newState, uint8_t pin) {
  Serial.println(newState ? "ALARM!" : "Alarm cleared");
}

void setup() {
  systemArmed.OnStateChange(OnArmChanged);
  alarmActive.OnStateChange(OnAlarmChanged);
}
```

### Pattern 5: Coordinator (Multiple Bits, Single Action)
```cpp
BitRegisterIOAdapter pump(5, BitRegisterIOAdapter::OUTPUT_MODE);
BitRegisterIOAdapter boiler(6, BitRegisterIOAdapter::OUTPUT_MODE);
BitRegisterIOAdapter alarm(7, BitRegisterIOAdapter::OUTPUT_MODE);

class HVACController {
public:
  void StartHeating() {
    pump.Set(true);    // Start circulation
    boiler.Set(true);  // Start boiler
    // Both state changes trigger callbacks
  }
  
  void StopHeating() {
    pump.Set(false);
    boiler.Set(false);
  }
};
```

## Testing/Virtual Usage

```cpp
// Create mock bits for testing
BitRegisterIOAdapter mockSensor(false);      // Virtual sensor
BitRegisterIOAdapter mockActuator(false);    // Virtual actuator

// Test code can set states without hardware
mockSensor.Set(true);              // Simulate sensor trigger
bool actuatorResponse = mockActuator.Get();  // Check response
```

## Compatibility

✓ Arduino Mega  
✓ Arduino Uno  
✓ ESP8266  
✓ ESP32  

## Need Help?

1. Check examples: `File → Examples → BitRegisterIOAdapter`
2. Read full docs: `README.md`
