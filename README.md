# JSON Parser

- Concise, optimistic JSON parser written in C++.
- Uses recursive parsing and std::variant (Prioritizing locality over space).
- No string allocations; uses references to original buffer.
- Uses std::pmr::memory_resource for allocations: allowing for custom pre-allocation.