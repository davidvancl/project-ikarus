#pragma once

namespace Display {

void begin();
void showDash();
void clear();
void setIntensity(int level);
int intensity();
void showPin(const char* pin);
void blank(int chipAddr, int startPos, int count);
void showTwoDigits(int chipAddr, int startPos, int number);
void showFourDigits(int chipAddr, int startPos, int number);

}
