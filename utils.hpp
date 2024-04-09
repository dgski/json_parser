#pragma once

#include <string_view>
#include <memory_resource>
#include <optional>

namespace utils {

std::string_view consumeWhitespace(std::string_view textData) {
  while (!textData.empty() && std::isspace(textData.front())) {
    textData.remove_prefix(1);
  }
  return textData;
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

} // namespace utils