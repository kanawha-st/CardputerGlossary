#include "Csv.h"

std::vector<String> Csv::parseLine(const String& input) {
  String line = input;
  if (line.endsWith("\r")) line.remove(line.length() - 1);

  std::vector<String> fields;
  String field;
  bool quoted = false;
  for (size_t i = 0; i < line.length(); ++i) {
    const char c = line[i];
    if (c == '"') {
      if (quoted && i + 1 < line.length() && line[i + 1] == '"') {
        field += '"';
        ++i;
      } else {
        quoted = !quoted;
      }
    } else if (c == ',' && !quoted) {
      fields.push_back(field);
      field = "";
    } else {
      field += c;
    }
  }
  fields.push_back(field);
  return fields;
}

String Csv::escape(const String& value) {
  if (value.indexOf(',') < 0 && value.indexOf('"') < 0 &&
      value.indexOf('\n') < 0 && value.indexOf('\r') < 0) {
    return value;
  }
  String escaped = value;
  escaped.replace("\"", "\"\"");
  String result = "\"";
  result += escaped;
  result += '"';
  return result;
}
