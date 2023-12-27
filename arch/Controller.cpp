#include "Controller.h"

#include "App.h"
#include "Config.h"

#include <cassert>

using namespace std;
using namespace TgBot;

Controller::Controller() : app_(App::app()), bot_(app_->bot()) {
  //clog << "in Controller constructor\n";
}

Config* Controller::config() { return app_->config(); }

bool Controller::is_user_allowed() {
  assert(message_);
  return config()->allowed_user_ids.count( message_->from->id ) > 0;
}


void Controller::send(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, res);
}

void Controller::reply(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, res, false, message_->messageId);
}

void Controller::reply_error() {
  reply("error");
  finish();
}

