#pragma once

#include "tgbot/Api.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace TgBot {

class Bot;
class EventHandler;

class Poller {

public:
  Poller(const Api* api, const EventHandler* eventHandler,
    std::int32_t limit,
    std::int32_t long_poll_timeout,
    std::chrono::seconds sleep_timeout,
    std::shared_ptr<std::vector<std::string>> allowUpdates);

  Poller(const Bot& bot,
    std::int32_t limit = 100,
    std::int32_t long_poll_timeout = 20,
    std::chrono::seconds sleep_timeout = std::chrono::seconds(600),
    const std::shared_ptr<std::vector<std::string>>& allowUpdates = nullptr);

  void run();

private:
  void set_http_client_timeout(std::int32_t timeout);

private:
  const Api* api_;
  const EventHandler* eventHandler_;
  std::int32_t limit_;
  std::int32_t long_poll_timeout_;
  std::chrono::seconds sleep_timeout_;
  std::int32_t lastUpdateId_ = 0;
  std::int32_t next_timeout_ = 0;
  std::int32_t next_count_ = 0;
  std::shared_ptr<std::vector<std::string>> allowUpdates_;

}; //class Poller

} //namespace TgBot

