// connection.h - Unix Domain Socket synchronous I/O wrapper
#pragma once

#include <memory>
#include <string>

#include "core/errors.h"
#include "core/message_codec.h"

namespace minibus {

class UdsConnection {
 public:
  ~UdsConnection();

  // Server: listen + accept
  static ErrorCode Listen(const std::string& path, int* out_fd);
  static ErrorCode Accept(int listen_fd,
                          std::unique_ptr<UdsConnection>* out);

  // Client: connect to server
  static ErrorCode Connect(const std::string& path,
                           std::unique_ptr<UdsConnection>* out);

  // Synchronous blocking I/O
  ErrorCode SendMessage(const LinkHeader& header,
                        const std::string& payload);
  ErrorCode RecvMessage(LinkHeader* header, std::string* payload);

  int Fd() const { return fd_; }
  void Close();

 private:
  explicit UdsConnection(int fd) : fd_(fd) {}

  // Read/write exactly N bytes, handling EINTR and partial I/O
  static ErrorCode ReadFull(int fd, uint8_t* buf, size_t len);
  static ErrorCode WriteFull(int fd, const uint8_t* buf, size_t len);

  int fd_ = -1;
};

}  // namespace minibus
