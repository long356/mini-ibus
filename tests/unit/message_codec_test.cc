#include <gtest/gtest.h>

#include <cstring>

#include "core/message_codec.h"

namespace minibus {
namespace {

TEST(MessageCodecTest, EncodeDecodeRoundtrip) {
  LinkHeader original{};
  original.magic[0] = 'M';
  original.magic[1] = 'I';
  original.version = kProtocolVersion;
  original.type = static_cast<uint8_t>(MessageType::RPC_REQUEST);
  original.src_addr = 0x00010001;
  original.dst_addr = 0x00020003;
  original.payload_size = 128;

  uint8_t buf[16];
  ASSERT_TRUE(EncodeHeader(original, buf, sizeof(buf)));

  LinkHeader decoded{};
  EXPECT_EQ(DecodeHeader(buf, sizeof(buf), &decoded), ErrorCode::Ok);
  EXPECT_EQ(decoded.magic[0], 'M');
  EXPECT_EQ(decoded.magic[1], 'I');
  EXPECT_EQ(decoded.version, kProtocolVersion);
  EXPECT_EQ(decoded.type, static_cast<uint8_t>(MessageType::RPC_REQUEST));
  EXPECT_EQ(decoded.src_addr, 0x00010001u);
  EXPECT_EQ(decoded.dst_addr, 0x00020003u);
  EXPECT_EQ(decoded.payload_size, 128u);
}

TEST(MessageCodecTest, HeaderSizeIsExactly16) {
  EXPECT_EQ(sizeof(LinkHeader), 16u);
}

TEST(MessageCodecTest, DecodeRejectsBadMagic) {
  LinkHeader original{};
  original.magic[0] = 'X';
  original.magic[1] = 'Y';
  original.version = kProtocolVersion;
  original.payload_size = 0;

  uint8_t buf[16];
  ASSERT_TRUE(EncodeHeader(original, buf, sizeof(buf)));

  LinkHeader decoded{};
  EXPECT_EQ(DecodeHeader(buf, sizeof(buf), &decoded), ErrorCode::InvalidMessage);
}

TEST(MessageCodecTest, DecodeRejectsWrongVersion) {
  LinkHeader original{};
  original.magic[0] = 'M';
  original.magic[1] = 'I';
  original.version = 99;
  original.payload_size = 0;

  uint8_t buf[16];
  ASSERT_TRUE(EncodeHeader(original, buf, sizeof(buf)));

  LinkHeader decoded{};
  EXPECT_EQ(DecodeHeader(buf, sizeof(buf), &decoded), ErrorCode::InvalidMessage);
}

TEST(MessageCodecTest, DecodeRejectsOversizedPayload) {
  LinkHeader original{};
  original.magic[0] = 'M';
  original.magic[1] = 'I';
  original.version = kProtocolVersion;
  original.payload_size = kMaxPayloadSize + 1;

  uint8_t buf[16];
  EXPECT_FALSE(EncodeHeader(original, buf, sizeof(buf)));

  // Also test decode side: manually craft oversized payload in buffer
  original.payload_size = kMaxPayloadSize + 1;
  std::memcpy(buf, &original, sizeof(LinkHeader));

  LinkHeader decoded{};
  EXPECT_EQ(DecodeHeader(buf, sizeof(buf), &decoded), ErrorCode::InvalidMessage);
}

TEST(MessageCodecTest, EncodeRejectsSmallBuffer) {
  LinkHeader hdr{};
  hdr.magic[0] = 'M';
  hdr.magic[1] = 'I';
  hdr.version = kProtocolVersion;
  hdr.payload_size = 0;

  uint8_t small_buf[8];
  EXPECT_FALSE(EncodeHeader(hdr, small_buf, sizeof(small_buf)));
}

TEST(MessageCodecTest, DecodeRejectsSmallBuffer) {
  uint8_t small_buf[8] = {};
  LinkHeader decoded{};
  EXPECT_EQ(DecodeHeader(small_buf, sizeof(small_buf), &decoded),
            ErrorCode::InvalidArgument);
}

TEST(MessageCodecTest, DecodeRejectsNullOutput) {
  uint8_t buf[16] = {};
  EXPECT_EQ(DecodeHeader(buf, sizeof(buf), nullptr),
            ErrorCode::InvalidArgument);
}

TEST(MessageCodecTest, AllMessageTypesEncode) {
  LinkHeader hdr{};
  hdr.magic[0] = 'M';
  hdr.magic[1] = 'I';
  hdr.version = kProtocolVersion;
  hdr.src_addr = 0;
  hdr.dst_addr = 0;
  hdr.payload_size = 0;

  MessageType types[] = {
      MessageType::REGISTER,   MessageType::RPC_REQUEST,
      MessageType::RPC_RESPONSE, MessageType::PUBLISH,
      MessageType::SUBSCRIBE,  MessageType::HEARTBEAT,
      MessageType::TOPOLOGY,   MessageType::GOODBYE,
  };

  for (auto mt : types) {
    hdr.type = static_cast<uint8_t>(mt);
    uint8_t buf[16];
    ASSERT_TRUE(EncodeHeader(hdr, buf, sizeof(buf)));

    LinkHeader decoded{};
    ASSERT_EQ(DecodeHeader(buf, sizeof(buf), &decoded), ErrorCode::Ok);
    EXPECT_EQ(decoded.type, static_cast<uint8_t>(mt));
  }
}

}  // namespace
}  // namespace minibus
