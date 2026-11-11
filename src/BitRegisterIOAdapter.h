#ifndef BITREGISTERIOADAPTER_H
#define BITREGISTERIOADAPTER_H

#include <Arduino.h>
#include <ILogger.h>

// Event callback type: called when bit state changes
// Parameters: newState (true/false), pin (or VIRTUAL_PIN for virtual bits)
typedef void (*OnStateChangeCallback)(bool newState, uint8_t pin);

#define VIRTUAL_PIN 255  // Special pin number for virtual bits (no hardware)

/**
 * BitRegisterIOAdapter - Single-bit digital I/O abstraction
 * 
 * Features:
 * - Supports both physical pins and virtual bits
 * - Automatic pinMode initialization in constructor
 * - Observable state changes via event callbacks
 * - Multiple subscribers possible
 */
class BitRegisterIOAdapter {
public:
  enum Mode {
    INPUT_MODE = INPUT,
    OUTPUT_MODE = OUTPUT,
    INPUT_PULLUP_MODE = INPUT_PULLUP,
    VIRTUAL_MODE = 254  // Special mode for virtual bits (no hardware access)
  };

  /**
   * Constructor for physical pin
   * @param pin - GPIO pin number (0-53 for Mega, etc.)
   * @param mode - INPUT_MODE, OUTPUT_MODE, INPUT_PULLUP_MODE, or VIRTUAL_MODE
   * @param logger - Optional logger for state change events and initialization messages
   * @param name - Optional log source name; must outlive this adapter (use a string literal)
   */
  BitRegisterIOAdapter(uint8_t pin, Mode mode, ILogger* logger = nullptr, const char* name = nullptr);

  /**
   * Constructor for virtual bit (no hardware)
   * Useful for simulation, testing, or logical bits
   * @param initialState - Starting state (true/false)
   * @param logger - Optional logger for state change events and initialization messages
   * @param name - Optional log source name; must outlive this adapter (use a string literal)
   */
  BitRegisterIOAdapter(bool initialState = false, ILogger* logger = nullptr, const char* name = nullptr);

  // Reject single-integer construction
  BitRegisterIOAdapter(int) = delete;

  // Core state management
  void Set(bool state);
  bool Get();
  void Toggle();

  // Pin information
  uint8_t GetPin() const;
  Mode GetMode() const;
  bool IsVirtual() const;

  // Event subscription
  // Each notification uses a snapshot of the registered callbacks.
  // Adding, removing, or clearing callbacks affects later notifications
  // (including nested ones), but does not change the current snapshot.
  bool OnStateChange(OnStateChangeCallback callback, const char* callbackName = nullptr);
  // Remove the first matching registration; false for nullptr or no match.
  bool RemoveStateChangeCallback(OnStateChangeCallback callback);
  void ClearCallbacks();


private:
  uint8_t pin;
  Mode mode;
  bool currentState;

  // Event callbacks (max 4 subscribers by default)
  static const uint8_t MAX_CALLBACKS = 4;
  struct CallbackRegistration {
    OnStateChangeCallback callback;
    const char* name;
  };
  CallbackRegistration callbacks[MAX_CALLBACKS];
  uint8_t callbackCount = 0;


  // Optional, non-owning dependency.
  // The logger must outlive this BitRegisterIOAdapter.
  ILogger* logger;

  // Optional, non-owning log source name. Null or empty uses the pin-based source.
  const char* name;

  // Internal helper
  void RaiseStateChangeEvent(bool newState);

};

#endif  // BITREGISTERIOADAPTER_H
