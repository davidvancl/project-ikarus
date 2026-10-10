#pragma once

namespace Display {

void begin();
void showDash();
void clear();
void setIntensity(int level);
void showPin(const char* pin);
void showTwoDigits(int chipAddr, int startPos, int number);
void showFourDigits(int chipAddr, int startPos, int number);

}
