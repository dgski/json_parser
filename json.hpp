#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <unordered_map>
#include <charconv>
#include <memory_resource>

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
struct Object : std::pmr::unordered_map<String, Value> {
  using std::pmr::unordered_map<String, Value>::unordered_map;
};
struct Array : std::pmr::vector<Value> {
  using std::pmr::vector<Value>::vector;
};

struct ParseResult { Value value; std::string_view rest; };

ParseResult parseImpl(std::string_view textData, std::pmr::memory_resource& memoryResource);

ParseResult parseObject(std::string_view textData, std::pmr::memory_resource& memoryResource)
{
  textData.remove_prefix(1);
  Object object(&memoryResource);
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.front() == '}') {
      return { object, textData.substr(1) };
    }
    textData.remove_prefix(object.empty() ? 0 : 1);
    auto [key, rest] = parseImpl(utils::consumeWhitespace(textData), memoryResource);
    auto [value, newRest] = parseImpl(utils::consumeWhitespace(rest).substr(1), memoryResource);
    object.insert({ std::get<String>(key), value });
    textData = newRest;
  }
}

ParseResult parseArray(std::string_view textData, std::pmr::memory_resource& memoryResource)
{
  textData.remove_prefix(1);
  Array array(&memoryResource);
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.front() == ']') {
      return { array, textData.substr(1) };
    }
    textData.remove_prefix(array.empty() ? 0 : 1);
    auto [value, newRest] = parseImpl(textData, memoryResource);
    array.push_back(value);
    textData = newRest;
  }
}

ParseResult parseString(std::string_view textData)
{
  textData.remove_prefix(1);
  const auto end = textData.find('"');
  return { textData.substr(0, end), textData.substr(end + 1) };
}

ParseResult parseNumber(std::string_view textData)
{
  const auto it = std::find_if_not(textData.begin(), textData.end(), [](char c) {
    return std::isdigit(c) | c == '.' | c == '-';
  });
  const auto end = (it == textData.end()) ? textData.size() : std::distance(textData.begin(), it);
  const auto value = textData.substr(0, end);
  textData.remove_prefix(end);
  const bool isFloat = value.find('.') != std::string_view::npos;
  return { isFloat ? Value(utils::stof(value)) : Value(utils::stoi(value)), textData };
}

ParseResult parseTrue(std::string_view textData)
{
  return { true, textData.substr(4) };
}

ParseResult parseFalse(std::string_view textData)
{
  return { false, textData.substr(5) };
}

ParseResult parseNull(std::string_view textData)
{
  return { Null{}, textData.substr(4) };
}

ParseResult parseImpl(std::string_view textData, std::pmr::memory_resource& memoryResource)
{
  textData = utils::consumeWhitespace(textData);
  if (textData.empty()) {
    return { Null{}, textData };
  }

  switch (textData.front()) {
    case '{': return parseObject(textData, memoryResource);
    case '[': return parseArray(textData, memoryResource);
    case '"': return parseString(textData);
    case 't': return parseTrue(textData);
    case 'f': return parseFalse(textData);
    case 'n': return parseNull(textData);
    default: return parseNumber(textData);
  }
}

Value parse(std::string_view textData) {
  return parseImpl(textData, *std::pmr::get_default_resource()).value;
}

Value parse(std::string_view textData, std::pmr::memory_resource& memoryResource) {
  return parseImpl(textData, memoryResource).value;
}

std::ostream& operator<<(std::ostream& os, const Object& object);
std::ostream& operator<<(std::ostream& os, const Array& array);
std::ostream& operator<<(std::ostream& os, const Value& value);

std::ostream& operator<<(std::ostream& os, const Object& object)
{
  os << "{";
  for (auto it = object.begin(); it != object.end(); ++it) {
    os << "\"" << it->first << "\":" << it->second;
    if (std::next(it) != object.end()) {
      os << ",";
    }
  }
  os << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Array& array)
{
  os << "[";
  for (auto it = array.begin(); it != array.end(); ++it) {
    os << *it;
    if (std::next(it) != array.end()) {
      os << ",";
    }
  }
  os << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Value& value)
{
  if (std::holds_alternative<Int>(value)) {
    os << std::get<Int>(value);
  } else if (std::holds_alternative<Float>(value)) {
    os << std::get<Float>(value);
  } else if (std::holds_alternative<String>(value)) {
    os << "\"" << std::get<String>(value) << "\"";
  } else if (std::holds_alternative<Bool>(value)) {
    os << (std::get<Bool>(value) ? "true" : "false");
  } else if (std::holds_alternative<Null>(value)) {
    os << "null";
  } else if (std::holds_alternative<Array>(value)) {
    os << std::get<Array>(value);
  } else if (std::holds_alternative<Object>(value)) {
    os << std::get<Object>(value);
  }
  return os;
}

} // namespace json