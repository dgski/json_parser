#include <iostream>
#include <cassert>

#include "json.hpp"

auto getSampleJson() {
  return R"(
    {
      "name": "John",
      "age": 30,
      "cars": {
        "car1": "Ford",
        "car2": "BMW",
        "car3": "Fiat"
      },
      "children": [
        {
          "name": "Ann",
          "age": 5
        },
        {
          "name": "Billy",
          "age": 7
        }
      ]
    }
  )";
}

auto getHugeJsonArray() {
  std::string result = "[";
  for (int i = 0; i < 10000; i++) {
    result += getSampleJson();
    result += ",";
  }
  result.pop_back();
  result += "]";
  return result;
}

int main() {
  assert(std::holds_alternative<json::Null>(json::parse("null")));
  assert(std::holds_alternative<json::String>(json::parse("\"hello\"")));
  assert(std::holds_alternative<json::Bool>(json::parse("true")));
  assert(std::holds_alternative<json::Int>(json::parse("42")));
  assert(std::holds_alternative<json::Float>(json::parse("3.14")));
  assert(std::holds_alternative<json::Array>(json::parse("[1, 2, 3]")));
  assert(std::holds_alternative<json::Object>(json::parse(getSampleJson())));
  
  auto sampleJson = json::parse(getSampleJson());
  auto& obj = std::get<json::Object>(sampleJson);
  assert(std::get<json::String>(obj.at("name")) == "John");
  assert(std::get<json::Int>(obj.at("age")) == 30);
  auto& cars = std::get<json::Object>(obj.at("cars"));
  assert(std::get<json::String>(cars.at("car1")) == "Ford");
  assert(std::get<json::String>(cars.at("car2")) == "BMW");
  assert(std::get<json::String>(cars.at("car3")) == "Fiat");
  auto& children = std::get<json::Array>(obj.at("children"));
  assert(std::get<json::String>(std::get<json::Object>(children.at(0)).at("name")) == "Ann");
  assert(std::get<json::Int>(std::get<json::Object>(children.at(0)).at("age")) == 5);
  assert(std::get<json::String>(std::get<json::Object>(children.at(1)).at("name")) == "Billy");
  assert(std::get<json::Int>(std::get<json::Object>(children.at(1)).at("age")) == 7);
  std::cout << "All tests parsing passed!" << std::endl;

  json::Value value = 42;
  assert(utils::toString(value) == "42");
  value = 3.14f;
  assert(utils::toString(value) == "3.14");
  value = "hello";
  assert(utils::toString(value) == "\"hello\"");
  value = true;
  assert(utils::toString(value) == "true");
  value = false;
  assert(utils::toString(value) == "false");
  value = json::Null{};
  assert(utils::toString(value) == "null");
  value = json::Array{1, 2, 3};
  assert(utils::toString(value) == "[1,2,3]");
  value = json::Object{{"key1", 1}, {"key2", 2}};
  assert(
    (utils::toString(value) == "{\"key1\":1,\"key2\":2}") ||
    (utils::toString(value) == "{\"key2\":2,\"key1\":1}"));
  std::cout << "All serialization tests passed!" << std::endl;

  const std::string_view finalBusinessJson = R"(
    {
      "items": [
        [1, "item1", 10],
        [2, "item2", 20]
      ]
    }
  )";
  auto businessJson = json::parse(finalBusinessJson);
  const std::string_view EXPECTED_STRING = "{\"items\":[[1,\"item1\",10],[2,\"item2\",20]]}";
  assert(utils::toString(businessJson) == EXPECTED_STRING);

  std::cout << "All tests passed!" << std::endl;
  
  const auto ITERATIONS = 10;
  auto jsonString = getHugeJsonArray();
  auto [result, timeTaken] = utils::benchmark([&jsonString]() {
    auto result = json::parse(jsonString);
    return result;
  }, ITERATIONS);
  std::cout << "Time taken to parse 1 JSON string: " << (timeTaken / ITERATIONS) << "ns result=" << utils::toString(result).size() << std::endl;

  return 0;
}