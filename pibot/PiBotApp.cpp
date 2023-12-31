#include "PiBotApp.h"

#include "Poller.h"

#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <map>

using namespace std;
using namespace TgBot;


PiBotApp::PiBotApp(const std::string& config_file)
  : config_(config_file),
#ifdef HAVE_CURL
  curlHttpClient_(),
  bot_(config_.token, curlHttpClient_),
#else
  bot_(config_.token),
#endif
  controller_(this)
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
    auto c = controller(message);
    //access check
    if ( c->user_allowed_internal_() ) {
      cout << "Unknown command: " << message->text << endl;
      c->reply_error();
    } else {
      //cout << "Access denied for unknown command: " << message->text << endl;
    }
  });

  //commands
  evt.onCommand("start", [this](Message::Ptr message) {
    auto c = controller(message);
    if ( !c->user_allowed() ) return;

    c->send("Hi, start!");
    cout << "Chat id " << message->chat->id << endl;
    cout << "From id " << message->from->id << ", username: " << message->from->username << endl;
    cout << "Msg text " << message->text << endl;
    c->finish();
  });

  evt.onCommand("reboot", std::bind(&Controller::cmd_reboot, &controller_, _1));
  evt.onCommand("rebootcheck", std::bind(&Controller::cmd_rebootcheck, &controller_, _1));
  evt.onCommand("status", std::bind(&Controller::cmd_status, &controller_, _1));
  evt.onCommand("restart", std::bind(&Controller::cmd_restart, &controller_, _1));
  evt.onCommand("start", std::bind(&Controller::cmd_start, &controller_, _1));
  evt.onCommand("stop", std::bind(&Controller::cmd_stop, &controller_, _1));
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
  } catch (const TgException& e) {
    cerr << "TgException: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
  } catch (const std::runtime_error& e) {
    cerr << "runtime_error: " << e.what() << endl;
  } catch (const std::exception& e) {
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

