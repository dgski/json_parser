# JSON Parser And Serializer

- Concise, optimistic JSON parser/serializer written in C++.
- Uses recursive parsing and std::variant (Prioritizing locality over space).
- No string allocations; uses references to original buffers.

Sample usage:

```c++
  std::string_view sample = R"({"name": "John", "age": 30, "favoriteNumbers": [1, 2, 3]})";
  auto parsed = json::parse(sample);
  for (const auto& [key, value] : std::get<json::Object>(parsed)) {
    std::cout << key << ": " << value << std::endl;
    if (std::holds_alternative<json::String>(value)) {
      std::cout << "It's a string!" << std::endl;
    } else if (std::holds_alternative<json::Int>(value)) {
      std::cout << "It's an int!" << std::endl;
    } else if (std::holds_alternative<json::Array>(value)) {
      auto& arr = std::get<json::Array>(value);
      std::cout << "It's an array with " << arr.size() << " elements" << std::endl;
    }
  }
```