#include "Network.h"

bool Network::init(const char* ssid, const char* password, unsigned long timeout) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("[Network]: Connecting to WiFi");
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > timeout) {
      Serial.println();
      Serial.println("[Network]: Connection failed (timeout).");
      return false;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("[Network]: Connected! IP: " + WiFi.localIP().toString());
  return true;
}

bool Network::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String Network::escapeJson(const String& text) {
  String out;
  out.reserve(text.length());
  for (size_t i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '"' || c == '\\') {
      out += '\\';
      out += c;
    } else if (c == '\n') {
      out += "\\n";
    } else {
      out += c;
    }
  }
  return out;
}

bool Network::sendSlackMessage(const String& text) {
  if (!isConnected()) {
    Serial.println("[Network]: Not connected to WiFi, message not sent.");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  bool success = false;

  if (https.begin(client, "https://slack.com/api/chat.postMessage")) {
    https.addHeader("Authorization", String("Bearer ") + SLACK_TOKEN);
    https.addHeader("Content-Type", "application/json; charset=utf-8");

    String payload = String("{\"channel\":\"") + SLACK_AUTHOR_CHANNEL + "\",\"text\":\"" + escapeJson(text) + "\"}";

    int httpCode = https.POST(payload);

    if (httpCode > 0) {
      String response = https.getString();
      Serial.printf("[Network]: HTTP code: %d\n", httpCode);
      Serial.println("[Network]: Response: " + response);
      success = (httpCode == HTTP_CODE_OK);
    } else {
      Serial.printf("[Network]: Request error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.println("[Network]: Failed to connect to Slack API");
  }

  return success;
}