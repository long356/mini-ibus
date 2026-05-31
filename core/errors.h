// errors.h - unified error code type for the entire project
#pragma once

#include <cstdint>

namespace minibus {

enum class ErrorCode : int32_t {
  Ok = 0,
  InvalidArgument = -1,
  NotFound = -2,
  AlreadyExists = -3,
  PermissionDenied = -4,
  Timeout = -5,
  Unavailable = -6,
  Internal = -7,
  NotConnected = -8,
  ConnectionRefused = -9,
  BrokenPipe = -10,
  InvalidMessage = -11,
  NoRoute = -12,
  Cancelled = -13,
};

const char* ErrorCodeToString(ErrorCode code);

}  // namespace minibus
