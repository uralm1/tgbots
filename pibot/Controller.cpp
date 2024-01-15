#include "Controller.h"

#include "BotApp.h"

#include "command.h"
#include "utf8.h"

#include <cstdlib>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace std;
using namespace TgBot;

Controller::Controller(BotApp* app)
  : app_(app),
    bot_(app_->bot()),
    config_(app_->config()) {
  //clog << "in Controller constructor\n";
}

bool Controller::check_access() {
  if (user_allowed_internal()) {
    return true;
  } else {
    cout << "Access denied for command: " << message_->text << endl;
  }
  finish();
  return false;
}


string Controller::md_escape(const string& s) {
  const char *nb_escaped_chars = "!<>#(){}|.-";
  bool blockt_flag = false; //`
  unsigned cct = 0;

  try {
    u32string r;
    utf8::iterator it_end(s.end(), s.begin(), s.end());
    for (utf8::iterator it(s.begin(), s.begin(), s.end()); it != it_end; ++it) {
      char32_t c = (char32_t)*it;
      if (c == '`') {
        if (cct < 3) ++cct;
        if (cct == 3) {
          blockt_flag = !blockt_flag;
          cct = 0;
        }
      } else {
        if (cct > 0) cct = 0;
      }

      if (!blockt_flag && c >= 0 && c < 128 && strchr(nb_escaped_chars, (char)c)) r += '\\';
      r += c;
    }
    return utf8::utf32to8(r);
  } catch (const utf8::exception& e) {
    cerr << "bad utf8: " << e.what() << endl;
  }
  return string();
}


void Controller::send(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  try {
    bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, /*chatId*/
        res.empty() ? "empty" : md_escape(res), /*text*/
        true, /*disableWebPagePreview*/
        0, /*replyToMessageId*/
        nullptr, /*replyMarkup*/
        "MarkdownV2" /*parseMode*/);
  } catch (const TgException& e) {
    cerr << "TgException in send: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
  }
}

void Controller::reply(const string& res) {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  try {
    bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, /*chatId*/
        res.empty() ? "empty" : md_escape(res), /*text*/
        true, /*disableWebPagePreview*/
        message_->messageId, /*replyToMessageId*/
        nullptr, /*replyMarkup*/
        "MarkdownV2" /*parseMode*/);
  } catch (const TgException& e) {
    cerr << "TgException in reply: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
  }
}

void Controller::reply_error() {
  reply("error");
  finish();
}


string Controller::param(unsigned int idx) {
  try {
    u32string text32 = utf8::utf8to32(message_->text);
    boost::char_separator<char32_t> sep(U" \t");
    boost::tokenizer<boost::char_separator<char32_t>,
      std::u32string::const_iterator, std::u32string> tok(text32, sep);
    decltype(tok)::iterator it = tok.begin();

    for (unsigned int i = 1; i <= idx; ++i) {
      if (it == tok.end()) break;
      if (i == idx) return utf8::utf32to8(*it);
      ++it;
    }
  } catch (const utf8::exception& e) {
    cerr << "bad utf8: " << e.what() << endl;
  }
  return string();
}


void Controller::run_with_output(const string& cmd) {
  cout << "Executing: " << cmd << endl;
  raymii::CommandResult r = raymii::Command::exec(cmd);
  //cout << r << endl;
  cout << "Command status: " << r.exitstatus << endl;
  if (r.exitstatus == 0) {
    if (r.output.empty()) r.output = "result is empty";
    send("```\n" + r.output + "```");
  } else
    send("Command execution error (status: " + to_string(r.exitstatus) + ").");
}

void Controller::run_without_output(const string& cmd) {
  cout << "Executing: " << cmd << endl;
  int r = system(cmd.c_str());
  cout << "Command status: " << r << endl;
  send("Command status: " + to_string(r) + ".");
}

