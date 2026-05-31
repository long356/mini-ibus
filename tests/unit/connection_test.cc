#include <gtest/gtest.h>

#include <cstdlib>
#include <cstring>
#include <thread>

#include "core/connection.h"

namespace minibus {
namespace {

#ifndef _WIN32

#include <unistd.h>

class UdsConnectionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    path_ = "/tmp/minibus_test_" + std::to_string(rand());
  }

  void TearDown() override {
    unlink(path_.c_str());
  }

  std::string path_;
};

TEST_F(UdsConnectionTest, ListenAndConnect) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);
  ASSERT_GE(listen_fd, 0);

  std::unique_ptr<UdsConnection> client;
  ASSERT_EQ(UdsConnection::Connect(path_, &client), ErrorCode::Ok);
  ASSERT_TRUE(client);
  EXPECT_GE(client->Fd(), 0);

  close(listen_fd);
  client->Close();
}

TEST_F(UdsConnectionTest, AcceptReturnsConnection) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  // 另一个线程去连接
  std::thread t([this]() {
    std::unique_ptr<UdsConnection> c;
    if (UdsConnection::Connect(path_, &c) == ErrorCode::Ok) {
      c->Close();
    }
  });

  std::unique_ptr<UdsConnection> server_conn;
  ASSERT_EQ(UdsConnection::Accept(listen_fd, &server_conn), ErrorCode::Ok);
  ASSERT_TRUE(server_conn);
  EXPECT_GE(server_conn->Fd(), 0);

  t.join();
  server_conn->Close();
  close(listen_fd);
}

TEST_F(UdsConnectionTest, SendAndRecvRoundtrip) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  // Client connects
  std::unique_ptr<UdsConnection> server_conn;
  std::thread t([this]() {
    std::unique_ptr<UdsConnection> c;
    if (UdsConnection::Connect(path_, &c) == ErrorCode::Ok) {
      LinkHeader hdr{};
      hdr.magic[0] = 'M';
      hdr.magic[1] = 'I';
      hdr.version = kProtocolVersion;
      hdr.type = static_cast<uint8_t>(MessageType::PUBLISH);
      hdr.src_addr = 0x00010001;
      hdr.dst_addr = 0;
      hdr.payload_size = 5;

      EXPECT_EQ(c->SendMessage(hdr, "hello"), ErrorCode::Ok);
      c->Close();
    }
  });

  ASSERT_EQ(UdsConnection::Accept(listen_fd, &server_conn), ErrorCode::Ok);

  LinkHeader hdr{};
  std::string payload;
  ASSERT_EQ(server_conn->RecvMessage(&hdr, &payload), ErrorCode::Ok);
  EXPECT_EQ(hdr.magic[0], 'M');
  EXPECT_EQ(hdr.magic[1], 'I');
  EXPECT_EQ(hdr.type, static_cast<uint8_t>(MessageType::PUBLISH));
  EXPECT_EQ(hdr.src_addr, 0x00010001u);
  EXPECT_EQ(hdr.payload_size, 5u);
  EXPECT_EQ(payload, "hello");

  t.join();
  server_conn->Close();
  close(listen_fd);
}

TEST_F(UdsConnectionTest, ConnectToNonexistentPath) {
  std::unique_ptr<UdsConnection> client;
  EXPECT_EQ(UdsConnection::Connect("/tmp/does_not_exist_XXXX.sock", &client),
            ErrorCode::ConnectionRefused);
  EXPECT_FALSE(client);
}

TEST_F(UdsConnectionTest, SendOnClosedConnection) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  std::unique_ptr<UdsConnection> client;
  ASSERT_EQ(UdsConnection::Connect(path_, &client), ErrorCode::Ok);

  client->Close();

  LinkHeader hdr{};
  hdr.payload_size = 0;
  EXPECT_EQ(client->SendMessage(hdr, ""), ErrorCode::NotConnected);

  close(listen_fd);
}

TEST_F(UdsConnectionTest, RecvOnClosedConnection) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  std::unique_ptr<UdsConnection> client;
  ASSERT_EQ(UdsConnection::Connect(path_, &client), ErrorCode::Ok);

  client->Close();

  LinkHeader hdr{};
  std::string payload;
  EXPECT_EQ(client->RecvMessage(&hdr, &payload), ErrorCode::NotConnected);

  close(listen_fd);
}

TEST_F(UdsConnectionTest, ListenWithEmptyPath) {
  int fd;
  EXPECT_EQ(UdsConnection::Listen("", &fd), ErrorCode::InvalidArgument);
}

