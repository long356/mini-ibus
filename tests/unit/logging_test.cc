#include <gtest/gtest.h>
#include <sstream>
#include "core/logging.h"

namespace minibus {
namespace {

TEST(LoggingTest, InitLoggingDoesNotCrash) {
  InitLogging("test_app", LogLevel::Info);
  SUCCEED();
}

TEST(LoggingTest, InitLoggingMultipleCalls) {
  InitLogging("first", LogLevel::Debug);
  InitLogging("second", LogLevel::Error);
  InitLogging("third", LogLevel::Warn);
  SUCCEED();
}

TEST(LoggingTest, MacrosCompile) {
  MINIBUS_DEBUG("debug message: {}", 42);
  MINIBUS_INFO("info message: {}", 42);
  MINIBUS_WARN("warn message: {}", 42);
  MINIBUS_ERROR("error message: {}", 42);
  SUCCEED();
}

TEST(LoggingTest, AllLogLevelsAreDistinct) {
  EXPECT_NE(static_cast<int>(LogLevel::Debug), static_cast<int>(LogLevel::Info));
  EXPECT_NE(static_cast<int>(LogLevel::Info), static_cast<int>(LogLevel::Warn));
  EXPECT_NE(static_cast<int>(LogLevel::Warn), static_cast<int>(LogLevel::Error));
}

}  // namespace
}  // namespace minibus
