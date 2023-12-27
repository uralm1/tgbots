#include "Poller.h"

#include "tgbot/Bot.h"
#include "tgbot/EventHandler.h"

#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <utility>
#include <thread>

namespace TgBot {

#define LONG_POLL_MAXCOUNT 10

Poller::Poller(const Api* api, const EventHandler* eventHandler,
  std::int32_t limit,
  std::int32_t long_poll_timeout,
  std::chrono::seconds sleep_timeout,
  std::shared_ptr<std::vector<std::string>> allowUpdates)
  : api_(api), eventHandler_(eventHandler),
    limit_(limit),
    long_poll_timeout_(long_poll_timeout),
    sleep_timeout_(sleep_timeout),
    allowUpdates_(std::move(allowUpdates)) {

  set_http_client_timeout(5);
}

Poller::Poller(const Bot& bot,
  std::int32_t limit,
  std::int32_t long_poll_timeout,
  std::chrono::seconds sleep_timeout,
  const std::shared_ptr<std::vector<std::string>>& allowUpdates)
  : Poller(&bot.getApi(), &bot.getEventHandler(), limit, long_poll_timeout, sleep_timeout, allowUpdates) {
}

void Poller::run() {
  std::clog << (next_timeout_ > 0 ? "Long polling" : "Momentary") << " getUpdates()\n";
  //std::vector<Update::Ptr> updates
  auto updates = api_->getUpdates(lastUpdateId_, limit_, next_timeout_, allowUpdates_);
  //std::clog << "after getUpdates()\n";

  if (updates.empty()) {
    if (next_timeout_ > 0) {
      ++next_count_;
      if (next_count_ > LONG_POLL_MAXCOUNT) {
        next_timeout_ = 0;
        next_count_ = 0;
        set_http_client_timeout(5);
      }
    }
  } else {
    // handle updates
    for (auto& item : updates) {
      if (item->updateId >= lastUpdateId_) {
        lastUpdateId_ = item->updateId + 1;
      }
      eventHandler_->handleUpdate(item);
    }
    next_timeout_ = long_poll_timeout_;
    next_count_ = 0;
    set_http_client_timeout(long_poll_timeout_ + 5);
  }

  //std::printf("lastUpdateId: %d\n", lastUpdateId_);
  //std::printf("next_timeout: %d\n", next_timeout_);
  //std::printf("next_count: %d\n", next_count_);

  if (next_timeout_ == 0) {
    std::clog << "NO TRAFFIC SLEEP\n";
    std::this_thread::sleep_for(sleep_timeout_);
  }
}

void Poller::set_http_client_timeout(std::int32_t timeout) {
  const_cast<TgBot::HttpClient&>(api_->_httpClient)._timeout = timeout;
}

} //namespace TgBot