TEST_F(UdsConnectionTest, ConnectWithEmptyPath) {
  std::unique_ptr<UdsConnection> c;
  EXPECT_EQ(UdsConnection::Connect("", &c), ErrorCode::InvalidArgument);
}

TEST_F(UdsConnectionTest, AcceptNullOutput) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);
  EXPECT_EQ(UdsConnection::Accept(listen_fd, nullptr),
            ErrorCode::InvalidArgument);
  close(listen_fd);
}

TEST_F(UdsConnectionTest, SendLargePayload) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  std::unique_ptr<UdsConnection> server_conn;
  std::string big_payload(4096, 'x');

  std::thread t([this, &big_payload]() {
    std::unique_ptr<UdsConnection> c;
    if (UdsConnection::Connect(path_, &c) == ErrorCode::Ok) {
      LinkHeader hdr{};
      hdr.magic[0] = 'M';
      hdr.magic[1] = 'I';
      hdr.version = kProtocolVersion;
      hdr.payload_size = static_cast<uint32_t>(big_payload.size());

      EXPECT_EQ(c->SendMessage(hdr, big_payload), ErrorCode::Ok);
      c->Close();
    }
  });

  ASSERT_EQ(UdsConnection::Accept(listen_fd, &server_conn), ErrorCode::Ok);

  LinkHeader hdr{};
  std::string payload;
  ASSERT_EQ(server_conn->RecvMessage(&hdr, &payload), ErrorCode::Ok);
  EXPECT_EQ(hdr.payload_size, big_payload.size());
  EXPECT_EQ(payload, big_payload);

  t.join();
  server_conn->Close();
  close(listen_fd);
}

TEST_F(UdsConnectionTest, DestructorClosesFd) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  {
    std::unique_ptr<UdsConnection> client;
    ASSERT_EQ(UdsConnection::Connect(path_, &client), ErrorCode::Ok);
    // client 离开作用域时自动析构，Close 被调用
  }

  // 验证 server 端读会返回 NotConnected（对端已关闭）
  std::unique_ptr<UdsConnection> server_conn;
  ASSERT_EQ(UdsConnection::Accept(listen_fd, &server_conn), ErrorCode::Ok);

  LinkHeader hdr{};
  std::string payload;
  EXPECT_EQ(server_conn->RecvMessage(&hdr, &payload), ErrorCode::NotConnected);

  server_conn->Close();
  close(listen_fd);
}

TEST_F(UdsConnectionTest, EchoServerMultiMessage) {
  int listen_fd;
  ASSERT_EQ(UdsConnection::Listen(path_, &listen_fd), ErrorCode::Ok);

  std::thread t([this]() {
    std::unique_ptr<UdsConnection> c;
    if (UdsConnection::Connect(path_, &c) == ErrorCode::Ok) {
      for (int i = 0; i < 5; ++i) {
        LinkHeader hdr{};
        hdr.magic[0] = 'M';
        hdr.magic[1] = 'I';
        hdr.version = kProtocolVersion;
        hdr.type = static_cast<uint8_t>(MessageType::RPC_REQUEST);
        hdr.src_addr = i;
        hdr.payload_size = 3;

        std::string msg = "msg";
        EXPECT_EQ(c->SendMessage(hdr, msg), ErrorCode::Ok);

        // 等 server echo 回来
        LinkHeader resp_hdr{};
        std::string resp;
        EXPECT_EQ(c->RecvMessage(&resp_hdr, &resp), ErrorCode::Ok);
        EXPECT_EQ(resp, msg);
      }
      c->Close();
    }
  });

  std::unique_ptr<UdsConnection> server_conn;
  ASSERT_EQ(UdsConnection::Accept(listen_fd, &server_conn), ErrorCode::Ok);

  for (int i = 0; i < 5; ++i) {
    LinkHeader hdr{};
    std::string payload;
    ASSERT_EQ(server_conn->RecvMessage(&hdr, &payload), ErrorCode::Ok);
    EXPECT_EQ(hdr.src_addr, static_cast<uint32_t>(i));
    EXPECT_EQ(payload, "msg");

    // echo back
    hdr.src_addr = 0;
    EXPECT_EQ(server_conn->SendMessage(hdr, payload), ErrorCode::Ok);
  }

  t.join();
  server_conn->Close();
  close(listen_fd);
}

#endif  // _WIN32

}  // namespace
}  // namespace minibus
