// logger.h   1/05/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <iostream>
#include <mutex>
#include <thread>

/* ========================================================================== */
enum Level { low = 10, middle = 50, high = 100 };

/* ========================================================================== */
class Logger {
 public:
  Logger() {}
  ~Logger() {}

 public:
  void info(const char *msg) {
    std::lock_guard<std::mutex> guard(log_mutex_);
    std::cout << "[INF]"
              << "(" << std::this_thread::get_id() << "): " << msg << std::endl;
  }

  void error(const char *msg) {
    std::lock_guard<std::mutex> guard(log_mutex_);
    std::cerr << "[ERR]"
              << "(" << std::this_thread::get_id() << "): " << msg << std::endl;
  }

  void debug(const int &level, const char *msg) {
    if (level <= log_level_) {
      std::lock_guard<std::mutex> guard(log_mutex_);
      std::cout << "[DEB]"
                << "(" << std::this_thread::get_id() << "): " << msg
                << std::endl;
    }
  }

  void set_level(const int level) { log_level_ = level; }

 private:
  std::mutex log_mutex_;
  int log_level_{Level::low};
};
/* ========================================================================== */
