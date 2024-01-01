#include "Config.h"

//#include "yaml-cpp/node/detail/iterator_fwd.h"
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

    const auto& aui = config["AllowedUserIds"];
    if (aui.IsDefined()) {
      if (aui.IsSequence()) {
        allowed_user_ids.reserve(aui.size());
        for (YAML::const_iterator it = aui.begin(); it != aui.end(); ++it) {
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

    const auto& smk = config["SetMyCommands"];
    if (smk.IsDefined()) {
      if (smk.IsMap()) {
        for (YAML::const_iterator it = smk.begin(); it != smk.end(); ++it) {
          TgBot::BotCommand::Ptr cmd(new TgBot::BotCommand);
          cmd->command = it->first.as<std::string>();
          cmd->description = it->second.as<std::string>();
          set_my_commands.push_back(cmd);
          //clog << "command: " << cmd->command << ", desc: " << cmd->description << endl;
        }
      } else {
        throw std::runtime_error("SetMyCommands must be a Map.");
      }
    }

  } catch(const std::runtime_error& e) {
    cerr << "Config error: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  assert(!token.empty());
}

