#include <gtest/gtest.h>
//#include <gmock/gmock.h>

#include "Controller.h"

#include <memory>

using namespace std;

class ControllerTest: public testing::Test {
public:
  unique_ptr<Controller> c;
  TgBot::Message::Ptr msg;

  void SetUp() {
    c = make_unique<Controller>(nullptr);
    msg = make_shared<TgBot::Message>();
    c->message(msg);
  }
  void TearDown() { c->finish(); c.reset(); }
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

TEST_F(ControllerTest, MdEscapeTest) {
  EXPECT_EQ(Controller::md_escape(""), "") << "empty string";
  EXPECT_EQ(Controller::md_escape("asdfghhjkjl123"), "asdfghhjkjl123") << "simple string";
  EXPECT_EQ(Controller::md_escape("asd!fgh```asd!123```"), "asd\\!fgh```asd!123```") << "!";
  EXPECT_EQ(Controller::md_escape("asd!fgh`asd!123```!```!``!```!"), "asd\\!fgh`asd\\!123```!```\\!``\\!```!") << "tilda3 block";
  EXPECT_EQ(Controller::md_escape("asd!<>#(){}|.-fgh```asd!<>#(){}|.-123```"), "asd\\!\\<\\>\\#\\(\\)\\{\\}\\|\\.\\-fgh```asd!<>#(){}|.-123```") << "!<>#(){}|.-";
}

