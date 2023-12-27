#pragma once

#include <tgbot/tgbot.h>

//#include <iostream>
#include <memory>

class App;
class Config;


class Controller {
public:
  Controller();
  virtual ~Controller() { /*std::clog << "in Controller destructor\n";*/ }

  App* app() { return app_; }
  TgBot::Bot* bot() { return bot_; }
  Config* config();

  const TgBot::Message::Ptr& message() { return message_; }
  const TgBot::Message::Ptr& message(TgBot::Message::Ptr message) {
    message_ = std::move(message);
    return message_;
  }

  //check is "from" userid allowed to accept commands
  bool is_user_allowed();

  virtual void send(const std::string& res);
  virtual void reply(const std::string& res);
  void finish() {
    std::clog << "message usage count: " << message_.use_count() << std::endl;
    message_.reset();
  }

  //reply error and finish session
  void reply_error();

private:
  App* app_;
  TgBot::Bot* bot_;
  TgBot::Message::Ptr message_;
}; //class Controller


class AbstractControllerCreator {
public:
  virtual std::shared_ptr<Controller> build(TgBot::Message::Ptr message) const = 0;
}; //class AbstractControllerCreator


template <typename C>
class ControllerCreator : public AbstractControllerCreator {
public:
  virtual std::shared_ptr<Controller> build(TgBot::Message::Ptr message) const {
    auto c = std::make_shared<C>();
    c->message(std::move(message));
    return c;
  }
}; //class ControllerCreator

