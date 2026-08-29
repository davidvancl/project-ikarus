#pragma once

#include <Arduino.h>
#include <cstring>

namespace NetworkUtils {

template <typename Stream>
int readByte(Stream& stream) {
  while (true) {
    if (stream.available()) return stream.read();
    if (!stream.connected()) return -1;
  }
}

template <typename Stream>
bool skipToMarker(Stream& stream, const char* marker) {
  size_t matched = 0;
  size_t len = strlen(marker);

  while (matched < len) {
    int c = readByte(stream);
    if (c < 0) return false;
    char ch = (char)c;
    matched = (ch == marker[matched]) ? matched + 1 : (ch == marker[0] ? 1 : 0);
  }

  return true;
}

template <typename Stream>
String readQuotedValue(Stream& stream) {
  String value;
  bool escaped = false;

  int c;
  while ((c = readByte(stream)) >= 0) {
    char ch = (char)c;
    if (escaped) {
      value += ch;
      escaped = false;
      continue;
    }
    if (ch == '\\') {
      escaped = true;
      continue;
    }
    if (ch == '"') break;
    value += ch;
  }

  return value;
}

template <typename Stream>
String collectLinksWithoutMarker(Stream& stream, const char* objectMarker, const char* matchMarker, const char* linkPrefix) {
  size_t objectLen = strlen(objectMarker);
  size_t matchLen = strlen(matchMarker);
  size_t objectMatched = 0;
  size_t matchMatched = 0;

  bool sawFirstObject = false;
  bool currentHasMatch = false;
  String links;

  const char* tsMarker = "\"ts\":\"";
  size_t tsLen = strlen(tsMarker);
  size_t tsMatched = 0;
  bool tsCaptured = false;
  bool capturingTs = false;
  String currentTs;

  int depth = 1;
  bool inString = false;
  bool escaped = false;

  auto appendLinkIfMissing = [&]() {
    if (sawFirstObject && !currentHasMatch) {
      if (links.length() > 0) links += "\n";
      links += linkPrefix;
      links += currentTs;
    }
  };

  int c;
  while (depth > 0 && (c = readByte(stream)) >= 0) {
    char ch = (char)c;

    if (capturingTs) {
      if (ch == '"') {
        capturingTs = false;
        tsCaptured = true;
        inString = false;
      } else {
        currentTs += ch;
      }
      continue;
    }

    if (inString) {
      if (escaped) {
        escaped = false;
      } else if (ch == '\\') {
        escaped = true;
      } else if (ch == '"') {
        inString = false;
      }
    } else if (ch == '"') {
      inString = true;
    } else if (ch == '{' || ch == '[') {
      depth++;
    } else if (ch == '}' || ch == ']') {
      depth--;
      if (depth == 0) break;
    }

    if (ch == objectMarker[objectMatched]) {
      objectMatched++;
      if (objectMatched == objectLen) {
        appendLinkIfMissing();
        sawFirstObject = true;
        currentHasMatch = false;
        tsCaptured = false;
        currentTs = "";
        objectMatched = 0;
      }
    } else {
      objectMatched = (ch == objectMarker[0]) ? 1 : 0;
    }

    if (ch == matchMarker[matchMatched]) {
      matchMatched++;
      if (matchMatched == matchLen) {
        currentHasMatch = true;
        matchMatched = 0;
      }
    } else {
      matchMatched = (ch == matchMarker[0]) ? 1 : 0;
    }

    if (!tsCaptured) {
      if (ch == tsMarker[tsMatched]) {
        tsMatched++;
        if (tsMatched == tsLen) {
          capturingTs = true;
          tsMatched = 0;
        }
      } else {
        tsMatched = (ch == tsMarker[0]) ? 1 : 0;
      }
    }
  }

  appendLinkIfMissing();

  return links;
}

}
