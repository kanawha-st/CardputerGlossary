#pragma once

#include <Arduino.h>

enum class WordStatus : uint8_t { Learning, Known };

struct Word {
  uint32_t id = 0;
  String english;
  String meaning;
};

struct Progress {
  WordStatus status = WordStatus::Learning;
  uint8_t okStreak = 0;
};

