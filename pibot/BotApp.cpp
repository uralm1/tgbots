#include "BotApp.h"

#include "Poller.h"

#include "utf8.h"

#include <stdexcept>
#include <string>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <thread>

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

    //c->send("H_i_, <s>t#[]{}a(r)t!?^$;|@% ```\nMonospace ! test```aaa`nomonospace test`bbb.,");
    cout << "Chat id " << message->chat->id << endl;
    cout << "From id " << message->from->id << ", username: " << message->from->username << endl;
    cout << "Msg text " << message->text << ", size " << message->text.size() << endl;

    string s(message->text);
    for (auto it = s.begin(); it != s.end(); ++it) cout << *it << ",";
    cout << endl;

    u32string s32 = utf8::utf8to32(s);
    cout << "Msg32 size " << s32.size() << endl;
    for (auto it = s32.begin(); it != s32.end(); ++it) cout << *it << ",";
    cout << endl;

    utf8::iterator it_end(s.end(), s.begin(), s.end());
    for (utf8::iterator it8(s.begin(), s.begin(), s.end()); it8 != it_end; ++it8) cout << *it8 << ",";
    cout << endl;

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

  cout << "Yml commands: [";
  for (auto it = config_.commands.begin(); it != config_.commands.end(); ) {
    cout << it->first;
    if (++it != config_.commands.end()) cout << ", ";
  }
  cout << "]\n";

  try {
    try {
      if (config_.delete_webhook_on_start) {
        cout << "Deleting webhook as requested.\n";
        bot_.getApi().deleteWebhook();
      }
    } catch (const std::runtime_error& e) {
      cerr << "(ignored) failed: " << e.what() << endl;
    }

    unsigned attempt = 1;
    while (attempt <= 5) {
      try {
        string username = bot_.getApi().getMe()->username;
        cout << "Bot username: " << username << endl;
        break;
      } catch (const std::runtime_error& e) {
        cerr << "(attempt " << attempt++ << ") Getting bot username failed: " << e.what() << endl;
        this_thread::sleep_for(4s);
      }
    }

    try {
      if (config_.set_my_commands) {
        cout << "Updating bot command list.\n";
        set_my_commands();
      }
    } catch (const std::runtime_error& e) {
      cerr << "(ignored) failed: " << e.what() << endl;
    }

    cout << "Running...\n";

    //libcurl has its 25s timeout on connection
    Poller poller(bot_, 100, 20, config_.sleep_interval);
    while (true) {
      poller.run();
    }
  } catch (const TgException& e) {
    cerr << "TgException: " << e.what() << " (" << (int)e.errorCode << ")" << endl;
    return EXIT_FAILURE;
  } catch (const std::runtime_error& e) {
    cerr << "runtime_error: " << e.what() << endl;
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    cerr << "error: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


void BotApp::set_my_commands() {
  bot_.getApi().setMyCommands(config_.my_commands);
}

