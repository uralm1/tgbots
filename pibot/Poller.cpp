#include "Poller.h"

#include "BotApp.h"

#include "tgbot/Bot.h"
#include "tgbot/EventHandler.h"

#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <utility>

constexpr bool POLLER_DEBUG = false;

#define LONG_POLL_MAXCOUNT 10

Poller::Poller(BotApp* app,
  std::int32_t limit,
  std::int32_t long_poll_timeout,
  std::shared_ptr<std::vector<std::string>> allowedUpdates)
  : api_(&app->bot()->getApi()),
    eventHandler_(&app->bot()->getEventHandler()),
    controller_(&app->controller_),
    limit_(limit),
    long_poll_timeout_(long_poll_timeout),
    allowedUpdates_(std::move(allowedUpdates)) {

  do_long_polling_cycle();
}

bool Poller::run() {
  if constexpr (POLLER_DEBUG)
    std::clog << (next_timeout_ > 0 ? "Long polling" : "Momentary") << " getUpdates()\n";

  //std::vector<Update::Ptr> updates
  auto updates = api_->getUpdates(lastUpdateId_, limit_, next_timeout_, allowedUpdates_);
  //std::clog << "after getUpdates()\n";

  if (updates.empty()) {
    if (next_timeout_ > 0) {
      ++next_count_;
      if (next_count_ >= LONG_POLL_MAXCOUNT) {
        do_momentary_polling();
      }
    }

    //queue processing
    //std::clog << "Queue size: " << controller_->command_queue_.size() << "\n";
    if (controller_->has_commands_in_queue()) {
      if constexpr (POLLER_DEBUG)
        std::clog << "Processing command queue\n";
      //process ALL queue
      controller_->execute_queued_commands();
      do_long_polling_cycle();
      return false;
    }
  } else {
    // handle updates
    for (auto& item : updates) {
      if (item->updateId >= lastUpdateId_) {
        lastUpdateId_ = item->updateId + 1;
      }
      eventHandler_->handleUpdate(item);
    }

    if (controller_->has_commands_in_queue())
      do_momentary_polling();
    else
      do_long_polling_cycle();

    return false;
  }

  //std::printf("lastUpdateId: %d\n", lastUpdateId_);
  //std::printf("next_timeout: %d\n", next_timeout_);
  //std::printf("next_count: %d\n", next_count_);

  if (next_timeout_ == 0/* && !controller_->has_commands_in_queue()*/) {
    if constexpr (POLLER_DEBUG)
      std::clog << "NO TRAFFIC SLEEP\n";
    return true;
  }
  return false;
}

void Poller::set_http_client_timeout(std::int32_t timeout) {
  const_cast<TgBot::HttpClient&>(api_->_httpClient)._timeout = timeout;
}

