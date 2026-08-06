#pragma once

#include <algorithm>
#include <vector>
#include "Model.h"

class Scheduler {
 public:
  Scheduler(std::vector<Progress>& progress, std::vector<size_t>& queue)
      : progress_(progress), queue_(queue) {}

  bool empty() const { return queue_.empty(); }
  size_t remaining() const { return queue_.size(); }
  size_t current() const { return queue_.front(); }
  void done() {
    if (empty()) return;
    const size_t index = queue_.front();
    queue_.erase(queue_.begin());
    progress_[index].status = WordStatus::Known;
    progress_[index].okStreak = 3;
  }

  void ok() {
    if (empty()) return;
    const size_t index = queue_.front();
    queue_.erase(queue_.begin());
    Progress& item = progress_[index];
    if (++item.okStreak >= 3) {
      item.okStreak = 3;
      item.status = WordStatus::Known;
    } else {
      queue_.push_back(index);
    }
  }

  void ng() {
    if (empty()) return;
    const size_t index = queue_.front();
    queue_.erase(queue_.begin());
    progress_[index].okStreak = 0;
    const size_t position = std::min<size_t>(20, queue_.size());
    queue_.insert(queue_.begin() + position, index);
  }

 private:
  std::vector<Progress>& progress_;
  std::vector<size_t>& queue_;
};
