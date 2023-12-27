#include "ControllerFactory.h"
//#include "Controller.h"

#include <stdexcept>
#include <cassert>

using namespace std;
using namespace TgBot;


std::shared_ptr<Controller> ControllerFactory::build(
    const std::string& command_name,
    Message::Ptr message) {
  factory_map::iterator it = registrations_.find(command_name);
  if (it != registrations_.end()) {
    auto& el = it->second;
    if (el.controller_cached_) {
      //clog << "in ControllerFactory registered controller build reuse\n";
      //replace message if controller is already created
      el.controller_cached_->message(std::move(message));
    } else {
      //clog << "in ControllerFactory registered controller build create new\n";
      assert(el.controller_creator_);
      el.controller_cached_ = el.controller_creator_->build(std::move(message));
    }
    return el.controller_cached_;

  } else {
    throw std::runtime_error("Attempt to create unregistered controller");
  }
}

std::shared_ptr<Controller> ControllerFactory::build(
    Message::Ptr message) {
  if (default_controller_cached_) {
    //clog << "in ControllerFactory default controller build reuse\n";
    //replace message if controller is already created
    default_controller_cached_->message(std::move(message));
  } else {
    //clog << "in ControllerFactory default controller build create new\n";
    default_controller_cached_ = ControllerCreator<Controller>().build(std::move(message));
  }
  return default_controller_cached_;
}

