#include <Arduino.h>
#include <EEPROM.h>
#include <OtaUpdater.h>
#include "Display.h"
#include "Panel.h"
#include "WifiCredentials.h"

static void migrateLegacyTokens() {
  struct {
    uint32_t magic;
    char slack[96];
    char gitlab[96];
  } legacy;
  EEPROM.begin(512);
  EEPROM.get(128, legacy);
  EEPROM.end();
  if (legacy.magic != 0x544F4B31) return;

  legacy.slack[sizeof(legacy.slack) - 1] = '\0';
  legacy.gitlab[sizeof(legacy.gitlab) - 1] = '\0';
  OtaUpdater::saveSecret("slack", legacy.slack);
  OtaUpdater::saveSecret("gitlab", legacy.gitlab);
  Serial.println("Legacy tokens migrated.");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  Display::begin();
  Display::showDash();

  Serial.print("Firmware version: ");
  Serial.println(FW_VERSION);
  OtaUpdater::run(WIFI_CREDENTIALS);

  migrateLegacyTokens();
#ifdef HAS_SECRETS
  OtaUpdater::saveSecret("slack", SLACK_TOKEN);
  OtaUpdater::saveSecret("gitlab", SECRET_GITLAB_TOKEN);
#endif

  Panel::begin();
  Display::clear();
}

void loop() {
  Panel::poll();
}
