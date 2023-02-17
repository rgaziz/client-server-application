// server/main.cc   1/05/2022
/* ========================================================================== */
#include <unistd.h>
#include <csignal>
#include <iostream>
#include "server.h"

/* ========================================================================== */
static server::Server application;

/* ========================================================================== */
void handle_signal(int signal) {
  // if (signal not_eq SIGPIPE) {
  std::cout << "Signal: " << signal << std::endl;
  application.stop();
  std::exit(EXIT_SUCCESS);
  // }
}

/* ========================================================================== */
int print_help() {
  std::cerr << "\nStart example:       ./Module -c data/config.txt \n\n";
  std::cerr << "[-h]                  Display this help and exit \n";

  return EXIT_FAILURE;
}

/* ========================================================================== */
void register_signals() {
  struct sigaction sig_action;
  sig_action.sa_handler = handle_signal;
  ::sigemptyset(&sig_action.sa_mask);
  sig_action.sa_flags = SA_RESTART;

  if (::sigaction(SIGINT, &sig_action, nullptr) < ZERO) {
    std::exit(EXIT_FAILURE);
  }

  if (::sigaction(SIGTERM, &sig_action, nullptr) < ZERO) {
    std::exit(EXIT_FAILURE);
  }

  if (::sigaction(SIGABRT, &sig_action, nullptr) < ZERO) {
    std::exit(EXIT_FAILURE);
  }

  if (::sigaction(SIGPIPE, &sig_action, nullptr) < ZERO) {
    std::exit(EXIT_FAILURE);
  }
}

/* ========================================================================== */
enum Keys { module = 0, key = 1, path = 2 };

/* ========================================================================== */
int main(const int argc, const char **argv) {
  if (argc < path) {
    return print_help();
  }

  register_signals();

  File config(argv[path]);
  if (not application.init(config)) {
    return EXIT_FAILURE;
  }

  return application.run();
}
/* ========================================================================== */
