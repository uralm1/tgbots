#include "StartController.h"

#include <cstdio>

using namespace std;
//using namespace TgBot;

void StartController::start() {
  send("Hi, start!");

  printf("Chat id %ld\n", message()->chat->id);
  printf("From id %ld, username: %s\n", message()->from->id, message()->from->username.c_str());
  printf("Msg text %s\n", message()->text.c_str());

  finish();
}

