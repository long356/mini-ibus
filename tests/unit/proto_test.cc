#include <gtest/gtest.h>
#include <proto/message.pb.h>
#include <proto/topology.pb.h>

namespace minibus {
namespace {

TEST(ProtoTest, RegisterPayloadRoundtrip) {
  proto::RegisterPayload original;
  original.set_my_address(0x00010001);
  original.add_services(1);
  original.add_services(2);
  original.add_topics(42);

  std::string serialized;
  ASSERT_TRUE(original.SerializeToString(&serialized));

  proto::RegisterPayload restored;
  ASSERT_TRUE(restored.ParseFromString(serialized));

  EXPECT_EQ(restored.my_address(), 0x00010001u);
  EXPECT_EQ(restored.services_size(), 2);
  EXPECT_EQ(restored.services(0), 1u);
  EXPECT_EQ(restored.services(1), 2u);
  EXPECT_EQ(restored.topics_size(), 1);
  EXPECT_EQ(restored.topics(0), 42u);
}

TEST(ProtoTest, RpcRequestRoundtrip) {
  proto::RpcRequest original;
  original.set_service_id(1);
  original.set_method_id(5);
  original.set_sequence(100);
  original.set_args("hello");

  std::string serialized;
  ASSERT_TRUE(original.SerializeToString(&serialized));

  proto::RpcRequest restored;
  ASSERT_TRUE(restored.ParseFromString(serialized));

  EXPECT_EQ(restored.service_id(), 1u);
  EXPECT_EQ(restored.method_id(), 5u);
  EXPECT_EQ(restored.sequence(), 100u);
  EXPECT_EQ(restored.args(), "hello");
}

TEST(ProtoTest, PublishPayloadRoundtrip) {
  proto::PublishPayload original;
  original.set_topic_id(42);
  original.set_data(std::string(100, 'x'));

  std::string serialized;
  ASSERT_TRUE(original.SerializeToString(&serialized));

  proto::PublishPayload restored;
  ASSERT_TRUE(restored.ParseFromString(serialized));

  EXPECT_EQ(restored.topic_id(), 42u);
  EXPECT_EQ(restored.data().size(), 100u);
}

TEST(ProtoTest, HeartbeatPayloadRoundtrip) {
  proto::HeartbeatPayload original;
  original.set_timestamp_us(1234567890123);
  original.set_seq(42);

  std::string serialized;
  ASSERT_TRUE(original.SerializeToString(&serialized));

  proto::HeartbeatPayload restored;
  ASSERT_TRUE(restored.ParseFromString(serialized));

  EXPECT_EQ(restored.timestamp_us(), 1234567890123u);
  EXPECT_EQ(restored.seq(), 42u);
}

TEST(ProtoTest, TopologyMessageRoundtrip) {
  proto::FullTopologyBroadcast original;
  original.mutable_topology()->set_publisher_role(1);
  original.mutable_topology()->set_version(5);
  original.mutable_topology()->set_timestamp_ms(1000);

  auto* link = original.mutable_topology()->add_links();
  link->set_peer_role(2);
  link->set_medium(0);
  link->set_weight(1);
  link->set_last_active_ms(900);

  auto* node = original.add_nodes();
  node->set_node_address(0x00010001);
  node->add_services(1);

  std::string serialized;
  ASSERT_TRUE(original.SerializeToString(&serialized));

  proto::FullTopologyBroadcast restored;
  ASSERT_TRUE(restored.ParseFromString(serialized));

  EXPECT_EQ(restored.topology().publisher_role(), 1u);
  EXPECT_EQ(restored.topology().version(), 5u);
  EXPECT_EQ(restored.topology().links_size(), 1);
  EXPECT_EQ(restored.nodes_size(), 1);
}

}  // namespace
}  // namespace minibus
