#include <gtest/gtest.h>
//#include <gmock/gmock.h>

#include "PiBotApp.h"

#include <memory>

using namespace std;

class ControllerTest: public testing::Test {
public:
  unique_ptr<PiBotApp> app;
  Controller* c;
  TgBot::Message::Ptr msg;

  void SetUp() {
    app = make_unique<PiBotApp>("../pibot.yml");
    msg = make_shared<TgBot::Message>();
    c = app->controller(msg);
  }
  void TearDown() { c->finish(); app.reset(); }
};

TEST_F(ControllerTest, ParamTest) {
  msg->text = "/test";
  EXPECT_EQ(c->param(0), "") << "no param0";
  EXPECT_EQ(c->param(1), "/test") << "param1";
  EXPECT_EQ(c->param(2), "") << "no param2";
  EXPECT_EQ(c->param2(), "") << "no param2";

  msg->text = "/test aaa";
  EXPECT_EQ(c->param(0), "") << "no param0";
  EXPECT_EQ(c->param(1), "/test") << "param1";
  EXPECT_EQ(c->param(2), "aaa") << "param2";
  EXPECT_EQ(c->param2(), "aaa") << "param2";

  msg->text = "";
  EXPECT_EQ(c->param(0), "") << "no param0";
  EXPECT_EQ(c->param(1), "") << "no param1";
  EXPECT_EQ(c->param(2), "") << "no param2";

}

