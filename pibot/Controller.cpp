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


string Controller::md_escape(const string_view s) {
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


void Controller::_send(const TgBot::Api* api, const int64_t to, const string_view resp) {
  auto lpo = make_shared<LinkPreviewOptions>();
  lpo->isDisabled = true;
  try {
    api->sendMessage(to, /*chatId*/
        resp.empty() ? "empty" : md_escape(resp), /*text*/
        lpo, /*linkPreviewOptions*/
        nullptr, /*replyParameters*/
        nullptr, /*replyMarkup*/
        "MarkdownV2" /*parseMode*/);
  } catch (const TgException& e) {
    cerr << "TgException in send: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
  }
}

void Controller::reply(const string_view resp) const {
  assert(message_);

  int64_t to = config()->send_only_to_chat_id;
  auto lpo = make_shared<LinkPreviewOptions>();
  lpo->isDisabled = true;
  auto rpar = make_shared<ReplyParameters>();
  rpar->messageId = message_->messageId;
  rpar->allowSendingWithoutReply = true;
  try {
    bot_->getApi().sendMessage(to == 0 ? message_->chat->id : to, /*chatId*/
        resp.empty() ? "empty" : md_escape(resp), /*text*/
        lpo, /*linkPreviewOptions*/
        rpar, /*replyParameters*/
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


void Controller::Command::run_with_output(const TgBot::Api* api) const {
  assert(!run_with_output_.empty());
  cout << "Executing: " << run_with_output_ << endl;
  raymii::CommandResult r = raymii::Command::exec(string{run_with_output_});
  //cout << r << endl;
  cout << "Command status: " << r.exitstatus << endl;

  if (r.output.empty())
    send(api, "Result is empty.");
  else
    send(api, "```\n" + r.output + "```");

  if (r.exitstatus != 0)
    send(api, "Command status: " + to_string(r.exitstatus) + ".");

}

void Controller::Command::run_without_output(const TgBot::Api* api) const {
  assert(!run_without_output_.empty());
  cout << "Executing: " << run_without_output_ << endl;
  int r = system(string{run_without_output_}.c_str());
  cout << "Command status: " << r << endl;
  send(api, "Command status: " + to_string(r) + ".");
}

