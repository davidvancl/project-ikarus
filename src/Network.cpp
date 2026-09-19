#include "Network.h"
#include "NetworkUtils.h"
#include "TokenStore.h"

bool Network::init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  Serial.print("[Network]: Connecting to WiFi");
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT) {
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

bool Network::request(const String& url, const char* method, const String& payload, std::function<void(HTTPClient&)> onSuccess) {
  if (!isConnected()) {
    Serial.println("[Network]: Not connected to WiFi, request skipped.");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  bool success = false;

  if (https.begin(client, url)) {
    https.addHeader("Authorization", String("Bearer ") + TokenStore::slack());

    int httpCode;
    if (strcmp(method, "POST") == 0) {
      https.addHeader("Content-Type", "application/json; charset=utf-8");
      httpCode = https.POST(payload);
    } else {
      httpCode = https.GET();
    }

    if (httpCode == HTTP_CODE_OK) {
      if (onSuccess) onSuccess(https);
      success = true;
    } else if (httpCode > 0) {
      Serial.printf("[Network]: HTTP code: %d\n", httpCode);
      Serial.println("[Network]: Response: " + https.getString());
    } else {
      Serial.printf("[Network]: Request error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.println("[Network]: Failed to connect to Slack API");
  }

  return success;
}

bool Network::sendSlackMessage(const String& text) {
  JsonDocument document;
  document["channel"] = SLACK_AUTHOR_CHANNEL;
  document["text"] = text;

  String payload;
  serializeJson(document, payload);

  return request("https://slack.com/api/chat.postMessage", "POST", payload, [](HTTPClient& https) {
    Serial.println("[Network]: Response: " + https.getString());
  });
}

void Network::reportUnansweredMessages() {
  String links;

  String url = String("https://slack.com/api/conversations.history?channel=") + SLACK_DEV_OTAZKY_CHANNEL + "&limit=200";

  request(url, "GET", "", [&links](HTTPClient& https) {
    WiFiClient& stream = https.getStream();

    if (!NetworkUtils::skipToMarker(stream, "\"ok\":")) {
      Serial.println("[Network]: Unexpected response shape (no \"ok\" field).");
      return;
    }

    String okValue;
    int c;
    while ((c = NetworkUtils::readByte(stream)) >= 0 && (char)c != ',' && (char)c != '}') {
      okValue += (char)c;
    }

    if (okValue != "true") {
      String error;
      if (NetworkUtils::skipToMarker(stream, "\"error\":\"")) {
        error = NetworkUtils::readQuotedValue(stream);
      }
      Serial.println("[Network]: conversations.history returned ok=false, error: " + error);
      return;
    }

    if (!NetworkUtils::skipToMarker(stream, "\"messages\":[")) {
      Serial.println("[Network]: \"messages\" array not found in response.");
      return;
    }

    String linkPrefix = String("https://app.slack.com/client/") + SLACK_TEAM_ID + "/" + SLACK_DEV_OTAZKY_CHANNEL + "/";
    links = NetworkUtils::collectLinksWithoutMarker(stream, "\"type\":\"message\"", "\"name\":\"" SLACK_DEV_COMPLETED "\"", linkPrefix.c_str());
  });

  if (links.length() == 0) {
    Serial.println("[Network]: No unanswered messages, nothing to send.");
    return;
  }

  sendSlackMessage("Nezodpovezene otazky:\n" + links);
}