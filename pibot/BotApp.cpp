#include "BotApp.h"

#include "Poller.h"

#include <string>
#include <iostream>
#include <functional>

using namespace std;
using namespace TgBot;


BotApp::BotApp(const std::string& config_file)
  : config_(config_file),
#ifdef HAVE_CURL
  curlHttpClient_(),
  bot_(config_.token, curlHttpClient_),
#else
  bot_(config_.token),
#endif
  controller_(this)
{
  //clog << "in BotApp constructor\n";
  startup();
}

void BotApp::startup() {
  //clog << "in startup()\n";

  using namespace std::placeholders;

  auto& evt = bot_.getEvents();

  //unknown commands call reply_error()
  evt.onUnknownCommand([this](Message::Ptr message) {
    auto c = controller(message);
    //access check
    if ( c->user_allowed_internal() ) {
      cout << "Unknown command: " << message->text << endl;
      c->reply_error();
    } else {
      //cout << "Access denied for unknown command: " << message->text << endl;
    }
  });

  //commands
#if 0
  //this is playground command
  evt.onCommand("test", [this](Message::Ptr message) {
    auto c = controller(message);
    if ( !c->check_access() ) return;

    c->send("H_i_, <s>t#[]{}a(r)t!?^$;|@% ```\nMonospace ! test```aaa`nomonospace test`bbb.,");
    cout << "Chat id " << message->chat->id << endl;
    cout << "From id " << message->from->id << ", username: " << message->from->username << endl;
    cout << "Msg text " << message->text << endl;
    c->finish();
  });
#endif

  for (const auto& [cmd, params] : config_.commands)
    evt.onCommand(cmd, std::bind(&Controller::cmd_handler, &controller_, params, _1));

  evt.onCommand("help", std::bind(&Controller::cmd_help, &controller_, _1));
  evt.onCommand("cam", std::bind(&Controller::cmd_cam, &controller_, _1));

  //evt.onCommand("additional_command", std::bind(&Controller::cmd_additional, &controller_, _1));
}

int BotApp::start() {
  //clog << "in start()\n";

  signal(SIGINT, [](int s) {
    cout << "\nSIGINT got\n";
    exit(EXIT_SUCCESS);
  });

  cout << "BotId from token: " << config()->token.substr(0, config()->token.find_first_of(':')) << endl;
  cout << "No traffic sleep interval: " << config()->sleep_interval.count() << " seconds" << endl;
  if (config()->send_only_to_chat_id != 0)
    cout << "Sending ONLY to ChatId: " << config()->send_only_to_chat_id << endl;

  cout << "Serving commands: [";
  for (auto it = config_.commands.begin(); it != config_.commands.end(); ) {
    cout << it->first;
    if (++it != config_.commands.end()) cout << ", ";
  }
  cout << "]\n";

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


void BotApp::set_my_commands() {
  bot_.getApi().setMyCommands(config_.my_commands);
}

