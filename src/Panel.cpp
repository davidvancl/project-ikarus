#include "Panel.h"

#include <Arduino.h>
#include <Wire.h>

namespace Panel {

static const uint8_t PCF8574A_ADDR = 0x38;
static const int GREEN_LED_PIN = D8;
static const int RED_LED_PIN = D6;
static const unsigned long BUTTON_INTERVAL = 50;

static unsigned long lastButtonCheck = 0;

static void readButtons() {
  Wire.beginTransmission(PCF8574A_ADDR);
  Wire.write((uint8_t)0b11100111);
  Wire.endTransmission();

  Wire.requestFrom(PCF8574A_ADDR, (uint8_t)1);
  if (!Wire.available()) {
    Serial.println("PCF8574A not responding - check wiring/address");
    return;
  }

  uint8_t state = Wire.read();

  if (!(state & 0x01)) Serial.println("Green button PRESSED");
  if (!(state & 0x02)) Serial.println("Red button PRESSED");
  if (!(state & 0x04)) Serial.println("Key ON");
  if (!(state & 0x20)) Serial.println("Button 1 PRESSED");
  if (!(state & 0x40)) Serial.println("Button 2 PRESSED");
  if (!(state & 0x80)) Serial.println("Button 3 PRESSED");
}

void begin() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  analogWrite(GREEN_LED_PIN, 512);
  analogWrite(RED_LED_PIN, 512);

  Wire.begin(D2, D1);
}

void poll() {
  unsigned long now = millis();
  if (now - lastButtonCheck < BUTTON_INTERVAL) return;
  lastButtonCheck = now;
  readButtons();
}

}
