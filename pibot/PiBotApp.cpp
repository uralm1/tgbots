#include "PiBotApp.h"

#include "Poller.h"
#include "command.h"

#include <iostream>
#include <vector>
#include <map>
#include <functional>

using namespace std;
using namespace TgBot;


PiBotApp::PiBotApp(const std::string& config_file)
  : config_(config_file),
#ifdef HAVE_CURL
  curlHttpClient_(),
  bot_(config_.token, curlHttpClient_)
#else
  bot_(config_.token)
#endif
{
  //clog << "in PiBotApp constructor\n";
  startup();
}

void PiBotApp::startup() {
  //clog << "in startup()\n";

  using namespace std::placeholders;

  auto& evt = bot_.getEvents();

  //unknown commands call reply_error()
  evt.onUnknownCommand([this](Message::Ptr message) {
    //access check
    if ( user_allowed_internal_(message) ) {
      cout << "Unknown command: " << message->text << endl;
      reply_error(message);
    } else {
      //cout << "Access denied for unknown command: " << message->text << endl;
    }
  });

  //commands
  evt.onCommand("start", [this](Message::Ptr message) {
    if ( !user_allowed(message) ) return;

    send(message, "Hi, start!");
    cout << "Chat id " << message->chat->id << endl;
    cout << "From id " << message->from->id << ", username: " << message->from->username << endl;
    cout << "Msg text " << message->text << endl;
  });

  evt.onCommand("reboot", std::bind(&PiBotApp::cmd_reboot, this, _1));
  evt.onCommand("rebootcheck", std::bind(&PiBotApp::cmd_rebootcheck, this, _1));
  evt.onCommand("status", std::bind(&PiBotApp::cmd_status, this, _1));
  evt.onCommand("restart", std::bind(&PiBotApp::cmd_restart, this, _1));
  evt.onCommand("start", std::bind(&PiBotApp::cmd_start, this, _1));
  evt.onCommand("stop", std::bind(&PiBotApp::cmd_stop, this, _1));
}

int PiBotApp::start() {
  //clog << "in start()\n";

  signal(SIGINT, [](int s) {
    cout << "\nSIGINT got\n";
    exit(EXIT_SUCCESS);
  });

  cout << "BotId from token: " << config()->token.substr(0, config()->token.find_first_of(':')) << endl;
  cout << "No traffic sleep interval: " << config()->sleep_interval.count() << " seconds" << endl;
  if (config()->send_only_to_chat_id != 0)
    cout << "Sending ONLY to ChatId: " << config()->send_only_to_chat_id << endl;

  try {
    bot_.getApi().deleteWebhook();

    cout << "Bot username: " << bot_.getApi().getMe()->username << endl;

    if (config_.set_my_commands) {
      cout << "Updating bot command list.\n";
      set_my_commands();
    }

    cout << "Running...\n";

    //libcurl has its 25s timeout on connection
    Poller poller(bot_, 100, 20, config_.sleep_interval);
    while (true) {
      poller.run();
    }
  } catch (exception& e) {
    cerr << "error: " << e.what() << endl;
  }

  return 0;
}


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

