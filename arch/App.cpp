#include "App.h"

#include "Controller.h"
#include "Poller.h"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <exception>

using namespace std;
using namespace TgBot;


App* app_global_instance;

App::App(const std::string& config_file)
  : config_(config_file),
  routes_(this),
#ifdef HAVE_CURL
  curlHttpClient_(),
  bot_(config_.token, curlHttpClient_)
#else
  bot_(config_.token)
#endif
{
  //clog << "in App constructor\n";
  assert(app_global_instance == nullptr);
  app_global_instance = this;

  //unknown commands call reply_error()
  bot_.getEvents().onUnknownCommand([this](Message::Ptr message) {
    auto c = build_controller(message);
    //access check
    if ( c->is_user_allowed() ) {
      cout << "Unknown command: " << message->text << endl;
      c->reply_error();
    } else {
      //cout << "Access denied for unknown command: " << message->text << endl;
    }
  });

  //startup();
}

//void App::startup() {
//  clog << "in App::startup()\n";
//} //startup()

int App::start() {
  clog << "in App::start()\n";

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
} //start()


//void App::dispatch(std::shared_ptr<Controller> c) {
  //c->reply_error();
  //static_cast<StartController*>(c.get())->start();
//}

//void App::handler(Message::Ptr message) {
  //build default controller and call dispatch
  //auto c = make_shared<Controller>(std::move(message));
  //auto c = make_shared<StartController>(std::move(message));
  //dispatch(std::move(c));
//}

