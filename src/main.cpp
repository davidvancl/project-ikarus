#include <Arduino.h>
#include <OtaUpdater.h>
#include "Display.h"
#include "Panel.h"
#include "TokenStore.h"
#include "WifiCredentials.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  Display::begin();
  Display::showDash();

  Serial.print("Firmware version: ");
  Serial.println(FW_VERSION);
  OtaUpdater::run(WIFI_CREDENTIALS);

#ifdef HAS_SECRETS
  TokenStore::save(SLACK_TOKEN, SECRET_GITLAB_TOKEN);
#endif
  TokenStore::load();

  Panel::begin();
  Display::clear();
}

void loop() {
  Panel::poll();
}
