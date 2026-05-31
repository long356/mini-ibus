#include "logging.h"

#ifdef MINIBUS_HAS_SPDLOG
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#endif

namespace minibus {

void InitLogging(const std::string& app_name, LogLevel level) {
#ifdef MINIBUS_HAS_SPDLOG
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>(app_name, console_sink);

  switch (level) {
    case LogLevel::Debug:
      logger->set_level(spdlog::level::debug);
      break;
    case LogLevel::Info:
      logger->set_level(spdlog::level::info);
      break;
    case LogLevel::Warn:
      logger->set_level(spdlog::level::warn);
      break;
    case LogLevel::Error:
      logger->set_level(spdlog::level::err);
      break;
  }

  spdlog::set_default_logger(std::move(logger));
#else
  // Fallback: logging macros already write to stderr; nothing to initialize.
  // Suppress unused warnings.
  (void)app_name;
  (void)level;
#endif
}

}  // namespace minibus
