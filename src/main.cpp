#include "Network.h"
#include "arduino_secrets.h"

#include <LedControl.h>
#include <Wire.h>

#define PCF8574A_ADDR 0x38

Network network;
LedControl lc = LedControl(13, 14, 2, 2); // D7, D5, D4, počet čipů = 2

const int greenLedPin = D8;
const int redLedPin = D6;

int topNumber = 1;      // čip 0: oba 220801K displeje, 1-99
int bottomNumber = 1;   // čip 1: oba SH5461AS displeje, 1-9999

unsigned long lastDisplayUpdate = 0;
unsigned long lastButtonCheck = 0;

const unsigned long displayInterval = 500; // ms, rychlost počítání
const unsigned long buttonInterval = 50;   // ms, rychlá odezva na tlačítka

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  lc.shutdown(0, false);
  lc.shutdown(1, false);
  lc.setScanLimit(0, 7);
  lc.setScanLimit(1, 7);
  lc.setIntensity(0, 8);
  lc.setIntensity(1, 8);
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  analogWrite(greenLedPin, 512);
  analogWrite(redLedPin, 512);

  Wire.begin(D2, D1); // SDA, SCL - PCF8574A

  // network.init();
  // network.reportUnansweredMessages();
}

void showTwoDigits(int chipAddr, int startPos, int number) {
  int tens = number / 10;
  int ones = number % 10;

  lc.setDigit(chipAddr, startPos, tens, false);
  lc.setDigit(chipAddr, startPos + 1, ones, false);
}

void showFourDigits(int chipAddr, int startPos, int number) {
  int thousands = number / 1000;
  int hundreds = (number / 100) % 10;
  int tens = (number / 10) % 10;
  int ones = number % 10;

  lc.setDigit(chipAddr, startPos, thousands, false);
  lc.setDigit(chipAddr, startPos + 1, hundreds, false);
  lc.setDigit(chipAddr, startPos + 2, tens, false);
  lc.setDigit(chipAddr, startPos + 3, ones, false);
}

void checkButtonsAndLeds() {
  Wire.beginTransmission(PCF8574A_ADDR);
  Wire.write((uint8_t)0b11100111); // P3, P4 na LOW = obě malé LED svítí
  Wire.endTransmission();

  Wire.requestFrom(PCF8574A_ADDR, 1);
  if (Wire.available()) {
    uint8_t state = Wire.read();

    bool greenPressed = !(state & 0x01);
    bool redPressed   = !(state & 0x02);
    bool keyOn        = !(state & 0x04);
    bool button1       = !(state & 0x20);
    bool button2       = !(state & 0x40);
    bool button3       = !(state & 0x80);

    if (greenPressed) Serial.println("Zelene tlacitko STISKNUTO");
    if (redPressed)   Serial.println("Cervene tlacitko STISKNUTO");
    if (keyOn)         Serial.println("Klic ZAPNUT");
    if (button1)        Serial.println("Tlacitko 1 STISKNUTO");
    if (button2)        Serial.println("Tlacitko 2 STISKNUTO");
    if (button3)        Serial.println("Tlacitko 3 STISKNUTO");
  } else {
    Serial.println("PCF8574A neodpovida - zkontroluj zapojeni/adresu");
  }
}

void loop() {
  unsigned long now = millis();

  if (now - lastButtonCheck >= buttonInterval) {
    lastButtonCheck = now;
    checkButtonsAndLeds();
  }

  if (now - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = now;

    showTwoDigits(0, 0, topNumber);
    showTwoDigits(0, 4, topNumber);

    showFourDigits(1, 0, bottomNumber);
    showFourDigits(1, 4, bottomNumber);

    topNumber++;
    if (topNumber > 99) topNumber = 1;

    bottomNumber++;
    if (bottomNumber > 9999) bottomNumber = 1;
  }
}