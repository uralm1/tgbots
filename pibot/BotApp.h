#pragma once

//#define HAVE_CURL

#include "Config.h"
#include "Controller.h"

#include <tgbot/tgbot.h>

#include <string>
//#include <iostream>


class BotApp {
public:
  BotApp(const std::string& config_file);
  BotApp(const BotApp&) = delete;
  BotApp& operator=(const BotApp&) = delete;

  virtual ~BotApp() { /*std::clog << "in BotApp destructor\n";*/ }

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

}; //class BotApp

