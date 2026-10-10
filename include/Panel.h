#pragma once

namespace Panel {

void begin();
void poll();
void setGreen(int percent);
void setRed(int percent);
int green();
int red();
bool keyOn();

}
