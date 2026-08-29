#include <Arduino.h>
#include <unity.h>
#include "NetworkUtils.h"

class FakeStream {
  public:
    explicit FakeStream(const char* data) : data(data), len(strlen(data)), pos(0) {}

    int available() { return pos < len ? 1 : 0; }
    int read() { return pos < len ? (unsigned char)data[pos++] : -1; }
    bool connected() { return pos < len; }

  private:
    const char* data;
    size_t len;
    size_t pos;
};

void test_skip_to_marker_found() {
  FakeStream stream("xxx\"messages\":[abc");
  TEST_ASSERT_TRUE(NetworkUtils::skipToMarker(stream, "\"messages\":["));
  TEST_ASSERT_EQUAL('a', stream.read());
}

void test_skip_to_marker_not_found() {
  FakeStream stream("no marker in here");
  TEST_ASSERT_FALSE(NetworkUtils::skipToMarker(stream, "\"messages\":["));
}

void test_read_quoted_value_handles_escapes() {
  FakeStream stream("ab\\\"cd\"ZZZ");
  String value = NetworkUtils::readQuotedValue(stream);
  TEST_ASSERT_EQUAL_STRING("ab\"cd", value.c_str());
  TEST_ASSERT_EQUAL('Z', stream.read());
}

void test_collect_links_message_with_reaction_is_answered() {
  FakeStream stream(
    "{\"user\":\"U1\",\"type\":\"message\",\"ts\":\"1000.001\",\"text\":\"ahoj\","
    "\"reactions\":[{\"name\":\"white_check_mark\",\"count\":1}]}]"
  );
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("", result.c_str());
}

void test_collect_links_message_without_reaction_is_listed() {
  FakeStream stream(
    "{\"user\":\"U1\",\"type\":\"message\",\"ts\":\"1000.002\",\"text\":\"otazka\"}]"
  );
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("https://slack/1000.002", result.c_str());
}

void test_collect_links_mixed_messages_only_lists_unanswered() {
  FakeStream stream(
    "{\"user\":\"U1\",\"type\":\"message\",\"ts\":\"1000.003\",\"text\":\"a\","
    "\"reactions\":[{\"name\":\"white_check_mark\"}]},"
    "{\"user\":\"U2\",\"type\":\"message\",\"ts\":\"1000.004\",\"text\":\"b\"}]"
  );
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("https://slack/1000.004", result.c_str());
}

void test_collect_links_ignores_nested_edited_ts() {
  FakeStream stream(
    "{\"user\":\"U1\",\"type\":\"message\",\"ts\":\"1000.005\",\"text\":\"puvodni\","
    "\"edited\":{\"user\":\"U1\",\"ts\":\"1000.999\"}}]"
  );
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("https://slack/1000.005", result.c_str());
}

void test_collect_links_survives_braces_inside_message_text() {
  FakeStream stream(
    "{\"user\":\"U1\",\"type\":\"message\",\"ts\":\"1000.006\","
    "\"text\":\"kod: if (x) { return {a:1}; }\"},"
    "{\"user\":\"U2\",\"type\":\"message\",\"ts\":\"1000.007\",\"text\":\"dalsi\","
    "\"reactions\":[{\"name\":\"white_check_mark\"}]}]"
  );
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("https://slack/1000.006", result.c_str());
}

void test_collect_links_empty_array_returns_empty_string() {
  FakeStream stream("]");
  String result = NetworkUtils::collectLinksWithoutMarker(
    stream, "\"type\":\"message\"", "\"name\":\"white_check_mark\"", "https://slack/");
  TEST_ASSERT_EQUAL_STRING("", result.c_str());
}

void setup() {
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_skip_to_marker_found);
  RUN_TEST(test_skip_to_marker_not_found);
  RUN_TEST(test_read_quoted_value_handles_escapes);
  RUN_TEST(test_collect_links_message_with_reaction_is_answered);
  RUN_TEST(test_collect_links_message_without_reaction_is_listed);
  RUN_TEST(test_collect_links_mixed_messages_only_lists_unanswered);
  RUN_TEST(test_collect_links_ignores_nested_edited_ts);
  RUN_TEST(test_collect_links_survives_braces_inside_message_text);
  RUN_TEST(test_collect_links_empty_array_returns_empty_string);
  UNITY_END();
}

void loop() {}
