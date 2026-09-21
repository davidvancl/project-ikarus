#pragma once

namespace Display {

void begin();
void showDash();
void clear();
void showTwoDigits(int chipAddr, int startPos, int number);
void showFourDigits(int chipAddr, int startPos, int number);

}
