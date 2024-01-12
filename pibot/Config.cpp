#include "Config.h"

#include "yaml-cpp/yaml.h"

#include <stdexcept>
#include <iostream>
#include <cassert>

using namespace std;

void YamlConfig::load(const std::string& config_file) {
  try {
    YAML::Node config = YAML::LoadFile(config_file);

    token = config["Token"].as<string>();
    send_only_to_chat_id = config["SendOnlyToChatId"].as<int64_t>(0);
    delete_webhook_on_start = config["DeleteWebHookOnStart"].as<bool>(false);

    const auto& aui_node = config["AllowedUserIds"];
    if (aui_node.IsDefined()) {
      if (aui_node.IsSequence()) {
        allowed_user_ids.reserve(aui_node.size());
        for (/*YAML::const_iterator*/auto it = aui_node.begin(); it != aui_node.end(); ++it) {
          //clog << "VAL: " << it->as<int64_t>() << endl;
          allowed_user_ids.insert(it->as<int64_t>());
        }
      } else {
        throw std::runtime_error("AllowedUsersIds must be a Sequence.");
      }
    } else {
      cerr << "Warning: AllowedUserIds is not defined, bot will accept commands from Everyone!\n";
    }

    sleep_interval = std::chrono::seconds(config["SleepInterval"].as<unsigned long>(10));

    //set_my_commands default false
    const auto& smk_node = config["SetMyCommands"];
    if (smk_node.IsDefined()) {
      if (smk_node.IsMap()) {
        for (auto it = smk_node.begin(); it != smk_node.end(); ++it) {
          TgBot::BotCommand::Ptr cmd(new TgBot::BotCommand);
          cmd->command = it->first.as<string>();
          cmd->description = it->second.as<string>();
          my_commands.push_back(cmd);
          //clog << "command: " << cmd->command << ", desc: " << cmd->description << endl;
        }
        set_my_commands = true;
      } else if (smk_node.IsScalar() && smk_node.as<string>(string{}) == "clear") {
        //we should reset SetMyCommands for bot
        my_commands.clear();
        set_my_commands = true;
      } else {
        throw std::runtime_error("SetMyCommands must be a Map or Scalar=clear.");
      }
    }

    const auto& camf_node = config["CamFiles"];
    if (camf_node.IsDefined()) {
      if (camf_node.IsSequence()) {
        cam_files.reserve(camf_node.size());
        for (auto it = camf_node.begin(); it != camf_node.end(); ++it) {
          cam_files.push_back(it->as<string>());
        }
      } else {
        throw std::runtime_error("CamFiles must be a Sequence.");
      }
    }

    parse_commands(config["Commands"], commands);

  } catch(const std::runtime_error& e) {
    cerr << "Config error: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  assert(!token.empty());
}


void YamlConfig::parse_commands(const YAML::Node& node, commands_map& to_map, int lvl) {
  const string _sub{ lvl > 1 ? "Sub" : "" };
  if (node.IsDefined()) {
    if (node.IsMap()) {
      for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        auto cmd = it->first.as<string>();
        const auto& cmd_node = node[cmd];
        //clog << _sub + "Command: " << cmd << endl;
        if (cmd_node.IsMap()) {
          auto pre_send = cmd_node["PreSend"].as<string>(string{});
          auto post_send = cmd_node["PostSend"].as<string>(string{});
          auto run_without_output = cmd_node["RunWithoutOutput"].as<string>(string{});
          auto run_with_output = cmd_node["RunWithOutput"].as<string>(string{});
          //clog << "pre_send: " << pre_send << ", post_send: " << post_send << ", run_without_output: " << run_without_output << ", run_with_output: " << run_with_output << endl;
          CommandParam _c;
          _c.pre_send = pre_send;
          _c.post_send = post_send;
          _c.run_without_output = run_without_output;
          _c.run_with_output = run_with_output;

          if (lvl < 2) {
            const auto& subcmd_node = cmd_node["SubCommands"];
            parse_commands(subcmd_node, _c.subcommands, lvl + 1);
          }

          to_map.emplace(cmd, _c);

        } else {
          throw std::runtime_error(_sub + "Command " + cmd + " must be a Map.");
        }
      }
    } else {
      throw std::runtime_error(_sub + "Commands must be a Map.");
    }
  } else {
    if (lvl < 2)
      cerr << "Warning: Commands are not defined. Probably bot will be unusable." << endl;
  }
}

