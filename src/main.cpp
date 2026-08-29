#include "Network.h"
#include "arduino_secrets.h"

Network network;

void setup() {
  Serial.begin(115200);

  network.init();
  network.reportUnansweredMessages();
}

void loop() {
  
}