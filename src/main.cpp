#include "Network.h"
#include "arduino_secrets.h"

Network network;

void setup() {
  Serial.begin(115200);

  if (network.init(SECRET_SSID, SECRET_PASS)) {
    network.sendSlackMessage("Test zpravy z bota (ESP8266, Network class)");
  }
}

void loop() {
  
}