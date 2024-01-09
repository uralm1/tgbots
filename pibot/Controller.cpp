#include "Controller.h"

#include "BotApp.h"

#include "command.h"

#include <cstdlib>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace std;
using namespace TgBot;

Controller::Controller(BotApp* app) : app_(app), bot_(app_->bot()) {
  //clog << "in Controller constructor\n";
}

Config* Controller::config() { return app_->config(); }

bool Controller::user_allowed() {
  if (user_allowed_internal_()) {
    return true;
  } else {
    cout << "Access denied for command: " << message_->text << endl;
  }
  finish();
  return false;
}


string Controller::md_escape(const string& s) {
  string r;
  const char *nb_escaped_chars = "!<>#(){}|.";
  bool blockt_flag = false; //`
  unsigned cct = 0;

  for (auto it = s.begin(); it != s.end(); ++it) {
    auto c = *it;
    if (c == '`') {
      if (cct < 3) ++cct;
      if (cct == 3) {
        blockt_flag = !blockt_flag;
        cct = 0;
      }
    } else {
      if (cct > 0) cct = 0;
    }

    if (!blockt_flag && strchr(nb_escaped_chars, c)) r += '\\';
    r += c;
  }
  return r;
}


void Controller::send(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, /*chatId*/
      md_escape(res), /*text*/
      true, /*disableWebPagePreview*/
      0, /*replyToMessageId*/
      nullptr, /*replyMarkup*/
      "MarkdownV2" /*parseMode*/);
}

void Controller::reply(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, /*chatId*/
      md_escape(res), /*text*/
      true, /*disableWebPagePreview*/
      message_->messageId, /*replyToMessageId*/
      nullptr, /*replyMarkup*/
      "MarkdownV2" /*parseMode*/);
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
  cout << "Executing: " << cmd << endl;
  raymii::CommandResult r = raymii::Command::exec(cmd);
  //cout << r << endl;
  cout << "Command status: " << r.exitstatus << endl;
  if (r.exitstatus == 0)
    send("```\n" + r.output + "```");
  else
    send("Command execution error (status: " + to_string(r.exitstatus) + ").");
}

void Controller::run_without_output(const string& cmd) {
  cout << "Executing: " << cmd << endl;
  int r = system(cmd.c_str());
  cout << "Command status: " << r << endl;
  send("Command status: " + to_string(r) + ".");
}

