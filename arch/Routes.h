#pragma once

#include <tgbot/tgbot.h>

#include <memory>
#include <unordered_map>
#include <functional>

class App;

class Routes {
  //using routes_map = std::unordered_map<std::string, std::function<void(TgBot::Message::Ptr)>>;
  using commands_map = std::unordered_map<std::string, std::string>;

  //helper class for bot.getEvents().onCommand()
  class Command {
  public:
    Command(App* app, const std::string& command)
      : app_(app), command_(command) {}

    void to(const std::function<void(TgBot::Message::Ptr)>& listener);

  private:
    App* app_;
    std::string command_;
  }; //class Routes::Command

public:
  Routes(App* app) : app_(app)/*, temp_cmd_it_(routes_.end())*/ {}

  //interface to bot.getEvents().onCommand()
  std::unique_ptr<Routes::Command> cmd(const std::string& command);
  std::unique_ptr<Routes::Command> cmd(const std::string& command, const std::string& description);

  //void to(std::function<void(TgBot::Message::Ptr)>) {
  //}

  //bool dispatch(TgBot::Message::Ptr message) {
  //  return false;
  //}

  bool set_my_commands();

private:
  App* app_;
  //routes_map routes_;
  //routes_map::iterator temp_cmd_it_;
  commands_map commands_;

}; //class Routes

