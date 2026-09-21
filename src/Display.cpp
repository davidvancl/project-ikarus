#include "Display.h"

#include <LedControl.h>

namespace Display {

static const int CHIP_COUNT = 2;
static const int DIGIT_COUNT = 8;

static LedControl lc = LedControl(13, 14, 2, CHIP_COUNT);

void begin() {
  for (int chip = 0; chip < CHIP_COUNT; chip++) {
    lc.shutdown(chip, false);
    lc.setScanLimit(chip, DIGIT_COUNT - 1);
    lc.setIntensity(chip, 8);
    lc.clearDisplay(chip);
  }
}

void showDash() {
  for (int chip = 0; chip < CHIP_COUNT; chip++) {
    for (int digit = 0; digit < DIGIT_COUNT; digit++) {
      lc.setChar(chip, digit, '-', false);
    }
  }
}

void clear() {
  for (int chip = 0; chip < CHIP_COUNT; chip++) {
    lc.clearDisplay(chip);
  }
}

void showTwoDigits(int chipAddr, int startPos, int number) {
  lc.setDigit(chipAddr, startPos, number / 10, false);
  lc.setDigit(chipAddr, startPos + 1, number % 10, false);
}

void showFourDigits(int chipAddr, int startPos, int number) {
  lc.setDigit(chipAddr, startPos, number / 1000, false);
  lc.setDigit(chipAddr, startPos + 1, (number / 100) % 10, false);
  lc.setDigit(chipAddr, startPos + 2, (number / 10) % 10, false);
  lc.setDigit(chipAddr, startPos + 3, number % 10, false);
}

}
