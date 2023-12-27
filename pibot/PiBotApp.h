#pragma once

//#define HAVE_CURL

#include "Config.h"

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

  void set_my_commands();
  bool is_user_allowed(const TgBot::Message::Ptr& message);
  void send(const TgBot::Message::Ptr& message, const std::string& res);
  void reply(const TgBot::Message::Ptr& message, const std::string& res);
  void reply_error(const TgBot::Message::Ptr& message);

private:
  YamlConfig config_;
#ifdef HAVE_CURL
  TgBot::CurlHttpClient curlHttpClient_;
#endif
  TgBot::Bot bot_;

}; //class PiBotApp

