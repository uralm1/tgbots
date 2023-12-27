#pragma once

#include "Controller.h"

#include <tgbot/tgbot.h>

//#include <string>
#include <memory>
#include <unordered_map>
#include <stdexcept>


struct ControllerFactoryElement {
  std::unique_ptr<AbstractControllerCreator> controller_creator_;
  std::shared_ptr<Controller> controller_cached_;

  ControllerFactoryElement(std::unique_ptr<AbstractControllerCreator> creator)
    : controller_creator_(std::move(creator)),
      controller_cached_(nullptr) {}
  //can't copy having unique_ptr
  ControllerFactoryElement(ControllerFactoryElement&&) = default;
  ControllerFactoryElement& operator=(ControllerFactoryElement&&) = default;

}; //struct ControllerFactoryElement


class ControllerFactory {
  using factory_map = std::unordered_map<std::string, ControllerFactoryElement>;

public:
  ControllerFactory() : default_controller_cached_(nullptr) { }
  ControllerFactory(const ControllerFactory&) = delete;
  ControllerFactory& operator=(const ControllerFactory&) = delete;

  template <typename C>
  void add(const std::string& command_name) {
    factory_map::iterator it = registrations_.find(command_name);
    if (it == registrations_.end()) {
      registrations_.emplace(command_name, ControllerFactoryElement(
          std::make_unique<ControllerCreator<C>>()
        ));
    } else {
      throw std::runtime_error("Registering already registered controller");
    }
  }

  //build registered controller
  std::shared_ptr<Controller> build(const std::string& command_name,
      TgBot::Message::Ptr message);

  //build default controller
  std::shared_ptr<Controller> build(TgBot::Message::Ptr message);

private:
  factory_map registrations_;
  std::shared_ptr<Controller> default_controller_cached_;

}; //class ControllerFactory

