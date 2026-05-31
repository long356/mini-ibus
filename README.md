# mini-ibus

A lightweight distributed communication framework preserving IBus core design.

## Features

- NodeAddress = (Role, Name) dual-layer addressing
- Pub/Sub + RPC dual communication model
- MsgDispatcher multi-lane priority isolation
- One ibusd per device + shared-memory route table
- Multi-device topology merging via gossip + Dijkstra
- Heartbeat + automatic fault re-routing

## Prerequisites

- C++17 (GCC 9+ or Clang 10+)
- CMake 3.16+
- Protobuf 3.x
- spdlog (logging)
- GoogleTest (unit testing)
- nlohmann/json (configuration)

## Quick Start

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure
```

## Project Structure

```
mini-ibus/
├── core/          # Core library (linked by business processes)
├── ibusd/         # Daemon process
├── proto/         # Protobuf definitions
├── tools/         # CLI tools
├── examples/      # Demo applications
├── tests/         # Unit and integration tests
├── addons/        # Optional business-side enhancements
└── docs/          # Documentation
```
