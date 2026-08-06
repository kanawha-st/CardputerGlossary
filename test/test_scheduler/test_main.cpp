#include <unity.h>
#include <vector>
#include "Scheduler.h"

static void test_ok_moves_to_end_and_third_ok_marks_known() {
  std::vector<Progress> progress(2);
  std::vector<size_t> queue{0, 1};
  Scheduler scheduler(progress, queue);

  scheduler.ok();
  TEST_ASSERT_EQUAL_UINT8(1, progress[0].okStreak);
  TEST_ASSERT_EQUAL_UINT32(1, queue[0]);
  TEST_ASSERT_EQUAL_UINT32(0, queue[1]);

  queue = {0, 1};
  progress[0].okStreak = 2;
  scheduler.ok();
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(WordStatus::Known),
                          static_cast<uint8_t>(progress[0].status));
  TEST_ASSERT_EQUAL_UINT8(3, progress[0].okStreak);
  TEST_ASSERT_EQUAL_UINT32(1, queue[0]);
}

static void test_ng_resets_and_inserts_twenty_places_back() {
  std::vector<Progress> progress(25);
  std::vector<size_t> queue;
  for (size_t i = 0; i < 25; ++i) queue.push_back(i);
  progress[0].okStreak = 2;
  Scheduler scheduler(progress, queue);

  scheduler.ng();
  TEST_ASSERT_EQUAL_UINT8(0, progress[0].okStreak);
  TEST_ASSERT_EQUAL_UINT32(0, queue[20]);
}

static void test_done_removes_current_word() {
  std::vector<Progress> progress(2);
  std::vector<size_t> queue{0, 1};
  Scheduler scheduler(progress, queue);
  scheduler.done();
  TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(WordStatus::Known),
                          static_cast<uint8_t>(progress[0].status));
  TEST_ASSERT_EQUAL_UINT32(1, queue[0]);
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_ok_moves_to_end_and_third_ok_marks_known);
  RUN_TEST(test_ng_resets_and_inserts_twenty_places_back);
  RUN_TEST(test_done_removes_current_word);
  UNITY_END();
}

void loop() {}
