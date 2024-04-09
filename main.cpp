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

  std::cout << "All tests passed!" << std::endl;

  return 0;
}