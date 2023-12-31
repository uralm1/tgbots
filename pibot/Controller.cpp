#include "Controller.h"

#include "PiBotApp.h"
#include "Config.h"

#include "command.h"

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace std;
using namespace TgBot;

Controller::Controller(PiBotApp* app) : app_(app), bot_(app_->bot()) {
  //clog << "in Controller constructor\n";
}

inline Config* Controller::config() { return app_->config(); }

inline bool Controller::user_allowed_internal_() {
  assert(message_);
  return config()->allowed_user_ids.count( message_->from->id ) > 0;
}

bool Controller::user_allowed() {
  if (user_allowed_internal_()) {
    cout << "Processing command: " << message_->text << endl;
    return true;
  } else {
    cout << "Access denied for command: " << message_->text << endl;
  }
  finish();
  return false;
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


string Controller::param(unsigned int idx) {
  const string& text = message_->text;
  boost::char_separator<char> sep(" \t");
  boost::tokenizer<boost::char_separator<char>> tok(text, sep);
  decltype(tok)::iterator it = tok.begin();

  for (unsigned int i = 1; i <= idx; ++i) {
    if (it == tok.end()) break;
    if (i == idx) return *it;
    ++it;
  }
  return string();
}


void Controller::run_with_output(const string& cmd) {
  cout << "Running command: " << cmd << endl;
  raymii::CommandResult r = raymii::Command::exec(cmd);
  //cout << r << endl;
  cout << "Command status: " << r.exitstatus << endl;
  if (r.exitstatus == 0)
    send(r.output);
  else
    send("Command execution error (status: " + to_string(r.exitstatus) + ").");
}

void Controller::run_without_output(const string& cmd) {
  cout << "Running command: " << cmd << endl;
  int r = system(cmd.c_str());
  cout << "Command status: " << r << endl;
  send("Command status: " + to_string(r) + ".");
}

