// server.cc   1/05/2022
/* ========================================================================== */
#include "server.h"
#include <cstdlib>
#include <string>

/* ========================================================================== */
namespace server {

/* ========================================================================== */
Server::Server() { socket_ = ZERO; }

/* ========================================================================== */
Server::~Server() { log_.info("Server stopped OK"); }

/* ========================================================================== */
bool Server::init(File &config) {
  log_.info("Server init ...");

  if (not parse_config(config)) {
    log_.error("Parse config failed!");
    return false;
  }

  if (not init_network()) {
    log_.error("Init networ settings failed!");
    return false;
  }

  log_.info("Server init OK");
  return true;
}

/* ========================================================================== */
bool Server::run() {
  log_.info("Server run ...");
  is_running_ = true;

  for (int i = ZERO; i < get_thread_count_; ++i) {
    workers_.emplace_back(
        std::async(std::launch::async, &Server::get_process, this));
  }
  log_.info("Server get process OK");

  for (int i = ZERO; i < set_thread_count_; ++i) {
    workers_.emplace_back(
        std::async(std::launch::async, &Server::set_process, this));
  }
  log_.info("Server set process OK");

  workers_.emplace_back(
      std::async(std::launch::async, &Server::file_process, this));
  log_.info("Server file process OK");

  workers_.emplace_back(
      std::async(std::launch::async, &Server::statistic_process, this));
  log_.info("Statistic process OK");

  int client;
  while (is_running_) {
    client = accept(socket_, NULL, NULL);
    if (client < ZERO) {
      log_.error("Accept client failed!");
      continue;
    }

    while (is_running_) {
      char buffer[BUFFER_SIZE] = {ZERO};
      int status = recv(client, buffer, BUFFER_SIZE, ZERO);
      if (status >= ZERO) {
        if (parse_message(buffer)) {
          send(client, SUCCESS.c_str(), SUCCESS.size(), ZERO);

        } else {
          send(client, ERROR.c_str(), ERROR.size(), ZERO);
        }

      } else {
        log_.error("Get input client message failed!");
        send(client, ERROR.c_str(), ERROR.size(), ZERO);
      }
    }

    close(socket_);
  }

  return true;
}

/* ========================================================================== */
void Server::stop() {
  log_.info("Server stop ...");
  is_running_ = false;

  for (auto &it : workers_) {
    it.get();
  }
}

/* ========================================================================== */
void Server::get_process() {
  while (is_running_) {
    if (auto task = get_queue_.dequeue(); task not_eq std::nullopt) {
      if (auto pos = data_.find(*task); pos not_eq data_.end()) {
        std::lock_guard<std::mutex> guard(mutex_);
        ++get_count_;
      } else {
        log_.debug(high, "[Get] Key not found in data.txt");
      }
    }
  }
}

/* ========================================================================== */
void Server::set_process() {
  while (is_running_) {
    if (auto task = set_queue_.dequeue(); task not_eq std::nullopt) {
      const auto & [ key, value ] = *task;

      if (auto pos = data_.find(key); pos not_eq data_.end()) {
        std::lock_guard<std::mutex> guard(mutex_);
        ++set_count_;
        data_[key] = value;
        file_->set_change_flag(true);

      } else {
        log_.debug(high, "[Set] Key not found in data.txt");
      }
    }
  }
}

/* ========================================================================== */
void Server::file_process() {
  file_ = std::make_unique<File>(data_file_path_);
  if (not file_->read()) {
    log_.error("Read data from data.txt failed!");
    return;
  }

  data_ = file_->get_params();
  if (data_.empty()) {
    log_.error("Keys & values not found in data.txt!");
    return;
  }

  while (is_running_) {
    std::this_thread::sleep_for(std::chrono::seconds(save_timeout_sec_));
    std::lock_guard<std::mutex> guard(mutex_);
    file_->write(data_);
  }
}

/* ========================================================================== */
void Server::statistic_process() {
  auto info_str = "For last " + std::to_string(statistic_timeout_sec_) + " sec";
  while (is_running_) {
    std::this_thread::sleep_for(std::chrono::seconds(statistic_timeout_sec_));
    std::lock_guard<std::mutex> guard(mutex_);

    std::string get_str = "Get operation count: " + std::to_string(get_count_);
    std::string set_str = "Set operation count: " + std::to_string(set_count_);

    log_.info(info_str.c_str());
    log_.info(get_str.c_str());
    log_.info(set_str.c_str());

    get_count_ = ZERO;
    set_count_ = ZERO;
  }
}

/* ========================================================================== */
bool Server::parse_message(const char *msg) {
  log_.debug(high, "Parse message ...");
  log_.debug(high, msg);

  std::string temp(msg);
  if (auto get_pos = temp.find(GET); get_pos not_eq std::string::npos) {
    temp.erase(temp.begin(), temp.begin() + GET.size());
    log_.debug(high, temp.c_str());
    get_queue_.enqueue(temp);

  } else if (auto set_pos = temp.find(SET); set_pos not_eq std::string::npos) {
    temp.erase(temp.begin(), temp.begin() + SET.size());

    if (auto res = temp.find("="); res not_eq std::string::npos) {
      std::string key = temp.substr(0, res);
      std::string value = temp.substr(res + 1);

      log_.debug(high, key.c_str());
      log_.debug(high, value.c_str());

      set_queue_.enqueue(std::make_pair(key, value));

    } else {
      log_.error("Parse set failed!");
      return false;
    }

  } else {
    log_.error("Parse message failed! Invalid message");
    return false;
  }

  log_.debug(high, "Parse message OK");
  return true;
}

/* ========================================================================== */
bool Server::parse_config(File &config) {
  log_.info("Parse config ...");

  if (not config.read()) {
    return false;
  }

  auto params = config.get_params();
  if (params.empty()) {
    log_.error("Parse config failed!");
    return false;
  }

  if (auto pos = params.find(kPort); pos not_eq params.end()) {
    port_ = std::atoi(pos->second.c_str());
  } else {
    log_.error("Port not found in config!");
    return false;
  }

  if (auto pos = params.find(kLevel); pos not_eq params.end()) {
    int level = std::atoi(pos->second.c_str());
    log_.set_level(level);
  } else {
    log_.error("Level not found in config!");
    return false;
  }

  if (auto pos = params.find(kPath); pos not_eq params.end()) {
    data_file_path_ = pos->second.c_str();
  } else {
    log_.error("Data file path not found in config!");
    return false;
  }

  if (auto pos = params.find(kSaveTimeoutSec); pos not_eq params.end()) {
    save_timeout_sec_ = std::atol(pos->second.c_str());
  } else {
    log_.error("Save timeout second not found in config!");
    return false;
  }

  if (auto pos = params.find(kGetThreadCount); pos not_eq params.end()) {
    get_thread_count_ = std::atol(pos->second.c_str());
  } else {
    log_.error("Get thread count not found in config!");
    return false;
  }

  if (auto pos = params.find(kSetThreadCount); pos not_eq params.end()) {
    set_thread_count_ = std::atol(pos->second.c_str());
  } else {
    log_.error("Set thread count not found in config!");
    return false;
  }

  if (auto pos = params.find(kStatisticTimeoutSec); pos not_eq params.end()) {
    statistic_timeout_sec_ = std::atol(pos->second.c_str());
  } else {
    log_.error("Statistic timeout sec not found in config!");
    return false;
  }

  log_.info("Parse config OK");
  return true;
}

/* ========================================================================== */
bool Server::init_network() {
  log_.info("Init network settings ...");

  if ((socket_ = socket(AF_INET, SOCK_STREAM, ZERO)) <= ZERO) {
    log_.error("Create server socket failed!");
    return false;
  }

  address_.sin_family = AF_INET;
  address_.sin_port = htons(port_);
  address_.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socket_, (struct sockaddr *)&address_, sizeof(address_)) < ZERO) {
    log_.error("Socket bind failed!");
    return false;
  }

  if (listen(socket_, MAX_CONNECTION_COUNT) < ZERO) {
    log_.error("Listen failed!");
    return false;
  }

  log_.info("Init network settings OK");
  return true;
}

/* ========================================================================== */
}  // namespace server
/* ========================================================================== */
