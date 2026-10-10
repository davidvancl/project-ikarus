#include <Arduino.h>
#include <DeviceControl.h>
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

static DeviceControl::Setter number(int min, int max, std::function<void(int)> apply) {
  return [min, max, apply](JsonVariantConst value) {
    if (!value.is<int>()) return false;
    int n = value.as<int>();
    if (n < min || n > max) return false;
    apply(n);
    return true;
  };
}

// A display accepts a number in 0..max, or null to blank it.
static DeviceControl::Setter display(int max, std::function<void(int)> show, std::function<void()> blank) {
  return [max, show, blank](JsonVariantConst value) {
    if (value.isNull()) {
      blank();
      return true;
    }
    if (!value.is<int>()) return false;
    int n = value.as<int>();
    if (n < 0 || n > max) return false;
    show(n);
    return true;
  };
}

static void setupControl() {
  DeviceControl::begin("Ikarus", "ikarus");

  DeviceControl::set("left2", display(99, [](int v) { Display::showTwoDigits(0, 0, v); }, [] { Display::blank(0, 0, 2); }));
  DeviceControl::set("right2", display(99, [](int v) { Display::showTwoDigits(0, 4, v); }, [] { Display::blank(0, 4, 2); }));
  DeviceControl::set("left4", display(9999, [](int v) { Display::showFourDigits(1, 0, v); }, [] { Display::blank(1, 0, 4); }));
  DeviceControl::set("right4", display(9999, [](int v) { Display::showFourDigits(1, 4, v); }, [] { Display::blank(1, 4, 4); }));
  DeviceControl::set("intensity", number(0, 15, Display::setIntensity));
  DeviceControl::set("green", number(0, 100, Panel::setGreen));
  DeviceControl::set("red", number(0, 100, Panel::setRed));
  DeviceControl::get<int>("intensity", Display::intensity);
  DeviceControl::get<int>("green", Panel::green);
  DeviceControl::get<int>("red", Panel::red);
  DeviceControl::get<bool>("key", Panel::keyOn);

  DeviceControl::pairWithPin(Display::showPin, Display::clear);
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
  setupControl();
}

void loop() {
  Panel::poll();
  DeviceControl::loop();
}
