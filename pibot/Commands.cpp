#include "Controller.h"

#include <stdexcept>
#include <string>
#include <iostream>

using namespace std;
using namespace TgBot;


void Controller::cmd_handler(const Config::CommandParam& cmd_param, Message::Ptr message) {
  if ( !init_and_check_access(message) ) return;

  cout << "Processing command: " << message_->text << endl; //command with "/"

  string service = param2();
  if (service.empty()) {
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
  if ( !init_and_check_access(message) ) return;

  cout << "Processing command: " << message_->text << endl;

  string resp;

  for (auto it = config()->commands.begin(); it != config()->commands.end(); ++it) {
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
  send("Help:\n" + resp + " /cam \\[N\\]\n");

  finish();
}


void Controller::cmd_cam(Message::Ptr message) {
  if ( !init_and_check_access(message) ) return;

  cout << "Processing command: " << message_->text << endl;

  string file_name;
  try {
    string sp2 = param2();
    size_t index = sp2.empty() ? 1 : std::stoul(sp2);
    if (index == 0) throw std::out_of_range("0");
    file_name = config()->cam_files.at(index - 1);
  }
  catch (std::logic_error&) {
    send("Bad or not-existed cam.");
    finish();
    return;
  }

  if (FILE* test = fopen(file_name.c_str(), "r"))
    fclose(test);
  else {
    send("Preview file is not exist.");
    finish();
    return;
  }

  int64_t to = config()->send_only_to_chat_id;
  try {
    bot_->getApi().sendPhoto(to == 0 ? message_->chat->id : to, /*chatId*/
        InputFile::fromFile(file_name, "image/jpeg"), /*photo*/
        file_name /*caption*/);
  } catch (const TgException& e) {
    cerr << "TgException in sendPhoto: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
    send("sendPhoto failed.");
  }

  finish();
}

