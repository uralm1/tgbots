#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Config.h"

#include <cstdio>
#include <memory>
#include <fstream>

using namespace std;
using namespace TgBot;

static const string testfile1 = "_test1.yml";

class ConfigTest: public testing::Test {
public:
  unique_ptr<YamlConfig> yc;

  static void SetUpTestSuite() {
    ofstream ofs(testfile1);
    ofs << "Token: 123456:asdfghjkl" << endl;
    ofs << "SendOnlyToChatId: 654321" << endl;
    ofs << "AllowedUserIds:" << endl;
    ofs << "  - 1111" << endl;
    ofs << "  - 2222" << endl;
    ofs << "SleepInterval: 123" << endl;
    ofs << "SetMyCommands:" << endl;
    ofs << "  test1: test1 description" << endl;
    ofs << "  test2: \"test2 description\"" << endl;
  }

  static void TearDownTestSuite() {
    remove(testfile1.c_str());
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

  EXPECT_THAT(yc->set_my_commands, testing::SizeIs(2)) << "set_my_commands size";

  EXPECT_THAT(*yc->set_my_commands[0], testing::FieldsAre("test1", "test1 description")) << "set_my_commands[0]";
  EXPECT_THAT(*yc->set_my_commands[1], testing::FieldsAre("test2", "test2 description")) << "set_my_commands[0]";

}

