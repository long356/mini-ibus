#include "core/message_codec.h"

#include <cstring>

namespace minibus {

bool EncodeHeader(const LinkHeader& header, uint8_t* buf, size_t buf_size) {
  if (buf_size < sizeof(LinkHeader)) {
    return false;
  }
  if (header.payload_size > kMaxPayloadSize) {
    return false;
  }
  std::memcpy(buf, &header, sizeof(LinkHeader));
  return true;
}

ErrorCode DecodeHeader(const uint8_t* buf, size_t buf_size, LinkHeader* out) {
  if (buf_size < sizeof(LinkHeader)) {
    return ErrorCode::InvalidArgument;
  }
  if (!out) {
    return ErrorCode::InvalidArgument;
  }

  std::memcpy(out, buf, sizeof(LinkHeader));

  if (out->magic[0] != 'M' || out->magic[1] != 'I') {
    return ErrorCode::InvalidMessage;
  }
  if (out->version != kProtocolVersion) {
    return ErrorCode::InvalidMessage;
  }
  if (out->payload_size > kMaxPayloadSize) {
    return ErrorCode::InvalidMessage;
  }

  return ErrorCode::Ok;
}

}  // namespace minibus
