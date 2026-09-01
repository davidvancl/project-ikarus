#include "Network.h"
#include "arduino_secrets.h"

#include <LedControl.h>

Network network;
LedControl lc = LedControl(13, 14, 2, 1); // D7, D5, D4 (GPIO2 místo GPIO15)

void setup() {
  Serial.begin(115200);
  Serial.println("Start"); // pridano pro kontrolu, ze ESP nabootovalo

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // network.init();
  // network.reportUnansweredMessages();
}

void showNumber(int number) {
  int tens = number / 10;
  int ones = number % 10;

  // první displej (DIG0/DIG1) - opraveno
  if (tens == 0) {
    lc.setChar(0, 0, ' ', false);
  } else {
    lc.setDigit(0, 0, tens, false);
  }
  lc.setDigit(0, 1, ones, false);

  // druhý displej (DIG4/DIG5) - beze změny
  if (tens == 0) {
    lc.setChar(0, 4, ' ', false);
  } else {
    lc.setDigit(0, 4, tens, false);
  }
  lc.setDigit(0, 5, ones, false);
}

void loop() {
  for (int number = 1; number <= 99; number++) {
    showNumber(number);
    delay(500);
  }
}