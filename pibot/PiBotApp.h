#pragma once

//#define HAVE_CURL

#include "Config.h"
#include "Controller.h"

#include <tgbot/tgbot.h>

#include <string>
//#include <iostream>


class PiBotApp {
public:
  PiBotApp(const std::string& config_file);
  PiBotApp(const PiBotApp&) = delete;
  PiBotApp& operator=(const PiBotApp&) = delete;

  virtual ~PiBotApp() { /*std::clog << "in PiBotApp destructor\n";*/ }

  void startup();
  int start();

  TgBot::Bot* bot() { return &bot_; }
  Config* config() { return &config_; }
  Controller* controller(TgBot::Message::Ptr message) {
    controller_.message(std::move(message));
    return &controller_;
  }

  void set_my_commands();

private:
  YamlConfig config_;
#ifdef HAVE_CURL
  TgBot::CurlHttpClient curlHttpClient_;
#endif
  TgBot::Bot bot_;
  Controller controller_;

}; //class PiBotApp

