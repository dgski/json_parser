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

  utils::reusable_buffer pool(1000000);
  {
    auto sampleJson2 = json::parse(getSampleJson(), pool);
    auto& obj2 = std::get<json::Object>(sampleJson2);
    assert(std::get<json::String>(obj2.at("name")) == "John");
    assert(std::get<json::Int>(obj2.at("age")) == 30);
    auto& cars2 = std::get<json::Object>(obj2.at("cars"));
    assert(std::get<json::String>(cars2.at("car1")) == "Ford");
    assert(std::get<json::String>(cars2.at("car2")) == "BMW");
    assert(std::get<json::String>(cars2.at("car3")) == "Fiat");
    auto& children2 = std::get<json::Array>(obj2.at("children"));
    assert(std::get<json::String>(std::get<json::Object>(children2.at(0)).at("name")) == "Ann");
  }
  pool.clear();
  {
    auto sampleJson3 = json::parse(getSampleJson(), pool);
    auto& obj3 = std::get<json::Object>(sampleJson3);
    assert(std::get<json::String>(obj3.at("name")) == "John");
    assert(std::get<json::Int>(obj3.at("age")) == 30);
    auto& cars3 = std::get<json::Object>(obj3.at("cars"));
    assert(std::get<json::String>(cars3.at("car1")) == "Ford");
    assert(std::get<json::String>(cars3.at("car2")) == "BMW");
    assert(std::get<json::String>(cars3.at("car3")) == "Fiat");
    auto& children3 = std::get<json::Array>(obj3.at("children"));
    assert(std::get<json::String>(std::get<json::Object>(children3.at(0)).at("name")) == "Ann");
  }

  std::cout << "All tests passed!" << std::endl;

  std::string_view sample = R"({"name": "John", "age": 30, "favoriteNumbers": [1, 2, 3]})";
  auto parsed = json::parse(sample);

  return 0;
}