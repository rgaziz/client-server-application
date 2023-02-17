// server.h   1/05/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <netinet/in.h>
#include <sys/socket.h>
#include <future>
#include "common/file.h"
#include "common/logger.h"
#include "common/settings.h"
#include "queue.h"
#include "unistd.h"

/* ========================================================================== */
namespace server {

/* ========================================================================== */
class Server {
 public:
  Server();
  ~Server();

 public:
  bool init(File &file);
  bool run();
  void stop();

 private:
  void get_process();
  void set_process();
  void file_process();
  void statistic_process();

 private:
  bool parse_message(const char *msg);
  bool parse_config(File &config);
  bool init_network();

 private:
  Logger log_;
  std::mutex mutex_;

  bool is_running_{false};
  std::string data_file_path_;

  std::unique_ptr<File> file_{nullptr};
  std::map<std::string, std::string> data_;

  std::vector<std::future<void>> workers_;

  SafeQueue<std::string> get_queue_;
  SafeQueue<std::pair<std::string, std::string>> set_queue_;

 private:
  int socket_;
  int port_{PORT};
  int opt_{DEFAULT};

  struct sockaddr_in address_;

 private:
  int64_t set_count_{ZERO};
  int64_t get_count_{ZERO};

  int64_t set_thread_count_{DEFAULT};
  int64_t get_thread_count_{DEFAULT};

  int64_t save_timeout_sec_{WAIT_SECONDS};
  int64_t statistic_timeout_sec_{WAIT_SECONDS};
};
/* ========================================================================== */
}  // namespace server
/* ========================================================================== */
