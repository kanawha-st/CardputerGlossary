#pragma once

#include <vector>
#include "Model.h"
#include "Scheduler.h"
#include "Storage.h"

class App {
 public:
  void begin();
  void update();

 private:
  void draw();
  void drawDisplayTest();
  void drawError(const String& message);
  void handleKey(char key);
  void save();

  Storage storage_;
  std::vector<Word> words_;
  std::vector<Progress> progress_;
  std::vector<size_t> queue_;
  Scheduler scheduler_{progress_, queue_};
  bool ready_ = false;
  bool englishVisible_ = false;
  String notice_;
};
