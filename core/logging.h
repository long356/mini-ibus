// logging.h - unified logging macros wrapping spdlog (with printf fallback)
#pragma once

#include <string>

namespace minibus {

enum class LogLevel {
  Debug = 0,
  Info = 1,
  Warn = 2,
  Error = 3,
};

void InitLogging(const std::string& app_name, LogLevel level);

}  // namespace minibus

#ifdef MINIBUS_HAS_SPDLOG

#include <spdlog/spdlog.h>

#define MINIBUS_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define MINIBUS_INFO(...)  spdlog::info(__VA_ARGS__)
#define MINIBUS_WARN(...)  spdlog::warn(__VA_ARGS__)
#define MINIBUS_ERROR(...) spdlog::error(__VA_ARGS__)

#else  // printf fallback when spdlog not available

#include <cstdio>

#define MINIBUS_DEBUG(fmt, ...) std::fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define MINIBUS_INFO(fmt, ...)  std::fprintf(stderr, "[INFO]  " fmt "\n", ##__VA_ARGS__)
#define MINIBUS_WARN(fmt, ...)  std::fprintf(stderr, "[WARN]  " fmt "\n", ##__VA_ARGS__)
#define MINIBUS_ERROR(fmt, ...) std::fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#endif
