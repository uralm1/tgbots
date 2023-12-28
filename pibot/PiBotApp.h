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
  bool user_allowed(const TgBot::Message::Ptr& message);
  void send(const TgBot::Message::Ptr& message, const std::string& res);
  void reply(const TgBot::Message::Ptr& message, const std::string& res);
  void reply_error(const TgBot::Message::Ptr& message);

  void cmd_reboot(TgBot::Message::Ptr message);
  void cmd_rebootcheck(TgBot::Message::Ptr message);
  void cmd_status(TgBot::Message::Ptr message);
  void cmd_restart(TgBot::Message::Ptr message);
  void cmd_start(TgBot::Message::Ptr message);
  void cmd_stop(TgBot::Message::Ptr message);

  std::string param(const TgBot::Message::Ptr& message, unsigned int idx);
  std::string param2(const TgBot::Message::Ptr& message) { return param(message, 2); }
  void run_with_output(const TgBot::Message::Ptr& message, const std::string& cmd);
  void run_without_output(const TgBot::Message::Ptr& message, const std::string& cmd);

private:
  bool user_allowed_internal_(const TgBot::Message::Ptr& message) {
    return config_.allowed_user_ids.count( message->from->id ) > 0;
  }

private:
  YamlConfig config_;
#ifdef HAVE_CURL
  TgBot::CurlHttpClient curlHttpClient_;
#endif
  TgBot::Bot bot_;

}; //class PiBotApp

