#include "Controller.h"
#include "BotApp.h"

#include <string>
#include <iostream>

using namespace std;
using namespace TgBot;


void Controller::cmd_handler(const Config::CommandParam& cmd_param, Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  cout << "Processing command: " << message_->text << endl; //command with "/"

  string service = param2();
  if (service == "") {
    //use cmd_param fields to execute command
    execute_cmd(cmd_param);

  } else {
    //subcommand?
    if (auto sc = cmd_param.subcommands.find(service); sc != cmd_param.subcommands.end()) {
      const Config::CommandParam& subcmd_param = sc->second;
      //use subcmd_param fields to execute subcommand
      execute_cmd(subcmd_param);

    } else {
      //not found
      send("Unknown subcommand.");
    }
  }
  finish();
}


void Controller::execute_cmd(const Config::CommandParam& cmd_param) {
  if (!cmd_param.pre_send.empty())
    send(cmd_param.pre_send);

  if (!cmd_param.run_with_output.empty())
    run_with_output(cmd_param.run_with_output);

  if (!cmd_param.run_without_output.empty())
    run_without_output(cmd_param.run_without_output);

  if (!cmd_param.post_send.empty())
    send(cmd_param.post_send);
}


void Controller::cmd_help(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  cout << "Processing command: " << message_->text << endl;

  string resp;
  Config* cf = config();

  for (auto it = cf->commands.begin(); it != cf->commands.end(); ++it) {
    resp += " /" + it->first;
    Config::commands_map subc = it->second.subcommands;
    if (!subc.empty()) {
      resp += " \\[";
      for (auto it2 = subc.begin(); it2 != subc.end(); ) {
        resp += it2->first;
        if (++it2 != subc.end()) resp += ", ";
      }
      resp += "\\]";
    }
    resp += "\n";
  }

  send("Help:\n" + resp);

  finish();
}



