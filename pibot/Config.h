#pragma once

#include <string>
#include <unordered_set>
#include <chrono>

class Config {
public:
  Config()
    //we can initialize config with hardcoded values for debug purposes
      //: token("12345:zxcvbn")
  { }

  // constructor that accepts string argument to simplify things
  Config(const std::string&) : Config() { }

public:
  std::string token;
  int64_t send_only_to_chat_id = 0;
  bool set_my_commands = false;
  std::unordered_set<int64_t> allowed_user_ids;
  std::chrono::seconds sleep_interval = std::chrono::seconds{10};

}; //class Config


class YamlConfig : public Config {
public:
  YamlConfig() = delete;
  YamlConfig(const std::string& config_file) {
    load(config_file);
  }

private:
  void load(const std::string& config_file);

}; //class YamlConfig

