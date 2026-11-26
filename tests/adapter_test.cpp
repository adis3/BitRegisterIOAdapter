#include <BitRegisterIOAdapter.h>
#include <stdio.h>
#include <stdlib.h>
static int levels[256] = {};
static int writes = 0;
static int calls = 0;
static bool observed = false;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
void pinMode(uint8_t pin, int mode) {
  if (mode == OUTPUT) CHECK(levels[pin] == (pin == 3 ? HIGH : LOW));
}
void digitalWrite(uint8_t pin, uint8_t value) { levels[pin] = value; ++writes; }
int digitalRead(uint8_t pin) { return levels[pin]; }
void Observe(bool value, uint8_t) { observed = value; ++calls; }
int main() {
  for (int invert = 0; invert < 2; ++invert) {
    const uint8_t pin = invert ? 3 : 2;
    BitRegisterIOAdapter output(pin, BitRegisterIOAdapter::OUTPUT_MODE, nullptr, "output", invert != 0);
    CHECK(!output.Get());
    CHECK(levels[pin] == (invert ? HIGH : LOW));
    CHECK(output.OnStateChange(Observe));
    calls = 0;
    output.Set(true);
    CHECK(output.Get() && observed && calls == 1);
    CHECK(levels[pin] == (invert ? LOW : HIGH));
    int before = writes;
    output.Set(true);
    CHECK(writes == before && calls == 1);
    output.Toggle();
    CHECK(!output.Get() && !observed && calls == 2);
    CHECK(levels[pin] == (invert ? HIGH : LOW));
  }
  int before = writes;
  BitRegisterIOAdapter input(4, BitRegisterIOAdapter::INPUT_MODE, nullptr, nullptr, true);
  levels[4] = HIGH;
  CHECK(input.Get());
  levels[4] = LOW;
  CHECK(!input.Get());
  BitRegisterIOAdapter pullup(5, BitRegisterIOAdapter::INPUT_PULLUP_MODE, nullptr, nullptr, true);
  levels[5] = HIGH;
  CHECK(pullup.Get());
  BitRegisterIOAdapter virtual_mode(6, BitRegisterIOAdapter::VIRTUAL_MODE, nullptr, nullptr, true);
  virtual_mode.Set(true);
  CHECK(virtual_mode.Get());
  BitRegisterIOAdapter virtual_pin(VIRTUAL_PIN, BitRegisterIOAdapter::OUTPUT_MODE, nullptr, nullptr, true);
  virtual_pin.Set(true);
  CHECK(virtual_pin.Get());
  BitRegisterIOAdapter logical(false);
  logical.Set(true);
  CHECK(logical.Get());
  CHECK(writes == before);
  puts("PASS: adapter polarity, initialization, logical callbacks, inputs, virtual bits");
}
