# ebroschin-sdk
Reusable C++23 library modules providing application infrastructure: lifecycle management, async networking, job scheduling, persistence, logging, and a command system.
## Usage
Add the SDK as a git submodule and include the libraries you need:
```cmake
add_subdirectory(ebroschin-sdk/core)
add_subdirectory(ebroschin-sdk/network)
add_subdirectory(ebroschin-sdk/network-modules)
target_link_libraries(my_app PRIVATE
    ebroschin::core
    ebroschin::network
    ebroschin::network-modules
)
```
## Libraries
| Library | Purpose |
| --- | --- |
| `core` | Application lifecycle, system registration, and dependency context |
| `utility` | Signals, argument parsing, and variadic helpers |
| `network` | Transport-agnostic TCP and RPC abstractions |
| `network-modules` | Network backends and reference implementations |
| `scheduling` | Dedicated-thread task/job scheduler with one-shot and periodic scheduling |
| `persistence` | Storage-agnostic persistence abstractions |
| `persistence-modules` | Persistence backends and reference implementations |
| `logging` | Global logger with runtime-replaceable backends |
| `logging-modules` | Logging backends and reference implementations |
| `commands` | Compile-time command registry and dispatcher |
## Dependencies
`boost-asio`, `boost-stacktrace`, `nlohmann-json`, `sqlitecpp`, `spdlog`