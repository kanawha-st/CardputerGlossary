#pragma once

#include <Arduino.h>
#include <vector>
#include "Model.h"

class Storage {
 public:
  bool begin();
  bool loadWords(std::vector<Word>& words, String& error);
  bool loadProgress(const std::vector<Word>& words,
                    std::vector<Progress>& progress,
                    std::vector<size_t>& queue,
                    String& error);
  bool saveProgress(const std::vector<Word>& words,
                    const std::vector<Progress>& progress,
                    const std::vector<size_t>& queue,
                    String& error);

 private:
  static constexpr const char* kDirectory = "/academic_vocab";
  static constexpr const char* kWordsPath = "/academic_vocab/words.csv";
  static constexpr const char* kProgressPath = "/academic_vocab/progress.csv";
  static constexpr const char* kTempPath = "/academic_vocab/progress.tmp";
};
