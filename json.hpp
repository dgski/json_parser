#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

#include "utils.hpp"

namespace json {

struct Object;
struct Array;
using Int = int;
using Float = float;
using String = std::string_view;
using Bool = bool;
using Null = std::monostate;
using Value = std::variant<Int, Float, String, Bool, Null, Array, Object>;
struct Object {
  std::vector<std::pair<String, Value>> values;
};
struct Array : std::vector<Value> {};

struct ParseResult { Value value; std::string_view rest; };

ParseResult parseImpl(std::string_view textData);

ParseResult parseObject(std::string_view textData)
{
  textData.remove_prefix(1);
  Object object;
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.empty()) {
      return { Null{}, textData };
    }
    if (textData.front() == '}') {
      textData.remove_prefix(1);
      return { object, textData };
    }
    if (!object.values.empty()) {
      if (textData.front() != ',') {
        return { Null{}, textData };
      }
      textData.remove_prefix(1);
    }
    textData = utils::consumeWhitespace(textData);
    if (textData.empty()) {
      return { Null{}, textData };
    }
    if (textData.front() != '"') {
      return { Null{}, textData };
    }
    auto [key, rest] = parseImpl(textData);
    auto keyStr = std::get<String>(key);
    if (keyStr.empty()) {
      return { Null{}, textData };
    }
    textData = rest;
    textData = utils::consumeWhitespace(textData);
    if (textData.empty() || textData.front() != ':') {
      return { Null{}, textData };
    }
    textData.remove_prefix(1);
    auto [value, newRest] = parseImpl(textData);
    if (std::holds_alternative<Null>(value)) {
      return { Null{}, textData };
    }
    object.values.push_back({ keyStr, value });
    textData = newRest;
  }
}

ParseResult parseArray(std::string_view textData)
{
  textData.remove_prefix(1);
  Array array;
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.empty()) {
      return { Null{}, textData };
    }
    if (textData.front() == ']') {
      textData.remove_prefix(1);
      return { array, textData };
    }
    if (!array.empty()) {
      if (textData.front() != ',') {
        return { Null{}, textData };
      }
      textData.remove_prefix(1);
    }
    auto [value, newRest] = parseImpl(textData);
    if (std::holds_alternative<Null>(value)) {
      return { Null{}, textData };
    }
    array.push_back(value);
    textData = newRest;
  }
}

ParseResult parseString(std::string_view textData)
{
  textData.remove_prefix(1);
  auto end = textData.find('"');
  if (end == std::string_view::npos) {
    return { Null{}, textData };
  }
  auto value = textData.substr(0, end);
  textData.remove_prefix(end + 1);
  return { value, textData };
}

ParseResult parseNumber(std::string_view textData)
{
  auto end = textData.find_first_not_of("0123456789.-");
  if (end == 0) {
    return { Null{}, textData };
  }
  auto value = textData.substr(0, end);
  textData.remove_prefix(end);
  if (value.find('.') != std::string_view::npos) {
    return { std::stof(std::string(value)), textData };
  } else {
    return { std::stoi(std::string(value)), textData };
  }
}

ParseResult parseImpl(std::string_view textData)
{
  textData = utils::consumeWhitespace(textData);
  if (textData.empty()) {
    return { Null{}, textData };
  }

  switch (textData.front()) {
    case '{': return parseObject(textData);
    case '[': return parseArray(textData);
    case '"': return parseString(textData);
    case 't': textData.remove_prefix(4); return { true, textData };
    case 'f': textData.remove_prefix(5); return { false, textData };
    case 'n': textData.remove_prefix(4); return { Null{}, textData };
    default: return parseNumber(textData);
  }
}

Value parse(std::string_view textData) {
  return parseImpl(textData).value;
}

} // namespace json