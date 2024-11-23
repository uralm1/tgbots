#pragma once

#include "Config.h"

#include <tgbot/tgbot.h>

//#include <iostream>
#include <cassert>
#include <string_view>
#include <queue>

class BotApp;


class Controller {
public:
  Controller(BotApp* app);
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;
  virtual ~Controller() { /*std::clog << "in Controller destructor\n";*/ }

  BotApp* app() const { return app_; }
  TgBot::Bot* bot() const { return bot_; }
  Config* config() const { return config_; }

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

  static std::string md_escape(const std::string_view s);

  virtual void send(const std::string_view resp) const {
    assert(message_);
    int64_t to = config()->send_only_to_chat_id;
    _send(&bot_->getApi(), to == 0 ? message_->chat->id : to, resp);
  }
  virtual void reply(const std::string_view resp) const;
  void finish() {
    //std::clog << "message usage count: " << message_.use_count() << std::endl;
    message_.reset();
  }

  //reply error and finish session
  void reply_error();

  std::string param(unsigned int idx);
  std::string param2() { return param(2); }

  //just a simplified copy of Config::CommandParam with destination address
  class Command {
  public:
    Command(std::int64_t chatId_to, const Config::CommandParam& cmd_param)
      : to_(chatId_to),
      pre_send_(cmd_param.pre_send),
      post_send_(cmd_param.post_send),
      run_without_output_(cmd_param.run_without_output),
      run_with_output_(cmd_param.run_with_output)
    { }

    void execute(const TgBot::Api* api) const {
      assert(api);
      if (!pre_send_.empty()) send(api, pre_send_);
      if (!run_with_output_.empty()) run_with_output(api);
      if (!run_without_output_.empty()) run_without_output(api);
      if (!post_send_.empty()) send(api, post_send_);
    }

  private:
    std::int64_t to_; //chatId to
    std::string_view pre_send_;
    std::string_view post_send_;
    std::string_view run_without_output_;
    std::string_view run_with_output_;

    void send(const TgBot::Api* api, const std::string_view resp) const {
      Controller::_send(api, to_, resp);
    }
    void run_with_output(const TgBot::Api* api) const;
    void run_without_output(const TgBot::Api* api) const;
  }; //class Command

  void queue_command(const Config::CommandParam& cmd_param) {
    assert(message_);
    std::int64_t to = config()->send_only_to_chat_id;
    command_queue_.push(Command{to == 0 ? message_->chat->id : to, cmd_param});
  }

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

  bool has_commands_in_queue() { return !command_queue_.empty(); }
  void execute_queued_commands() {
    for (; !command_queue_.empty(); command_queue_.pop())
      command_queue_.front().execute(&this->bot_->getApi());
  }

private:
  static void _send(const TgBot::Api* api, const std::int64_t to, const std::string_view resp);

private:
  BotApp* app_;
  TgBot::Bot* bot_;
  Config* config_;
  TgBot::Message::Ptr message_;
  std::queue<Command> command_queue_;

friend class BotApp;
friend class Poller;
}; //class Controller

