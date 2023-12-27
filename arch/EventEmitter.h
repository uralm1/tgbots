#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <any>
#include <string>
#include <string_view>
#include <iostream>
#include <stdexcept>

constexpr bool EVENTEMITTER_DEBUG = false;

template<typename Id = std::string_view>
class EventEmitter {
  using event_id = Id;
  using event_cb = std::function<void(std::any)>;
  using event_list = std::vector<event_cb>;
  using events_map = std::unordered_map<event_id, event_list>;

  class _once_wrapper {
  public:
    inline void operator()(std::any param) const {
      this_->unsubscribe(event_, wrapper_pos_);
      cb_(std::move(param));
    }
    _once_wrapper(EventEmitter* _this, const event_id& event, event_cb cb)
    : this_{_this},
      event_{event},
      cb_{std::move(cb)},
      wrapper_pos_{nullptr}
    {}
    void set_wrapper_pos(event_list::iterator wrapper_pos) { wrapper_pos_ = wrapper_pos; }
  private:
    EventEmitter* this_;
    const event_id event_;
    event_cb cb_;
    event_list::iterator wrapper_pos_;
  }; //class _once_wrapper

public:
  EventEmitter* emit(const event_id& event, std::any param = std::any());

  bool has_subscribers(const event_id& event) {
    typename events_map::const_iterator it = events_.find(event);
    if (it != events_.end() && !it->second.empty())
      return true;
    return false;
  }

  event_list::iterator on(const event_id& event, event_cb cb) {
    auto& evl = events_[event];
    evl.push_back(std::move(cb));
    return --evl.end();
  }

  event_list::iterator once(const event_id& event, event_cb cb) {
    event_list::iterator wrapper_pos = on(event, _once_wrapper{this, event, std::move(cb)});
    wrapper_pos->target<_once_wrapper>()->set_wrapper_pos(wrapper_pos);
    return wrapper_pos;
  }

  event_list::iterator on_error(event_cb cb) {
    return on("error", std::move(cb));
  }

  event_list& subscribers(const event_id& event) {
    return events_[event];
  }

  EventEmitter* unsubscribe(const event_id& event) {
    events_[event].clear();
    return this;
  }

  EventEmitter* unsubscribe(const event_id& event, const event_list::iterator& cb_it) {
    events_[event].erase(cb_it);
    return this;
  }

private:
  bool emit_internal_(const event_id& event, std::any param) {
    typename events_map::const_iterator it = events_.find(event);
    if (it != events_.end() && !it->second.empty()) {
      if constexpr (EVENTEMITTER_DEBUG)
        std::clog << "Emit " << event << " event in (" << it->second.size() << ") observers\n";
      for(const auto& ev : it->second) ev(param);
      return true;
    } else {
      if constexpr (EVENTEMITTER_DEBUG)
        std::clog << "Emit " << event << " event in (0) observers\n";
    }
    return false;
  }

  void emit_error_(const event_id& event, std::any param) {
    if (event == "error") {
      std::string p;
      try { p = std::any_cast<const char*>(param);
      } catch(const std::bad_any_cast&) {
        try { p = std::any_cast<const std::string_view>(param);
        } catch(const std::bad_any_cast&) {
          try { p = std::any_cast<const std::string>(param);
          } catch(const std::bad_any_cast&) { }
        }
      }
      throw std::runtime_error("Uncatched error event: " + p);
    }
  }

private:
  events_map events_;
}; //class EventEmitter


template<typename Id>
EventEmitter<Id>* EventEmitter<Id>::emit(const event_id& event, std::any param) {
  emit_internal_(event, std::move(param));
  return this;
}

template<>
inline EventEmitter<std::string>* EventEmitter<std::string>::emit(const std::string& event, std::any param) {
  if (!emit_internal_(event, param)) emit_error_(event, param);
  return this;
}

template<>
inline EventEmitter<std::string_view>* EventEmitter<std::string_view>::emit(const std::string_view& event, std::any param) {
  if (!emit_internal_(event, param)) emit_error_(event, param);
  return this;
}

