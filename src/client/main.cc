// client/main.cc   1/05/2022
/* ========================================================================== */
#include "client.h"

/* ========================================================================== */
int print_help() {
  std::cerr << "\nStart example:       ./Module -c data/config.txt \n\n";
  std::cerr << "[-h]                  Display this help and exit \n";

  return EXIT_FAILURE;
}

/* ========================================================================== */
enum Keys { module = 0, key = 1, path = 2 };

/* ========================================================================== */
int main(const int argc, const char **argv) {
  if (argc < path) {
    return print_help();
  }

  File config(argv[path]);
  client::Client application(config);

  return application.run();
}
