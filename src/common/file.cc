// file.cc   1/05/2022
/* ========================================================================== */
#include "file.h"

/* ========================================================================== */
bool File::read() {
  log_.debug(middle, "File read ...");

  std::ifstream in_(path_);
  std::string key, value;

  if (in_.is_open()) {
    while (in_ >> key >> value) {
      params_.insert(std::pair<std::string, std::string>(key, value));
    }

  } else {
    log_.error("File read failed!");
    return false;
  }

  in_.close();
  log_.debug(middle, "File read OK");
  return true;
}

/* ========================================================================== */
bool File::write(std::map<std::string, std::string> &data) {
  log_.debug(high, "File write ...");

  if (not is_changed_) {
    log_.debug(high, "File is not changed");
    return true;
  }

  std::ofstream out_(path_);
  if (out_.is_open()) {
    for (const auto &value : data) {
      out_ << value.first << " " << value.second << std::endl;
    }
  }

  out_.close();

  is_changed_ = false;
  log_.debug(high, "File write OK");
  return true;
}

/* ========================================================================== */
std::map<std::string, std::string> &File::get_params() { return params_; }

/* ========================================================================== */
void File::set_change_flag(bool flag) { is_changed_ = flag; }

/* ========================================================================== */
