#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <unordered_map>
#include <charconv>
#include <iterator>

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
struct Object : std::unordered_map<String, Value> {
  using std::unordered_map<String, Value>::unordered_map;
};
struct Array : std::vector<Value> {
  using std::vector<Value>::vector;
};

struct ParseResult { Value value; std::string_view rest; };

ParseResult parseImpl(std::string_view textData);

ParseResult parseObject(std::string_view textData)
{
  textData.remove_prefix(1);
  Object object;
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.front() == '}') {
      return { std::move(object), textData.substr(1) };
    }
    textData.remove_prefix(object.empty() ? 0 : 1);
    auto [key, rest] = parseImpl(utils::consumeWhitespace(textData));
    auto [value, newRest] = parseImpl(utils::consumeWhitespace(rest).substr(1));
    object.insert({ std::get<String>(key), std::move(value) });
    textData = newRest;
  }
}

ParseResult parseArray(std::string_view textData)
{
  textData.remove_prefix(1);
  Array array;
  while (true) {
    textData = utils::consumeWhitespace(textData);
    if (textData.front() == ']') {
      return { std::move(array), textData.substr(1) };
    }
    textData.remove_prefix(array.empty() ? 0 : 1);
    auto [value, newRest] = parseImpl(textData);
    array.push_back(std::move(value));
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
    case 't': return parseTrue(textData);
    case 'f': return parseFalse(textData);
    case 'n': return parseNull(textData);
    default: return parseNumber(textData);
  }
}

Value parse(std::string_view textData) {
  return parseImpl(textData).value;
}

std::ostream& operator<<(std::ostream& os, const Object& object);
std::ostream& operator<<(std::ostream& os, const Array& array);
std::ostream& operator<<(std::ostream& os, const Value& value);

std::ostream& operator<<(std::ostream& os, const Object& object)
{
  if (object.empty()) {
    os << "{}";
    return os;
  }
  os << "{\"" << object.begin()->first << "\":" << object.begin()->second;
  for (auto it = std::next(object.begin()); it != object.end(); ++it) {
    os << ",\"" << it->first << "\":" << it->second;
  }
  os << "}";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Array& array)
{
  os << "[";
  std::copy(array.begin(), std::prev(array.end()), std::ostream_iterator<Value>(os, ","));
  os << array.back() << "]";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Value& value)
{
  struct Visitor {
    std::ostream& os;
    void operator()(const Int& value) const { os << value; }
    void operator()(const Float& value) const { os << value; }
    void operator()(const String& value) const { os << "\"" << value << "\""; }
    void operator()(const Bool& value) const { os << (value ? "true" : "false"); }
    void operator()(const Null& value) const { os << "null"; }
    void operator()(const Array& value) const { os << value; }
    void operator()(const Object& value) const { os << value; }
  };
  std::visit(Visitor{ os }, value);
  return os;
}

} // namespace json