#include "Routes.h"
#include "App.h"
#include "Controller.h"

#include <cassert>
#include <vector>
#include <iostream>

using namespace std;
using namespace TgBot;


unique_ptr<Routes::Command> Routes::cmd(const string& command) {
  commands_[command] = "";
  return make_unique<Command>(app_, command);
}

unique_ptr<Routes::Command> Routes::cmd(const string& command, const string& description) {
  commands_[command] = description;
  return make_unique<Command>(app_, command);
}

bool Routes::set_my_commands() {
  vector<BotCommand::Ptr> cmds;

  for (const auto& [c, desc] : commands_) {
    BotCommand::Ptr cmd(new BotCommand);
    cmd->command = c;
    cmd->description = desc;
    cmds.push_back(cmd);
    //clog << "command: " << c << ", desc: " << desc << endl;
  }
  app_->bot()->getApi().setMyCommands(cmds);

  return !cmds.empty();
}


void Routes::Command::to(const function<void(Message::Ptr)>& action) {
  assert(command_.length() > 0);
  app_->bot()->getEvents().onCommand(command_, [app=app_, action](Message::Ptr message) {
    //FIXME optimize duplicate controller scheme
    auto c = app->build_controller(message);
    //access check
    if ( c->is_user_allowed() ) {
      cout << "Processing command: " << message->text << endl;
      action(std::move(message));
    } else {
      cout << "Access denied for command: " << message->text << endl;
    }
    c->finish();
  });
}

