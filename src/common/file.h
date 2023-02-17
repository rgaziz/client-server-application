// file.h   1/05/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <fstream>
#include <map>
#include "logger.h"
#include "settings.h"

/* ========================================================================== */
class File {
 public:
  File(const std::string file_path) : path_(file_path) {}
  ~File() {}

 public:
  bool read();
  bool write(std::map<std::string, std::string> &data);

  std::map<std::string, std::string> &get_params();
  void set_change_flag(bool flag);

 private:
  Logger log_;
  std::string path_;
  std::map<std::string, std::string> params_;

  bool is_changed_{false};
};
/* ========================================================================== */
