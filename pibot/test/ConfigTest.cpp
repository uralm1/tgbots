#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Config.h"

#include <cstdio>
#include <memory>
#include <fstream>

using namespace std;
using namespace TgBot;

static const string testfile1 = "_test1.yml";
static const string testfile2 = "_test2.yml";

class ConfigTest: public testing::Test {
public:
  unique_ptr<YamlConfig> yc;

  static void SetUpTestSuite() {
    ofstream ofs(testfile1);
    ofs << "Token: 123456:asdfghjkl\n";
    ofs << "SendOnlyToChatId: 654321\n";
    ofs << "AllowedUserIds:\n";
    ofs << "  - 1111\n";
    ofs << "  - 2222\n";
    ofs << "SleepInterval: 123\n";
    ofs << "SetMyCommands:\n";
    ofs << "  test1: test1 description\n";
    ofs << "  test2: \"test2 description\"\n";
    ofs << "Commands:\n";
    ofs << "  command1:\n";
    ofs << "    PreSend: command1 accepted.\n";
    ofs << "    RunWithoutOutput: /bin/true\n";
    ofs << "    SubCommands:\n";
    ofs << "      subcommand1:\n";
    ofs << "        PreSend: subcommand1 accepted.\n";
    ofs << "        RunWithoutOutput: /bin/true1\n";
    ofs << "      subcommand2:\n";
    ofs << "        RunWithOutput: /bin/false1\n";
    ofs << "        PostSend: after subcommand2.\n";
    ofs << "  command2:\n";
    ofs << "    RunWithOutput: /bin/false\n";
    ofs << "    PostSend: after command2.\n";
    ofs << endl;

    ofstream ofs2(testfile2);
    ofs2 << "Token: 123456:asdfghjkl\n";
    ofs2 << "SetMyCommands: clear\n";
    ofs2 << endl;
  }

  static void TearDownTestSuite() {
    remove(testfile1.c_str());
    remove(testfile2.c_str());
  }

  void SetUp() {
    yc = make_unique<YamlConfig>(testfile1);
  }

  void TearDown() { yc.reset(); }
};


TEST_F(ConfigTest, NoYamlFileTest) {
  EXPECT_EXIT({
    YamlConfig c{"non_existed.yml"};
  }, testing::ExitedWithCode(1), "^Config error") << "exception on non existed config file";
}

TEST_F(ConfigTest, Test1) {
  EXPECT_EQ(yc->token, "123456:asdfghjkl") << "token";
  EXPECT_EQ(yc->send_only_to_chat_id, 654321) << "send_only_to_chat_id";
  EXPECT_EQ(yc->sleep_interval, 123s) << "sleep_interval";

  EXPECT_THAT(yc->allowed_user_ids, testing::SizeIs(2)) << "allowed_user_ids size";
  EXPECT_THAT(yc->allowed_user_ids, testing::UnorderedElementsAre(1111, 2222)) << "allowed_user_ids";

  EXPECT_THAT(yc->my_commands, testing::SizeIs(2)) << "my_commands size";

  EXPECT_THAT(*yc->my_commands[0], testing::FieldsAre("test1", "test1 description")) << "my_commands[0]";
  EXPECT_THAT(*yc->my_commands[1], testing::FieldsAre("test2", "test2 description")) << "my_commands[1]";
  EXPECT_TRUE(yc->set_my_commands) << "set_my_commands is true";

  EXPECT_THAT(yc->commands, testing::SizeIs(2)) << "commands size";

  EXPECT_THAT(yc->commands, testing::Contains(testing::Key("command1"))) << "has command1";
  EXPECT_THAT(yc->commands, testing::Contains(testing::Key("command2"))) << "has command2";

  Config::CommandParam c1 = yc->commands["command1"];
  EXPECT_THAT(c1, testing::Field(&Config::CommandParam::pre_send, testing::StrEq("command1 accepted."))) << "command1 pre_send";
  EXPECT_THAT(c1, testing::Field(&Config::CommandParam::run_without_output, testing::StrEq("/bin/true"))) << "command1 run_without_output";
  EXPECT_THAT(c1, testing::Field(&Config::CommandParam::run_with_output, testing::StrEq(""))) << "command1 run_with_output";
  EXPECT_THAT(c1, testing::Field(&Config::CommandParam::post_send, testing::StrEq(""))) << "command1 post_send";
  EXPECT_THAT(c1, testing::Field(&Config::CommandParam::subcommands, testing::SizeIs(2))) << "command1 has 2 subcommands";

  EXPECT_THAT(c1.subcommands, testing::Contains(testing::Key("subcommand1"))) << "has subcommand1";
  EXPECT_THAT(c1.subcommands, testing::Contains(testing::Key("subcommand2"))) << "has subcommand2";

  Config::CommandParam sc1 = c1.subcommands["subcommand1"];
  EXPECT_THAT(sc1, testing::Field(&Config::CommandParam::pre_send, testing::StrEq("subcommand1 accepted."))) << "subcommand1 pre_send";
  EXPECT_THAT(sc1, testing::Field(&Config::CommandParam::run_without_output, testing::StrEq("/bin/true1"))) << "subcommand1 run_without_output";
  EXPECT_THAT(sc1, testing::Field(&Config::CommandParam::run_with_output, testing::StrEq(""))) << "subcommand1 run_with_output";
  EXPECT_THAT(sc1, testing::Field(&Config::CommandParam::post_send, testing::StrEq(""))) << "subcommand1 post_send";
  EXPECT_THAT(sc1, testing::Field(&Config::CommandParam::subcommands, testing::IsEmpty())) << "subcommand1 no subcommands";

  Config::CommandParam sc2 = c1.subcommands["subcommand2"];
  EXPECT_THAT(sc2, testing::Field(&Config::CommandParam::pre_send, testing::StrEq(""))) << "subcommand2 pre_send";
  EXPECT_THAT(sc2, testing::Field(&Config::CommandParam::run_without_output, testing::StrEq(""))) << "subcommand2 run_without_output";
  EXPECT_THAT(sc2, testing::Field(&Config::CommandParam::run_with_output, testing::StrEq("/bin/false1"))) << "subcommand2 run_with_output";
  EXPECT_THAT(sc2, testing::Field(&Config::CommandParam::post_send, testing::StrEq("after subcommand2."))) << "subcommand2 post_send";
  EXPECT_THAT(sc2, testing::Field(&Config::CommandParam::subcommands, testing::IsEmpty())) << "subcommand2 no subcommands";


  Config::CommandParam c2 = yc->commands["command2"];
  EXPECT_THAT(c2, testing::Field(&Config::CommandParam::pre_send, testing::StrEq(""))) << "command2 pre_send";
  EXPECT_THAT(c2, testing::Field(&Config::CommandParam::run_without_output, testing::StrEq(""))) << "command2 run_without_output";
  EXPECT_THAT(c2, testing::Field(&Config::CommandParam::run_with_output, testing::StrEq("/bin/false"))) << "command2 run_with_output";
  EXPECT_THAT(c2, testing::Field(&Config::CommandParam::post_send, testing::StrEq("after command2."))) << "command2 post_send";
  EXPECT_THAT(c2, testing::Field(&Config::CommandParam::subcommands, testing::IsEmpty())) << "command2 no subcommands";
}

TEST_F(ConfigTest, TestScalarSetMyCommands) {
  unique_ptr<YamlConfig> yc2 = make_unique<YamlConfig>(testfile2);

  EXPECT_THAT(yc2->my_commands, testing::IsEmpty()) << "my_commands empty";
  EXPECT_TRUE(yc2->set_my_commands) << "set_my_commands is true";
}

