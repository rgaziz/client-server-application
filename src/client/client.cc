// client.cc   1/05/2022
/* ========================================================================== */
#include "client.h"

/* ========================================================================== */
namespace client {

/* ========================================================================== */
Client::Client(File& config) {
  if (not parse_config(config)) {
    std::exit(EXIT_FAILURE);
  }

  file_ = std::make_unique<File>(file_path_);
  if (not file_->read()) {
    log_.error("Read data from data.txt failed!");
    return;
  }

  data_ = file_->get_params();
  if (data_.empty()) {
    log_.error("Keys & values not found in data.txt!");
    return;
  }

  if ((socket_ = socket(AF_INET, SOCK_STREAM, ZERO)) < ZERO) {
    log_.error("Client socket create failed!");
    std::exit(EXIT_FAILURE);
  }

  address_.sin_family = AF_INET;
  address_.sin_port = htons(port_);

  if (inet_pton(AF_INET, host_.c_str(), &address_.sin_addr) <= ZERO) {
    log_.error("Invalid host / Address not supported");
    std::exit(EXIT_FAILURE);
  }
}

/* ========================================================================== */
Client::~Client() { log_.info("Client stopped OK"); }

/* ========================================================================== */
bool Client::run() {
  log_.info("Client run ...");

  if (connect(socket_, (struct sockaddr*)&address_, sizeof(address_)) < ZERO) {
    log_.error("Connection to server failed!");
    return false;
  }

  for (auto i = 0; i < request_count_; ++i) {
    char buffer[BUFFER_SIZE];

    auto it = data_.begin();
    int random = rand() % data_.size();
    std::advance(it, random);

    int result = rand() % 100 + 1;

    std::string msg;
    if (result <= 99) {
      msg = GET + it->first;
    } else {
      msg = SET + it->first + "=value" + std::to_string(rand() % 100);
    }

    log_.debug(high, msg.c_str());

    send(socket_, msg.c_str(), msg.size(), ZERO);
    recv(socket_, buffer, BUFFER_SIZE, 0);

    log_.debug(high, buffer);
  }

  // TODO remove it
  while (true) {
  }

  log_.info("Client run OK");
  return true;
}

/* ========================================================================== */
bool Client::parse_config(File& config) {
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

  if (auto pos = params.find(kHost); pos not_eq params.end()) {
    host_ = pos->second;
  } else {
    log_.error("Host not found in config!");
    return false;
  }

  if (auto pos = params.find(kLevel); pos not_eq params.end()) {
    int level = std::atoi(pos->second.c_str());
    log_.set_level(level);
  } else {
    log_.error("Level not found in config!");
    return false;
  }

  if (auto pos = params.find(kClientRequestCount); pos not_eq params.end()) {
    request_count_ = std::atol(pos->second.c_str());
  } else {
    log_.error("Client request count not found in config!");
    return false;
  }

  if (auto pos = params.find(kPath); pos not_eq params.end()) {
    file_path_ = pos->second.c_str();
  } else {
    log_.error("Data file path not found in config!");
    return false;
  }

  log_.info("Parse config OK");
  return true;
}

/* ========================================================================== */
}  // namespace client
/* ========================================================================== */
