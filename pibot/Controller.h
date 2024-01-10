#pragma once

#include "Config.h"

#include <tgbot/tgbot.h>

//#include <iostream>
#include <cassert>

class BotApp;


class Controller {
public:
  Controller(BotApp* app);
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
  virtual ~Controller() { /*std::clog << "in Controller destructor\n";*/ }

  BotApp* app() { return app_; }
  TgBot::Bot* bot() { return bot_; }
  Config* config() { return config_; }

  const TgBot::Message::Ptr& message() { return message_; }
  const TgBot::Message::Ptr& message(TgBot::Message::Ptr message) {
    message_ = std::move(message);
    return message_;
  }

  //check "from" userid is allowed to accept commands
  //  finish(), cout message and return false if NOT
  bool check_access();

  //set message, then return check_access()
  bool init_and_check_access(TgBot::Message::Ptr message) {
    message_ = std::move(message);
    return check_access();
  }

  static std::string md_escape(const std::string& s);

  virtual void send(const std::string& res);
  virtual void reply(const std::string& res);
  void finish() {
    //std::clog << "message usage count: " << message_.use_count() << std::endl;
    message_.reset();
  }

  //reply error and finish session
  void reply_error();

  std::string param(unsigned int idx);
  std::string param2() { return param(2); }
  void run_with_output(const std::string& cmd);
  void run_without_output(const std::string& cmd);
  void execute_cmd(const Config::CommandParam& cmd_param);

  //commands
  void cmd_handler(const Config::CommandParam& cmd_param, TgBot::Message::Ptr message);
  void cmd_help(TgBot::Message::Ptr message);
  void cmd_cam(TgBot::Message::Ptr message);
  //void cmd_additional(TgBot::Message::Ptr message);

protected:
  bool user_allowed_internal() {
    assert(message_);
    return config_->allowed_user_ids.count( message_->from->id ) > 0;
  }

private:
  BotApp* app_;
  TgBot::Bot* bot_;
  Config* config_;
  TgBot::Message::Ptr message_;

friend class BotApp;
}; //class Controller

