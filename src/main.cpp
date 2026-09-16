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

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  lc.shutdown(0, false);
  lc.shutdown(1, false);
  lc.setScanLimit(0, 7); // plný rozsah 8 pozic na čipu 0
  lc.setScanLimit(1, 7); // plný rozsah 8 pozic na čipu 1
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

  lc.setDigit(chipAddr, startPos, tens, false); // "01" místo " 1"
  lc.setDigit(chipAddr, startPos + 1, ones, false);
}

void showFourDigits(int chipAddr, int startPos, int number) {
  int thousands = number / 1000;
  int hundreds = (number / 100) % 10;
  int tens = (number / 10) % 10;
  int ones = number % 10;

  lc.setDigit(chipAddr, startPos, thousands, false); // "0001" místo "   1"
  lc.setDigit(chipAddr, startPos + 1, hundreds, false);
  lc.setDigit(chipAddr, startPos + 2, tens, false);
  lc.setDigit(chipAddr, startPos + 3, ones, false);
}

void checkButtons() {
  Wire.beginTransmission(PCF8574A_ADDR);
  Wire.write((uint8_t)0xFF);
  Wire.endTransmission();

  Wire.requestFrom(PCF8574A_ADDR, 1);
  if (Wire.available()) {
    uint8_t state = Wire.read();
    bool greenPressed = !(state & 0x01); // P0
    bool redPressed = !(state & 0x02);   // P1
    bool keyOn = !(state & 0x04);        // P2

    if (greenPressed) Serial.println("Zelene tlacitko STISKNUTO");
    if (redPressed) Serial.println("Cervene tlacitko STISKNUTO");
    if (keyOn) Serial.println("Klic ZAPNUT");
  } else {
    Serial.println("PCF8574A neodpovida - zkontroluj zapojeni/adresu");
  }
}

void loop() {
  showTwoDigits(0, 0, topNumber);  // displej 1 (220801K)
  showTwoDigits(0, 4, topNumber);  // displej 2 (220801K)

  showFourDigits(1, 0, bottomNumber); // displej A (SH5461AS)
  showFourDigits(1, 4, bottomNumber); // displej B (SH5461AS)

  checkButtons();

  delay(500);

  topNumber++;
  if (topNumber > 99) topNumber = 1;

  bottomNumber++;
  if (bottomNumber > 9999) bottomNumber = 1;
}