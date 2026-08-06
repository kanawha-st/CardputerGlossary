#include "Storage.h"

#include <SD.h>
#include <SPI.h>
#include <algorithm>
#include <unordered_map>
#include "Csv.h"

bool Storage::begin() {
  SPI.begin(40, 39, 14, 12);
  if (!SD.begin(12, SPI, 25000000)) return false;
  if (!SD.exists(kDirectory)) SD.mkdir(kDirectory);
  return true;
}

bool Storage::loadWords(std::vector<Word>& words, String& error) {
  File file = SD.open(kWordsPath, FILE_READ);
  if (!file) {
    error = "words.csv がありません";
    return false;
  }

  words.clear();
  bool first = true;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.isEmpty()) continue;
    const auto fields = Csv::parseLine(line);
    if (first && fields.size() >= 3 && fields[0].equalsIgnoreCase("id")) {
      first = false;
      continue;
    }
    first = false;
    if (fields.size() < 3) continue;
    char* end = nullptr;
    const uint32_t id = strtoul(fields[0].c_str(), &end, 10);
    if (end == fields[0].c_str() || *end != '\0') continue;
    words.push_back({id, fields[1], fields[2]});
  }
  file.close();
  if (words.empty()) {
    error = "有効な単語がありません";
    return false;
  }
  return true;
}

bool Storage::loadProgress(const std::vector<Word>& words,
                           std::vector<Progress>& progress,
                           std::vector<size_t>& queue,
                           String& error) {
  progress.assign(words.size(), Progress{});
  queue.clear();

  std::unordered_map<uint32_t, size_t> byId;
  for (size_t i = 0; i < words.size(); ++i) byId[words[i].id] = i;

  File file = SD.open(kProgressPath, FILE_READ);
  if (!file) {
    for (size_t i = 0; i < words.size(); ++i) queue.push_back(i);
    return true;
  }

  struct Ordered { int order; size_t index; };
  std::vector<Ordered> ordered;
  bool first = true;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.isEmpty()) continue;
    const auto fields = Csv::parseLine(line);
    if (first && fields.size() >= 4 && fields[0].equalsIgnoreCase("id")) {
      first = false;
      continue;
    }
    first = false;
    if (fields.size() < 4) continue;
    const uint32_t id = strtoul(fields[0].c_str(), nullptr, 10);
    const auto found = byId.find(id);
    if (found == byId.end()) continue;
    const size_t index = found->second;
    const bool known = fields[1].equalsIgnoreCase("KNOWN");
    progress[index].status = known ? WordStatus::Known : WordStatus::Learning;
    progress[index].okStreak = constrain(fields[2].toInt(), 0, 3);
    const int order = fields[3].toInt();
    if (!known && order >= 0) ordered.push_back({order, index});
  }
  file.close();

  std::sort(ordered.begin(), ordered.end(),
            [](const Ordered& a, const Ordered& b) { return a.order < b.order; });
  std::vector<bool> queued(words.size(), false);
  for (const auto& item : ordered) {
    if (!queued[item.index]) {
      queue.push_back(item.index);
      queued[item.index] = true;
    }
  }
  for (size_t i = 0; i < words.size(); ++i) {
    if (progress[i].status == WordStatus::Learning && !queued[i]) queue.push_back(i);
  }
  return true;
}

bool Storage::saveProgress(const std::vector<Word>& words,
                           const std::vector<Progress>& progress,
                           const std::vector<size_t>& queue,
                           String& error) {
  SD.remove(kTempPath);
  File file = SD.open(kTempPath, FILE_WRITE);
  if (!file) {
    error = "進捗を書き込めません";
    return false;
  }
  file.println("id,status,ok_streak,queue_order");
  std::vector<int> order(words.size(), -1);
  for (size_t i = 0; i < queue.size(); ++i) order[queue[i]] = static_cast<int>(i);
  for (size_t i = 0; i < words.size(); ++i) {
    file.printf("%lu,%s,%u,%d\n", static_cast<unsigned long>(words[i].id),
                progress[i].status == WordStatus::Known ? "KNOWN" : "LEARNING",
                progress[i].okStreak, order[i]);
  }
  file.flush();
  file.close();

  SD.remove(kProgressPath);
  if (!SD.rename(kTempPath, kProgressPath)) {
    error = "進捗を確定できません";
    return false;
  }
  return true;
}
