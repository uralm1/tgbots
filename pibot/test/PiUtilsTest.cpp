#include <gtest/gtest.h>
//#include <gmock/gmock.h>

#include "PiBotApp.h"

#include <memory>

using namespace std;

class PiUtilsTest: public testing::Test {
public:
  unique_ptr<PiBotApp> app;
  TgBot::Message::Ptr msg;

  void SetUp() {
    app = make_unique<PiBotApp>("../telegram.yml");
    msg = make_shared<TgBot::Message>();
  }
  void TearDown() { msg.reset(); app.reset(); }
};

TEST_F(PiUtilsTest, ParamTest) {
  msg->text = "/test";
  EXPECT_EQ(app->param(msg, 0), "") << "no param0";
  EXPECT_EQ(app->param(msg, 1), "/test") << "param1";
  EXPECT_EQ(app->param(msg, 2), "") << "no param2";
  EXPECT_EQ(app->param2(msg), "") << "no param2";

  msg->text = "/test aaa";
  EXPECT_EQ(app->param(msg, 0), "") << "no param0";
  EXPECT_EQ(app->param(msg, 1), "/test") << "param1";
  EXPECT_EQ(app->param(msg, 2), "aaa") << "param2";
  EXPECT_EQ(app->param2(msg), "aaa") << "param2";

  msg->text = "";
  EXPECT_EQ(app->param(msg, 0), "") << "no param0";
  EXPECT_EQ(app->param(msg, 1), "") << "no param1";
  EXPECT_EQ(app->param(msg, 2), "") << "no param2";

}

