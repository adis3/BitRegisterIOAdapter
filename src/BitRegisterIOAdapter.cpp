#include "BitRegisterIOAdapter.h"

namespace {
  void LogBit(ILogger* logger, LogLevel level, uint8_t pin, const char* pinName, const char* message) {
    // Limit the name so the pin or Virtual suffix always fits.
    char source[82];
    const bool hasName = pinName != nullptr && pinName[0] != '\0';
    const char* separator = hasName ? " " : "";
    const char* logName = hasName ? pinName : "";
    if (pin == VIRTUAL_PIN) {
      snprintf(source, sizeof(source), "BitRegisterIOAdapter%s'%.50s' Virtual", separator, logName);
    } else {
      snprintf(source, sizeof(source), "BitRegisterIOAdapter%s'%.50s' pin:%u", separator, logName, static_cast<unsigned int>(pin));
    }
    logger->Log(level, source, message);
  }
  void LogCallbackAction(ILogger* logger, uint8_t pin, const char* pinName, const char* action, const char* callbackName) {
    char message[76];
    bool missingCallbackName = callbackName == nullptr || callbackName[0] == '\0';
    bool missingAction = action == nullptr || action[0] == '\0';
    snprintf(message, sizeof(message), "%.50s callback %s", missingCallbackName ? "Missing name" : callbackName, missingAction ? " missing action" : action);
    LogBit(logger, LogLevel::Debug, pin, pinName, message);
  }    
}

// Constructor for physical pin
BitRegisterIOAdapter::BitRegisterIOAdapter(uint8_t pin, BitRegisterIOAdapter::Mode mode, ILogger* logger, const char* name) 
  : pin(pin), 
    mode(mode), 
    currentState(false),
    callbackCount(0),
    logger(logger),
    name(name) {
  // Initialize hardware pin if not virtual
  if (!IsVirtual()) {
    ::pinMode(pin, mode);
    // Read initial state from pin
    if (mode == INPUT_MODE || mode == INPUT_PULLUP_MODE) {
      currentState = ::digitalRead(pin) == HIGH;
    } else if (mode == OUTPUT_MODE) {
      ::digitalWrite(pin, currentState ? HIGH : LOW);
    }
  }
  // Initialize callbacks array
  for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
    callbacks[i] = { nullptr, nullptr };
  }
  if (logger != nullptr) {
    LogBit(logger, LogLevel::Info, pin, name,
      currentState
        ? "Initialized with state=true"
        : "Initialized with state=false");
  }  
}

// Constructor for virtual bit (no hardware)
BitRegisterIOAdapter::BitRegisterIOAdapter(bool initialState, ILogger* logger, const char* name) 
  : pin(VIRTUAL_PIN),
    mode(VIRTUAL_MODE), 
    currentState(initialState),
    callbackCount(0),
    logger(logger),
    name(name) {
  // Initialize callbacks array
  for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
    callbacks[i] = { nullptr, nullptr };
  }
  if (logger != nullptr) {
    LogBit(logger, LogLevel::Info, VIRTUAL_PIN, name,
      currentState
        ? "Initialized with state=true"
        : "Initialized with state=false");
  }
}

// Set the bit state
void BitRegisterIOAdapter::Set(bool state) {
  if (currentState != state) {
    currentState = state;
    // Write to hardware if not virtual
    if (!IsVirtual()) {
      if (mode == OUTPUT_MODE) {
        ::digitalWrite(pin, state ? HIGH : LOW);
      } else {
        if (logger != nullptr) {
          LogBit(logger, LogLevel::Error, pin, name, "Attempted to set state on non-output hardware pin");
        }
      }
    }
    if (logger != nullptr) {
      LogBit(logger, LogLevel::Debug, pin, name,
        state
          ? "State Set(): false -> true"
          : "State Set(): true -> false");
    }    
    RaiseStateChangeEvent(state);
  }
}

