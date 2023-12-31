#pragma once

#include <tgbot/tgbot.h>

//#include <iostream>

class PiBotApp;
class Config;


class Controller {
public:
  Controller(PiBotApp* app);
  virtual ~Controller() { /*std::clog << "in Controller destructor\n";*/ }

  PiBotApp* app() { return app_; }
  TgBot::Bot* bot() { return bot_; }
  Config* config();

  const TgBot::Message::Ptr& message() { return message_; }
  const TgBot::Message::Ptr& message(TgBot::Message::Ptr message) {
    message_ = std::move(message);
    return message_;
  }

  //check is "from" userid allowed to accept commands
  bool user_allowed();

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

protected:
  bool user_allowed_internal_();

private:
  PiBotApp* app_;
  TgBot::Bot* bot_;
  TgBot::Message::Ptr message_;

friend class PiBotApp;
}; //class Controller

