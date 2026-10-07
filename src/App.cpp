#include "App.h"

#include <M5Cardputer.h>

void App::begin() {
  auto config = M5.config();
  M5Cardputer.begin(config, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextWrap(true);

  drawDisplayTest();

  if (!storage_.begin()) {
    drawError("SDカードを読めません");
    return;
  }
  String error;
  if (!storage_.loadWords(words_, error) ||
      !storage_.loadProgress(words_, progress_, queue_, error)) {
    drawError(error);
    return;
  }
  ready_ = true;
  draw();
}

void App::drawDisplayTest() {
  auto& display = M5Cardputer.Display;
  display.fillScreen(TFT_BLACK);

  // A compact splash layout designed for the 240x135 Cardputer display.
  display.drawRoundRect(5, 5, 230, 125, 8, TFT_DARKGREY);
  display.fillRoundRect(6, 6, 228, 4, 2, TFT_CYAN);

  // Simple open-book mark.
  display.drawRoundRect(17, 29, 36, 34, 4, TFT_CYAN);
  display.drawLine(35, 29, 35, 62, TFT_CYAN);
  display.drawLine(19, 34, 32, 34, TFT_DARKGREY);
  display.drawLine(38, 34, 51, 34, TFT_DARKGREY);

  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setFont(&fonts::Font4);
  display.setCursor(64, 24);
  display.print("ACADEMIC");
  display.setTextColor(TFT_CYAN, TFT_BLACK);
  display.setCursor(64, 51);
  display.print("GLOSSARY");

  display.setTextColor(TFT_YELLOW, TFT_BLACK);
  display.setFont(&fonts::lgfxJapanGothic_16);
  display.setCursor(64, 82);
  display.print("学術英単語");

  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setFont(&fonts::Font0);
  display.setCursor(64, 108);
  display.print("SD CARD / STARTING...");
  delay(2000);
}

void App::update() {
  M5Cardputer.update();
  if (!ready_ || !M5Cardputer.Keyboard.isChange() ||
      !M5Cardputer.Keyboard.isPressed()) return;
  const auto state = M5Cardputer.Keyboard.keysState();
  for (char key : state.word) handleKey(key);
}

void App::handleKey(char key) {
  if (scheduler_.empty()) return;
  if (key == ';') {
    englishVisible_ = true;
    draw();
    return;
  }
  if (key == ',') scheduler_.done();
  else if (key == '.') scheduler_.ng();
  else if (key == '/') scheduler_.ok();
  else return;
  englishVisible_ = false;
  save();
  draw();
}

void App::save() {
  String error;
  notice_ = storage_.saveProgress(words_, progress_, queue_, error) ? "" : error;
}

void App::drawError(const String& message) {
  auto& display = M5Cardputer.Display;
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_RED, TFT_BLACK);
  display.setFont(&fonts::lgfxJapanGothic_16);
  display.setCursor(8, 20);
  display.println("起動エラー");
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setCursor(8, 50);
  display.println(message);
}

void App::draw() {
  auto& display = M5Cardputer.Display;
  display.fillScreen(TFT_BLACK);
  if (scheduler_.empty()) {
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    display.setFont(&fonts::lgfxJapanGothic_16);
    display.setCursor(28, 45);
    display.println("すべて完了しました");
    return;
  }

  const size_t index = scheduler_.current();
  display.setTextColor(TFT_CYAN, TFT_BLACK);
  display.setFont(&fonts::Font2);
  display.setCursor(5, 3);
  display.printf("LEFT %u   OK %u/3", static_cast<unsigned>(scheduler_.remaining()),
                 progress_[index].okStreak);

  int battery = M5Cardputer.Power.getBatteryLevel();
  battery = constrain(battery, 0, 100);
  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setFont(&fonts::Font0);
  display.setCursor(210, 4);
  display.printf("%d%%", battery);

  if (englishVisible_) {
    display.setTextColor(TFT_YELLOW, TFT_BLACK);
    display.setFont(&fonts::Font4);
    display.setCursor(8, 25);
    display.println(words_[index].english);
  }

  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setFont(&fonts::lgfxJapanGothic_16);
  display.setCursor(8, englishVisible_ ? 59 : 35);
  display.println(words_[index].meaning);

  if (!notice_.isEmpty()) {
    display.setTextColor(TFT_RED, TFT_BLACK);
    display.setFont(&fonts::Font0);
    display.setCursor(5, 104);
    display.print(notice_);
  }
  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setFont(&fonts::lgfxJapanGothic_16);
  display.setCursor(2, 118);
  display.print("←DONE ↓NG ↑SHOW →OK");
}
