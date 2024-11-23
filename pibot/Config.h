#pragma once

#include <tgbot/tgbot.h>

#include <string>
#include <string_view>
#include <unordered_set>
#include <chrono>
#include <map>
#include <vector>

class Config {
public:
  Config()
    //we can initialize config with hardcoded values for debug purposes
      //: token("12345:zxcvbn")
  { }

  // constructor that accepts string argument to simplify things
  Config(std::string_view) : Config() { }

public:
  std::string token;
  int64_t send_only_to_chat_id = 0;
  bool delete_webhook_on_start = false;
  std::unordered_set<int64_t> allowed_user_ids;
  std::chrono::seconds sleep_interval = std::chrono::seconds{10};

  bool set_my_commands = false;
  std::vector<TgBot::BotCommand::Ptr> my_commands;

  std::vector<std::string> cam_files;

  struct CommandParam;
  using commands_map = std::map<std::string, CommandParam>;
  struct CommandParam {
    std::string pre_send;
    std::string post_send;
    std::string run_without_output;
    std::string run_with_output;
    commands_map subcommands;
  };
  commands_map commands;

}; //class Config


namespace YAML { class Node; }

class YamlConfig : public Config {
public:
  YamlConfig() = delete;
  YamlConfig(std::string_view config_file) {
    load(config_file);
  }

private:
  void load(std::string_view config_file);
  void parse_commands(const YAML::Node& node, commands_map& to_map, int lvl = 1);

}; //class YamlConfig

