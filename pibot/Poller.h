#pragma once

#include "tgbot/Api.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class BotApp;
class Controller;

namespace TgBot {
class EventHandler;
}

class Poller {
public:
  Poller(BotApp* app,
    std::int32_t limit = 100,
    std::int32_t long_poll_timeout = 20,
    std::shared_ptr<std::vector<std::string>> allowedUpdates = nullptr);

  //bool can_sleep = run();
  //return true when we can sleep for a long time
  bool run();

private:
  void set_http_client_timeout(std::int32_t timeout);
  void do_long_polling_cycle() {
    next_timeout_ = long_poll_timeout_;
    next_count_ = 0;
    set_http_client_timeout(long_poll_timeout_ + 5);
  }
  void do_momentary_polling() {
    next_timeout_ = 0;
    next_count_ = 0;
    set_http_client_timeout(5);
  }

private:
  const TgBot::Api* api_;
  const TgBot::EventHandler* eventHandler_;
  Controller* controller_;
  std::int32_t limit_;
  std::int32_t long_poll_timeout_;
  std::int32_t lastUpdateId_ = 0;
  std::int32_t next_timeout_;
  std::int32_t next_count_;
  std::shared_ptr<std::vector<std::string>> allowedUpdates_;

}; //class Poller

