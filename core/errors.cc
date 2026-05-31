#include "errors.h"

namespace minibus {

const char* ErrorCodeToString(ErrorCode code) {
  switch (code) {
    case ErrorCode::Ok:
      return "Ok";
    case ErrorCode::InvalidArgument:
      return "InvalidArgument";
    case ErrorCode::NotFound:
      return "NotFound";
    case ErrorCode::AlreadyExists:
      return "AlreadyExists";
    case ErrorCode::PermissionDenied:
      return "PermissionDenied";
    case ErrorCode::Timeout:
      return "Timeout";
    case ErrorCode::Unavailable:
      return "Unavailable";
    case ErrorCode::Internal:
      return "Internal";
    case ErrorCode::NotConnected:
      return "NotConnected";
    case ErrorCode::ConnectionRefused:
      return "ConnectionRefused";
    case ErrorCode::BrokenPipe:
      return "BrokenPipe";
    case ErrorCode::InvalidMessage:
      return "InvalidMessage";
    case ErrorCode::NoRoute:
      return "NoRoute";
    case ErrorCode::Cancelled:
      return "Cancelled";
    default:
      return "Unknown";
  }
}

}  // namespace minibus
