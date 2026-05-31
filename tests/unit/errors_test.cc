#include <gtest/gtest.h>
#include "core/errors.h"

namespace minibus {
namespace {

TEST(ErrorCodeTest, KnownCodesReturnNonEmptyString) {
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::Ok), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::InvalidArgument), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::NotFound), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::AlreadyExists), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::PermissionDenied), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::Timeout), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::Unavailable), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::Internal), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::NotConnected), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::ConnectionRefused), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::BrokenPipe), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::InvalidMessage), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::NoRoute), "");
  EXPECT_STRNE(ErrorCodeToString(ErrorCode::Cancelled), "");
}

TEST(ErrorCodeTest, EachCodeHasUniqueString) {
  const char* a = ErrorCodeToString(ErrorCode::InvalidArgument);
  const char* b = ErrorCodeToString(ErrorCode::NotFound);
  EXPECT_STRNE(a, b);
}

TEST(ErrorCodeTest, OkReturnsZero) {
  EXPECT_EQ(static_cast<int32_t>(ErrorCode::Ok), 0);
}

TEST(ErrorCodeTest, UnknownCodeReturnsUnknown) {
  auto unknown = static_cast<ErrorCode>(999);
  EXPECT_STREQ(ErrorCodeToString(unknown), "Unknown");
}

}  // namespace
}  // namespace minibus
