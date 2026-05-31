// message_codec.h - LinkHeader 固定16字节帧头的编解码
#pragma once

#include <cstdint>

#include "core/errors.h"

namespace minibus {

enum class MessageType : uint8_t {
  REGISTER = 1,
  RPC_REQUEST = 2,
  RPC_RESPONSE = 3,
  PUBLISH = 4,
  SUBSCRIBE = 5,
  HEARTBEAT = 6,
  TOPOLOGY = 7,
  GOODBYE = 8,
};

#pragma pack(push, 1)
struct LinkHeader {
  uint8_t magic[2];       // "MI"
  uint8_t version;        // 协议版本
  uint8_t type;           // MessageType
  uint32_t src_addr;
  uint32_t dst_addr;
  uint32_t payload_size;  // 限制 64KB
};
#pragma pack(pop)

static_assert(sizeof(LinkHeader) == 16, "LinkHeader must be exactly 16 bytes");

constexpr uint8_t kProtocolVersion = 1;
constexpr uint32_t kMaxPayloadSize = 64 * 1024;

bool EncodeHeader(const LinkHeader& header, uint8_t* buf, size_t buf_size);
ErrorCode DecodeHeader(const uint8_t* buf, size_t buf_size, LinkHeader* out);

}  // namespace minibus
