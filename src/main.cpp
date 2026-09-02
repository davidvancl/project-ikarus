#include "Network.h"
#include "arduino_secrets.h"

#include <LedControl.h>

Network network;
LedControl lc = LedControl(13, 14, 2, 2); // D7, D5, D4, počet čipů = 2

int topNumber = 1;      // čip 0: displeje 220801K, 1-99
int bottomNumber = 0;   // čip 1: displeje SH5461AS A i B, 0-9999

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  lc.shutdown(0, false);
  lc.shutdown(1, false);
  lc.setIntensity(0, 8);
  lc.setIntensity(1, 8);
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  // network.init();
  // network.reportUnansweredMessages();
}

void showNumberTop(int number) {
  int tens = number / 10;
  int ones = number % 10;

  if (tens == 0) {
    lc.setChar(0, 0, ' ', false);
  } else {
    lc.setDigit(0, 0, tens, false);
  }
  lc.setDigit(0, 1, ones, false);

  if (tens == 0) {
    lc.setChar(0, 4, ' ', false);
  } else {
    lc.setDigit(0, 4, tens, false);
  }
  lc.setDigit(0, 5, ones, false);
}

void showFourDigits(int chipAddr, int startPos, int number) {
  int thousands = number / 1000;
  int hundreds = (number / 100) % 10;
  int tens = (number / 10) % 10;
  int ones = number % 10;

  bool leadingZero = true;

  if (thousands == 0 && leadingZero) {
    lc.setChar(chipAddr, startPos, ' ', false);
  } else {
    lc.setDigit(chipAddr, startPos, thousands, false);
    leadingZero = false;
  }

  if (hundreds == 0 && leadingZero) {
    lc.setChar(chipAddr, startPos + 1, ' ', false);
  } else {
    lc.setDigit(chipAddr, startPos + 1, hundreds, false);
    leadingZero = false;
  }

  if (tens == 0 && leadingZero) {
    lc.setChar(chipAddr, startPos + 2, ' ', false);
  } else {
    lc.setDigit(chipAddr, startPos + 2, tens, false);
    leadingZero = false;
  }

  lc.setDigit(chipAddr, startPos + 3, ones, false);
}

void loop() {
  showNumberTop(topNumber);
  showFourDigits(1, 0, bottomNumber); // displej A: pozice 0-3
  showFourDigits(1, 4, bottomNumber); // displej B: pozice 4-7
  delay(500);

  topNumber++;
  if (topNumber > 99) topNumber = 1;

  bottomNumber++;
  if (bottomNumber > 9999) bottomNumber = 0;
}