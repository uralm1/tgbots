#include "PiBotApp.h"

#include "command.h"

#include <cstdlib>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

using namespace std;
using namespace TgBot;


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

