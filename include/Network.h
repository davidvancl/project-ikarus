#pragma once

#include "arduino_secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

class Network {
  public:
    bool init(const char* ssid, const char* password, unsigned long timeout = 15000);
    bool sendSlackMessage(const String& text);
    bool isConnected();

  private:
    String escapeJson(const String& text);
};