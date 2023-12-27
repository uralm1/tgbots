#pragma once

//#define HAVE_CURL

#include "Config.h"
#include "Routes.h"
#include "ControllerFactory.h"

#include <tgbot/tgbot.h>

#include <string>
#include <memory>
//#include <iostream>

extern App* app_global_instance;

class App {
public:
  App(const std::string& config_file);
  App(const App&) = delete;
  App& operator=(const App&) = delete;

  virtual ~App() { /*std::clog << "in App destructor\n";*/ }

  virtual void startup() = 0;
  virtual int start();

  static App* app() { return app_global_instance; }
  TgBot::Bot* bot() { return &bot_; }
  Config* config() { return &config_; }
  Routes* routes() { return &routes_; }

  //register new controller
  template <typename C>
  void register_controller(const std::string& command_name) {
    controller_factory_.add<C>(command_name);
  }
  //build registered controller
  std::shared_ptr<Controller> build_controller(
      const std::string& command_name,
      TgBot::Message::Ptr message) {
    return controller_factory_.build(command_name, std::move(message));
  }
  //build default controller (Controller class)
  std::shared_ptr<Controller> build_controller(TgBot::Message::Ptr message) {
    return controller_factory_.build(std::move(message));
  }

  //register all bot commands in Telegram
  bool set_my_commands() { return routes_.set_my_commands(); }

private:
  //void dispatch(std::shared_ptr<Controller> c);
  //void handler(TgBot::Message::Ptr message);

private:
  YamlConfig config_;
  Routes routes_;
#ifdef HAVE_CURL
  TgBot::CurlHttpClient curlHttpClient_;
#endif
  TgBot::Bot bot_;
  ControllerFactory controller_factory_;

}; //class App

