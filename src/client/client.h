// client.h   1/05/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "common/file.h"
#include "common/logger.h"
#include "common/settings.h"

/* ========================================================================== */
namespace client {

/* ========================================================================== */
class Client {
 public:
  Client(File& config);
  ~Client();

 public:
  bool run();

 private:
  bool parse_config(File& config);

 private:
  Logger log_;
  std::string file_path_;
  int64_t request_count_{DEFAULT};
  std::unique_ptr<File> file_{nullptr};
  std::map<std::string, std::string> data_;

 private:
  int port_{PORT};
  int socket_{ZERO};
  std::string host_{LOCALHOST};
  struct sockaddr_in address_;
};
/* ========================================================================== */
}  // namespace client
/* ========================================================================== */
