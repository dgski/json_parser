#pragma once

#include <string_view>

namespace utils {

std::string_view consumeWhitespace(std::string_view textData) {
  while (!textData.empty() && std::isspace(textData.front())) {
    textData.remove_prefix(1);
  }
  return textData;
}

} // namespace utils