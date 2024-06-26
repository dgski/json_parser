#pragma once

#include <string_view>
#include <memory_resource>
#include <optional>
#include <charconv>
#include <cctype>
#include <sstream>
#include <algorithm>

namespace utils {

std::string_view consumeWhitespace(std::string_view textData) {
  const auto begin = std::find_if_not(
    textData.begin(), textData.end(), [](char c) { return std::isspace(c); });
  return std::string_view(begin, textData.end());
}

class reusable_buffer : public std::pmr::memory_resource
{
  size_t _size = 0;
  std::unique_ptr<std::byte[]> _buffer;
  std::optional<std::pmr::monotonic_buffer_resource> _resource;
public:
  reusable_buffer(std::size_t size)
  : _size(size),
    _buffer(std::make_unique<std::byte[]>(size)),
    _resource(std::in_place, _buffer.get(), _size)
  {}
  void* do_allocate(std::size_t bytes, std::size_t alignment) override {
    return _resource->allocate(bytes, alignment);
  }
  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
    _resource->deallocate(p, bytes, alignment);
  }
  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
    return this == &other;
  }
  void clear() {
    _resource.reset();
    _resource.emplace(_buffer.get(), _size);
  }
};

int stoi(std::string_view str) {
  int result;
  std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

float stof(std::string_view str) {
  float result;
  std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

template<typename T>
std::string toString(const T& value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template<typename F>
auto benchmark(F&& f, size_t iterations)
{
  const auto t1 = std::chrono::high_resolution_clock::now();
  auto result = f();
  const auto iterationsLeft = iterations - 1;
  for (size_t i = 0; i < iterationsLeft; ++i) {
    f();
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::make_pair(result, std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}

} // namespace utils