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
  std::cout << "All tests passed!\n";

  return 0;
}