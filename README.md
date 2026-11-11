# BitRegisterIOAdapter Library

Single-bit digital I/O abstraction for Arduino with event system.

## Features

- **Physical or Virtual Bits** - Works with actual GPIO pins or pure software bits
- **Automatic Pin Initialization** - Constructor handles `pinMode()` setup
- **Observable State Changes** - Subscribe callbacks to state transitions
- **Multiple Subscribers** - Up to 4 callbacks per bit (configurable)
- **State History** - Track change count and last change time
- **Memory Efficient** - Minimal overhead for Arduino boards

## Installation

1. Download this repository
2. Copy to `~/Arduino/libraries/BitRegisterIOAdapter/`
3. Restart Arduino IDE
4. Examples available under **File → Examples → BitRegisterIOAdapter**

## Quick Start

### Physical Pin (LED Control)

```cpp
#include <BitRegisterIOAdapter.h>

// Create digital bit on pin 13 as OUTPUT
BitRegisterIOAdapter ledPin(13, BitRegisterIOAdapter::OUTPUT_MODE);

void OnLedChange(bool newState, uint8_t pin) {
  Serial.println(newState ? "LED ON" : "LED OFF");
}

void setup() {
  Serial.begin(9600);
  ledPin.OnStateChange(OnLedChange);  // Subscribe to changes
}

void loop() {
  ledPin.Set(true);   // Turn on
  delay(1000);
  ledPin.Set(false);  // Turn off
  delay(1000);
}
```

### Virtual Bit (Logical State)

```cpp
#include <BitRegisterIOAdapter.h>

// Create virtual bit (no hardware pin)
BitRegisterIOAdapter systemArmed(false);

void OnArmedChange(bool newState, uint8_t pin) {
  Serial.println(newState ? "System ARMED" : "System DISARMED");
}

void setup() {
  Serial.begin(9600);
  systemArmed.OnStateChange(OnArmedChange);
}

void loop() {
  systemArmed.Set(true);   // Logical state change
  delay(2000);
  systemArmed.Set(false);
  delay(2000);
}
```

## API Reference

### Constructors

```cpp
// Physical pin with mode
BitRegisterIOAdapter bit(uint8_t pin, Mode mode);
// Modes: OUTPUT_MODE, INPUT_MODE, INPUT_PULLUP_MODE

// Virtual bit with initial state
BitRegisterIOAdapter bit(bool initialState);
```

### Core Methods

```cpp
void Set(bool state);           // Set bit state
bool Get();                     // Get current state
void Toggle();                  // Toggle state
```

### Pin Information

```cpp
uint8_t GetPin() const;         // Get pin number (VIRTUAL_PIN if virtual)
Mode GetMode() const;           // Get pin mode
bool IsVirtual() const;         // Check if virtual bit
```

### Event Subscription

```cpp
// Subscribe callback
bool OnStateChange(OnStateChangeCallback callback);

// Clear all callbacks
void ClearCallbacks();
```

Callback signature:
```cpp
void MyCallback(bool newState, uint8_t pin) {
  // newState: true or false
  // pin: pin number or VIRTUAL_PIN
}
```

### History


## Examples

### 1. Basic Hardware Control
**File:** `examples/basic_hardware_control/basic_hardware_control.ino`

LED toggled by button press. Demonstrates:
- Physical pin initialization
- Event subscription

### 2. Virtual Bits
**File:** `examples/virtual_bits/virtual_bits.ino`

Simulates system state without hardware. Demonstrates:
- Virtual bit creation
- State changes without GPIO
- Multi-bit coordination

### 3. Multiple Subscribers
**File:** `examples/multi_subscribers/multi_subscribers.ino`

Multiple systems react to same event. Demonstrates:
- Registering multiple callbacks
- Loose coupling between systems
- Event propagation

## Design Patterns

### Observer Pattern
Subscribe multiple handlers to state changes:

```cpp
BitRegisterIOAdapter button(2, BitRegisterIOAdapter::INPUT_PULLUP_MODE);

button.OnStateChange(LogButtonPress);
button.OnStateChange(UpdateUI);
button.OnStateChange(TriggerAlarm);
// All three callbacks fire when button state changes
```

### Virtual Objects
Test logic without hardware:

```cpp
// In production
BitRegisterIOAdapter pump(5, BitRegisterIOAdapter::OUTPUT_MODE);

// In unit test
BitRegisterIOAdapter pumpMock(false);  // Virtual pump
// Test code sees same interface
```

## Memory Usage

- **Per bit:** ~30 bytes base + 4 bytes per callback
- **Max callbacks:** 4 (configurable via MAX_CALLBACKS)
- **Virtual vs Physical:** Same memory footprint

### Typical Usage

```cpp
// Mega 2560: 8KB SRAM
BitRegisterIOAdapter led(13, BitRegisterIOAdapter::OUTPUT_MODE);          // ~30 bytes
BitRegisterIOAdapter button(2, BitRegisterIOAdapter::INPUT_PULLUP_MODE);  // ~30 bytes
BitRegisterIOAdapter alarm(7, BitRegisterIOAdapter::INPUT_MODE);          // ~30 bytes
// Total: ~90 bytes + callbacks
```

## Compatibility

- **Arduino Mega** ✓
- **Arduino Uno** ✓
- **ESP8266** ✓
- **ESP32** ✓
- **ARM boards (SAM, SAMD)** ✓

## License

MIT License - Free for commercial and personal use

## Contributing

Contributions welcome! Please:
1. Fork repository
2. Create feature branch
3. Submit pull request

## Support

Issues or questions? Open an issue on GitHub.