// Get current bit state
bool BitRegisterIOAdapter::Get() {
  if (!IsVirtual() && (mode == INPUT_MODE || mode == INPUT_PULLUP_MODE)) {
    bool newState = ::digitalRead(pin) == HIGH;
    if (newState != currentState) {
      currentState = newState;
      if (logger != nullptr) {
        LogBit(logger,
          LogLevel::Debug,
          pin,
          name,
          currentState
              ? "State Get(): false -> true"
              : "State Get(): true -> false");
      }
      RaiseStateChangeEvent(currentState);
    }
  }
  return currentState;
}

// Toggle the bit state
void BitRegisterIOAdapter::Toggle() {
  if (logger != nullptr) {
    LogBit(logger, LogLevel::Debug, pin, name, "State Toggle");
  }
  Set(!currentState);
}

// Get the pin number
uint8_t BitRegisterIOAdapter::GetPin() const {
  return pin;
}

// Get the pin mode
BitRegisterIOAdapter::Mode BitRegisterIOAdapter::GetMode() const {
  return mode;
}

// Check if this is a virtual bit
bool BitRegisterIOAdapter::IsVirtual() const {
  return pin == VIRTUAL_PIN || mode == VIRTUAL_MODE;
}

// Subscribe to state change events
bool BitRegisterIOAdapter::OnStateChange(OnStateChangeCallback callback, const char* callbackName) {
  if (callback == nullptr) {
    if (logger != nullptr) {
      LogBit(logger, LogLevel::Error, pin, name, "OnStateChange callback is null");
    }
    return false;
  }
  if (callbackCount >= MAX_CALLBACKS) {
    if (logger != nullptr) {
      LogBit(logger, LogLevel::Error, pin, name, "OnStateChange callback limit reached");
    }
    return false;
  }
  callbacks[callbackCount] = {callback, callbackName};
  callbackCount++;
  if (logger != nullptr) {
    LogCallbackAction(logger, pin, name, "added", callbackName);
  }
  return true;
}

// Unsubscribe the first matching state change callback.
bool BitRegisterIOAdapter::RemoveStateChangeCallback(OnStateChangeCallback callback) {
  if (callback == nullptr) {
    if (logger != nullptr) {
      LogBit(logger, LogLevel::Error, pin, name, "RemoveStateChangeCallback callback is null");
    }
    return false;
  }

  for (uint8_t i = 0; i < callbackCount; i++) {
    if (callbacks[i].callback == callback) {
      if (logger != nullptr) {
        LogCallbackAction(logger, pin, name, "removed", callbacks[i].name);
      }
      for (uint8_t j = i + 1; j < callbackCount; j++) {
        callbacks[j - 1] = callbacks[j];
      }
      callbacks[--callbackCount] = {nullptr, nullptr};
      return true;
    }
  }
  if (logger != nullptr) {
    LogBit(logger, LogLevel::Debug, pin, name, "RemoveStateChangeCallback callback not found");
  }
  return false;
}

// Clear all event callbacks
void BitRegisterIOAdapter::ClearCallbacks() {
  for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
    callbacks[i] = {nullptr, nullptr};
  }
  callbackCount = 0;
  if (logger != nullptr) {
    LogBit(logger, LogLevel::Debug, pin, name, "All callbacks cleared");
  }
}

// Internal: Raise state change event to all subscribers
void BitRegisterIOAdapter::RaiseStateChangeEvent(bool newState) {
  const uint8_t count = callbackCount;
  CallbackRegistration tempCallbacks[MAX_CALLBACKS];
  for(uint8_t i = 0; i < count; i++) {
    tempCallbacks[i] = callbacks[i];
  }
  for (uint8_t i = 0; i < count; i++) {
    if (tempCallbacks[i].callback != nullptr) {
      tempCallbacks[i].callback(newState, pin);
      if (logger != nullptr) {
        LogCallbackAction(logger, pin, name, "invoked", tempCallbacks[i].name);
      }
    }
  }
}
