#include "Panel.h"

#include <Arduino.h>
#include <DeviceControl.h>
#include <Wire.h>

namespace Panel {

static const uint8_t PCF8574A_ADDR = 0x38;
static const int GREEN_LED_PIN = D8;
static const int RED_LED_PIN = D6;
static const unsigned long BUTTON_INTERVAL = 50;
static const uint8_t KEY_BIT = 0x04;

struct Button {
  uint8_t bit;
  const char* id;
  const char* name;
};

static const Button BUTTONS[] = {
  {0x01, "greenButton", "Green button"},
  {0x02, "redButton", "Red button"},
  {0x20, "button1", "Button 1"},
  {0x40, "button2", "Button 2"},
  {0x80, "button3", "Button 3"},
};

static unsigned long lastButtonCheck = 0;
static uint8_t lastState = 0xFF;
static int greenPercent = 0;
static int redPercent = 0;

static bool readState(uint8_t& state) {
  Wire.beginTransmission(PCF8574A_ADDR);
  Wire.write((uint8_t)0b11100111);
  Wire.endTransmission();

  Wire.requestFrom(PCF8574A_ADDR, (uint8_t)1);
  if (!Wire.available()) {
    Serial.println("PCF8574A not responding - check wiring/address");
    return false;
  }

  state = Wire.read();
  return true;
}

static void readButtons() {
  uint8_t state;
  if (!readState(state)) return;

  uint8_t pressed = lastState & ~state;
  uint8_t changed = lastState ^ state;
  lastState = state;

  for (const Button& button : BUTTONS) {
    if (!(pressed & button.bit)) continue;
    Serial.printf("%s PRESSED\n", button.name);
    DeviceControl::notify(button.id, true);
  }

  if (changed & KEY_BIT) {
    Serial.println(state & KEY_BIT ? "Key OFF" : "Key ON");
    DeviceControl::notify("key");
  }
}

void begin() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  analogWriteRange(100);
  setGreen(100);
  setRed(100);

  Wire.begin(D2, D1);
}

void poll() {
  unsigned long now = millis();
  if (now - lastButtonCheck < BUTTON_INTERVAL) return;
  lastButtonCheck = now;
  readButtons();
}

void setGreen(int percent) {
  greenPercent = constrain(percent, 0, 100);
  analogWrite(GREEN_LED_PIN, greenPercent);
}

void setRed(int percent) {
  redPercent = constrain(percent, 0, 100);
  analogWrite(RED_LED_PIN, redPercent);
}

int green() {
  return greenPercent;
}

int red() {
  return redPercent;
}

bool keyOn() {
  uint8_t state;
  if (!readState(state)) return false;
  return !(state & KEY_BIT);
}

}
