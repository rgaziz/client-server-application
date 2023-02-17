// settings.h   1/05/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <string>

/* ========================================================================== */
static const std::string kStatisticTimeoutSec = "StatisticTimeoutSec";
static const std::string kClientRequestCount = "ClientRequestCount";
static const std::string kSaveTimeoutSec = "SaveTimeoutSec";
static const std::string kGetThreadCount = "GetThreadCount";
static const std::string kSetThreadCount = "SetThreadCount";

static const std::string kLevel = "Level";
static const std::string kHost = "Host";
static const std::string kPort = "Port";
static const std::string kPath = "Path";

/* ========================================================================== */
static const int ZERO = 0;
static const int DEFAULT = 1;
static const int PORT = 8080;
static const int BUFFER_SIZE = 256;
static const int WAIT_SECONDS = 10;
static const int MAX_CONNECTION_COUNT = 10;

static const std::string SUCCESS = "SUCCESS";
static const std::string ERROR = "ERROR";

static const std::string GET = "$get ";
static const std::string SET = "$set ";

static const std::string LOCALHOST = "127.0.0.1";

/* ========================================================================== */
struct Data {
  std::string key_;
  std::string value_;

  int64_t set_count_;
  int64_t get_count_;
};
/* ========================================================================== */
