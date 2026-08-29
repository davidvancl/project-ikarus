#pragma once

#include "config.h"
#include "arduino_secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <functional>
#include <ArduinoJson.h>

class Network {
  public:
    bool init();
    bool sendSlackMessage(const String& text);
    void reportUnansweredMessages();
    bool isConnected();

  private:
    bool request(const String& url, const char* method, const String& payload, std::function<void(HTTPClient&)> onSuccess);
};
