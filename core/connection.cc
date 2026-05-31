#include "core/connection.h"

#include <cerrno>
#include <cstring>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace minibus {

#ifndef _WIN32
// ============ Linux implementation ============

ErrorCode UdsConnection::ReadFull(int fd, uint8_t* buf, size_t len) {
  size_t offset = 0;
  while (offset < len) {
    ssize_t n = recv(fd, buf + offset, len - offset, 0);
    if (n < 0) {
      if (errno == EINTR) continue;
      return ErrorCode::BrokenPipe;
    }
    if (n == 0) {
      return ErrorCode::NotConnected;
    }
    offset += static_cast<size_t>(n);
  }
  return ErrorCode::Ok;
}

ErrorCode UdsConnection::WriteFull(int fd, const uint8_t* buf, size_t len) {
  size_t offset = 0;
  while (offset < len) {
    ssize_t n = send(fd, buf + offset, len - offset, MSG_NOSIGNAL);
    if (n < 0) {
      if (errno == EINTR) continue;
      return ErrorCode::BrokenPipe;
    }
    offset += static_cast<size_t>(n);
  }
  return ErrorCode::Ok;
}

ErrorCode UdsConnection::Listen(const std::string& path, int* out_fd) {
  if (path.empty() || !out_fd) {
    return ErrorCode::InvalidArgument;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) return ErrorCode::Internal;

  unlink(path.c_str());

  struct sockaddr_un addr {};
  addr.sun_family = AF_UNIX;
  if (path.size() >= sizeof(addr.sun_path)) {
    close(fd);
    return ErrorCode::InvalidArgument;
  }
  std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

  if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
    close(fd);
    return ErrorCode::Internal;
  }

  if (listen(fd, 8) < 0) {
    close(fd);
    return ErrorCode::Internal;
  }

  *out_fd = fd;
  return ErrorCode::Ok;
}

ErrorCode UdsConnection::Accept(int listen_fd,
                                std::unique_ptr<UdsConnection>* out) {
  if (!out) return ErrorCode::InvalidArgument;

  int client_fd;
  do {
    client_fd = accept(listen_fd, nullptr, nullptr);
  } while (client_fd < 0 && errno == EINTR);

  if (client_fd < 0) {
    return ErrorCode::Internal;
  }

  *out = std::unique_ptr<UdsConnection>(new UdsConnection(client_fd));
  return ErrorCode::Ok;
}

ErrorCode UdsConnection::Connect(const std::string& path,
                                 std::unique_ptr<UdsConnection>* out) {
  if (path.empty() || !out) {
    return ErrorCode::InvalidArgument;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) return ErrorCode::Internal;

  struct sockaddr_un addr {};
  addr.sun_family = AF_UNIX;
  if (path.size() >= sizeof(addr.sun_path)) {
    close(fd);
    return ErrorCode::InvalidArgument;
  }
  std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

  if (connect(fd, reinterpret_cast<struct sockaddr*>(&addr),
              sizeof(addr)) < 0) {
    close(fd);
    if (errno == ENOENT || errno == ECONNREFUSED) {
      return ErrorCode::ConnectionRefused;
    }
    return ErrorCode::Internal;
  }

  *out = std::unique_ptr<UdsConnection>(new UdsConnection(fd));
  return ErrorCode::Ok;
}

ErrorCode UdsConnection::SendMessage(const LinkHeader& header,
                                     const std::string& payload) {
  if (fd_ < 0) return ErrorCode::NotConnected;

  uint8_t header_buf[sizeof(LinkHeader)];
  if (!EncodeHeader(header, header_buf, sizeof(header_buf))) {
    return ErrorCode::InvalidMessage;
  }

  ErrorCode err = WriteFull(fd_, header_buf, sizeof(header_buf));
  if (err != ErrorCode::Ok) return err;

  if (!payload.empty()) {
    err = WriteFull(fd_, reinterpret_cast<const uint8_t*>(payload.data()),
                    payload.size());
    if (err != ErrorCode::Ok) return err;
  }

  return ErrorCode::Ok;
}

ErrorCode UdsConnection::RecvMessage(LinkHeader* header,
                                     std::string* payload) {
  if (fd_ < 0) return ErrorCode::NotConnected;
  if (!header || !payload) return ErrorCode::InvalidArgument;

  uint8_t header_buf[sizeof(LinkHeader)];
  ErrorCode err = ReadFull(fd_, header_buf, sizeof(header_buf));
  if (err != ErrorCode::Ok) return err;

  err = DecodeHeader(header_buf, sizeof(header_buf), header);
  if (err != ErrorCode::Ok) return err;

  if (header->payload_size > 0) {
    payload->resize(header->payload_size);
    err = ReadFull(fd_, reinterpret_cast<uint8_t*>(payload->data()),
                   header->payload_size);
    if (err != ErrorCode::Ok) return err;
  } else {
    payload->clear();
  }

  return ErrorCode::Ok;
}

void UdsConnection::Close() {
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
}

#else  // _WIN32
// ============ Windows stub implementation ============

ErrorCode UdsConnection::ReadFull(int, uint8_t*, size_t) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::WriteFull(int, const uint8_t*, size_t) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::Listen(const std::string&, int*) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::Accept(int, std::unique_ptr<UdsConnection>*) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::Connect(const std::string&,
                                 std::unique_ptr<UdsConnection>*) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::SendMessage(const LinkHeader&, const std::string&) {
  return ErrorCode::Internal;
}

ErrorCode UdsConnection::RecvMessage(LinkHeader*, std::string*) {
  return ErrorCode::Internal;
}

void UdsConnection::Close() { fd_ = -1; }

#endif  // _WIN32

UdsConnection::~UdsConnection() { Close(); }

}  // namespace minibus
