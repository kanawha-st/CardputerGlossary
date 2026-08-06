#pragma once

#include <Arduino.h>
#include <vector>

namespace Csv {
std::vector<String> parseLine(const String& line);
String escape(const String& value);
}

