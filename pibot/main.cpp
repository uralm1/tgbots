#include "BotApp.h"

#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc >= 2 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 ||
       argc > 2)) {
    cout << "Help: pibot [pibot.yml]" << endl;
    exit(EXIT_SUCCESS);
  }

  const char* config_file = "pibot.yml";
  if (argc == 2) {
    config_file = argv[1];
  }

  static BotApp app(config_file);
  return app.start();
}

