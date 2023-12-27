#include "SBotApp.h"

#include "StartController.h"

//#include <iostream>
#include <memory>

using namespace std;
using namespace TgBot;


void SBotApp::startup() {
  clog << "in SBotApp::startup()\n";

  register_controller<StartController>("start");

  auto r = routes();
  r->cmd("start", "starts an app")->to([this](Message::Ptr message) {
    auto c = build_controller("start", std::move(message));
    static_pointer_cast<StartController>(c)->start();
  });

  /*bot_.getEvents().onAnyMessage([&bot = bot_](Message::Ptr message) {
    printf("User wrote %s\n", message->text.c_str());
    if (StringTools::startsWith(message->text, "/start")) {
      return;
    }
    bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
  });
  */

} //startup()


SBotApp::SBotApp(const std::string& config_file)
  : App(config_file)
{
  //clog << "in SBotApp constructor\n";
  startup();
}

