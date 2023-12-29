#include "PiBotApp.h"

#include "command.h"

#include <cstdlib>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <vector>
#include <map>

using namespace std;
using namespace TgBot;


void PiBotApp::set_my_commands() {
  vector<BotCommand::Ptr> cmds;

  for (auto&& [c, desc] : map<string, string>{{"start", "starts an app"}}) {
    BotCommand::Ptr cmd(new BotCommand);
    cmd->command = c;
    cmd->description = desc;
    cmds.push_back(cmd);
    //clog << "command: " << c << ", desc: " << desc << endl;
  }
  bot_.getApi().setMyCommands(cmds);
}


bool PiBotApp::user_allowed(const Message::Ptr& message) {
  if (user_allowed_internal_(message)) {
    cout << "Processing command: " << message->text << endl;
    return true;
  } else {
    cout << "Access denied for command: " << message->text << endl;
  }
  return false;
}


void PiBotApp::send(const Message::Ptr& message, const string& res) {
  int64_t to = config()->send_only_to_chat_id;
  bot_.getApi().sendMessage(to == 0 ? message->chat->id : to, res);
}

void PiBotApp::reply(const Message::Ptr& message, const string& res) {
  int64_t to = config()->send_only_to_chat_id;
  bot_.getApi().sendMessage(to == 0 ? message->chat->id : to, res, false, message->messageId);
}

void PiBotApp::reply_error(const Message::Ptr& message) {
  reply(message, "error");
}


string PiBotApp::param(const Message::Ptr& message, unsigned int idx) {
  const string& text = message->text;
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


void PiBotApp::run_with_output(const Message::Ptr& message, const string& cmd) {
  cout << "Running command: " << cmd << endl;
  raymii::CommandResult r = raymii::Command::exec(cmd);
  //cout << r << endl;
  cout << "Command status: " << r.exitstatus << endl;
  if (r.exitstatus == 0)
    send(message, r.output);
  else
    send(message, "Command execution error (status: " + to_string(r.exitstatus) + ").");
}

void PiBotApp::run_without_output(const Message::Ptr& message, const string& cmd) {
  cout << "Running command: " << cmd << endl;
  int r = system(cmd.c_str());
  cout << "Command status: " << r << endl;
  send(message, "Command status: " + to_string(r) + ".");
}

