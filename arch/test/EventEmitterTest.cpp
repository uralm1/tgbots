#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "EventEmitter.h"

#include <memory>
#include <stdexcept>
#include <any>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;

class EventEmitterTest: public testing::Test {
public:
  unique_ptr<EventEmitter<>> ee;

  void SetUp() { ee = make_unique<EventEmitter<>>(); }
  void TearDown() { ee.reset(); }
};

TEST_F(EventEmitterTest, NormalEvent) {
  int called = 0;
  ee->on("test1", [&](std::any param){ ++called; });
  ee->emit("test1");
  EXPECT_EQ(called, 1) << "event must be emitted";
}

TEST_F(EventEmitterTest, IntEventId) {
  auto ee1 = make_unique<EventEmitter<int>>();
  int called = 0;
  ee1->on(1, [&](std::any param){ ++called; });
  ee1->emit(1);
  EXPECT_EQ(called, 1) << "event must be emitted";
  EXPECT_THAT(ee1->subscribers(1), testing::SizeIs(1)) << "one subscriber";
  EXPECT_THAT(ee1->subscribers(2), testing::SizeIs(0)) << "no subscribers";
}

TEST_F(EventEmitterTest, StringEventId) {
  auto ee2 = make_unique<EventEmitter<string>>();
  int called = 0;
  ee2->on("test2", [&](std::any param){ ++called; });
  ee2->emit("test2");
  EXPECT_EQ(called, 1) << "event must be emitted";
  EXPECT_THAT(ee2->subscribers("test2"), testing::SizeIs(1)) << "one subscriber";
  EXPECT_THAT(ee2->subscribers("test3"), testing::SizeIs(0)) << "no subscribers";
  string ret;
  try {
    ee2->emit("error", string("works"));
  } catch(const exception& ex) {
    ret = ex.what();
  }
  EXPECT_THAT(ret, testing::StrCaseEq("Uncatched error event: works")) << "wrong error exception";
}

TEST_F(EventEmitterTest, Error) {
  ee->on("die", [&](std::any param){ throw runtime_error("works!"); });
  string ret;
  try {
    ee->emit("die");
  } catch(const exception& ex) {
    ret = ex.what();
  }
  EXPECT_EQ(ret, "works!") << "wrong error exception";
}

TEST_F(EventEmitterTest, UnhandledErrorEvent) {
  string ret;
  try {
    ee->emit("error", "works");
  } catch(const exception& ex) {
    ret = ex.what();
  }
  EXPECT_THAT(ret, testing::StrCaseEq("Uncatched error event: works")) << "wrong error exception";
  try {
    ee->emit("error", string_view("string_view"));
  } catch(const exception& ex) {
    ret = ex.what();
  }
  EXPECT_THAT(ret, testing::StrCaseEq("Uncatched error event: string_view")) << "wrong error exception";
  try {
    ee->emit("error", string("string"));
  } catch(const exception& ex) {
    ret = ex.what();
  }
  EXPECT_THAT(ret, testing::StrCaseEq("Uncatched error event: string")) << "wrong error exception";
}

TEST_F(EventEmitterTest, OnError) {
  string err;
  ASSERT_FALSE(ee->has_subscribers("foo")) << "shouldn't have subscribers";
  ee->on_error([&](std::any param){ err = any_cast<string>(param); });
  ASSERT_TRUE(ee->has_subscribers("error")) << "should have subscribers";
  ee->emit("error", string("just works!"));
  EXPECT_EQ(err, "just works!") << "wrong error";
}

TEST_F(EventEmitterTest, NormalEventAgain) {
  int called = 0;
  ee->on("test1", [&](std::any param){ ++called; });
  ee->emit("test1");
  EXPECT_EQ(called, 1) << "event must be emitted";
  EXPECT_THAT(ee->subscribers("test1"), testing::SizeIs(1)) << "one subscriber";
  ee->emit("test1");
  ee->unsubscribe("test1", ee->subscribers("test1").begin()); //first element
  EXPECT_EQ(called, 2) << "event should be emitted twice";
  EXPECT_THAT(ee->subscribers("test1"), testing::SizeIs(0)) << "should be no subscribers";
  ee->emit("test1");
  EXPECT_EQ(called, 2) << "event should not be emitted again";
  ee->emit("test1");
  EXPECT_EQ(called, 2) << "event should not be emitted again";
}

TEST_F(EventEmitterTest, OneTimeEvent) {
  int once = 0;
  ee->once("one_time", [&](std::any){ ++once; });
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(1)) << "one subscriber";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event must be emitted";
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(0)) << "should be no subscribers";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event shouldn't be emitted again";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event shouldn't be emitted again";

  ee->once("one_time", [&](std::any) {
      ee->once("one_time", [&](std::any){ ++once; });
    });
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event should be emitted once";
  ee->emit("one_time");
  EXPECT_EQ(once, 2) << "event should be emitted again";
  ee->emit("one_time");
  EXPECT_EQ(once, 2) << "event shouldn't be emitted again";

  ee->once("one_time", [&](std::any) {
      once = ee->has_subscribers("one_time");
    });
  ee->emit("one_time");
  EXPECT_FALSE(once) << "no subscribers";
}

TEST_F(EventEmitterTest, NestedOneTimeEvents) {
  int once = 0;
  ee->once("one_time", [&](std::any) {
      ee->once("one_time", [&](std::any) {
          ee->once("one_time", [&](std::any) {
              ++once;
            });
        });
    });
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(1)) << "one subscriber";
  ee->emit("one_time");
  EXPECT_EQ(once, 0) << "only first event was emitted";
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(1)) << "one subscriber";
  ee->emit("one_time");
  EXPECT_EQ(once, 0) << "only second event was emitted";
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(1)) << "one subscriber";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "third event was emitted";
  EXPECT_THAT(ee->subscribers("one_time"), testing::SizeIs(0)) << "no subscribers";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event shouldn't be emitted again";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event shouldn't be emitted again";
  ee->emit("one_time");
  EXPECT_EQ(once, 1) << "event shouldn't be emitted again";
}

TEST_F(EventEmitterTest, OneTimeEventUseDirectly) {
  EXPECT_FALSE(ee->has_subscribers("foo")) << "no subscribers";
  int once = 0;
  auto it = ee->once("foo", [&](std::any){ ++once; });
  EXPECT_TRUE(ee->has_subscribers("foo")) << "has subscribers";
  (*it)(std::any());
  EXPECT_EQ(once, 1) << "event must be emitted once";
  EXPECT_FALSE(ee->has_subscribers("foo")) << "no subscribers";
}

TEST_F(EventEmitterTest, Unsubscribe) {
  int counter = 0;
  auto it = ee->on("foo", [&](std::any){ ++counter; });
  ee->on("foo", [&](std::any){ ++counter; });
  ee->on("foo", [&](std::any){ ++counter; });
  ee->unsubscribe("foo", ee->once("foo", [&](std::any){ ++counter; }));
  EXPECT_THAT(ee->subscribers("foo"), testing::SizeIs(3)) << "three subscribers";
  ee->emit("foo")->unsubscribe("foo", it);
  EXPECT_EQ(counter, 3) << "event must be emitted three times";
  EXPECT_THAT(ee->subscribers("foo"), testing::SizeIs(2)) << "two subscribers";
  ee->emit("foo");
  EXPECT_EQ(counter, 5) << "event was emitted two times";
  EXPECT_TRUE(ee->has_subscribers("foo")) << "has subscribers";
  EXPECT_FALSE(ee->unsubscribe("foo")->has_subscribers("foo")) << "no subscribers";
  EXPECT_THAT(ee->subscribers("foo"), testing::SizeIs(0)) << "0 subscribers";
  ee->emit("foo");
  EXPECT_EQ(counter, 5) << "event was not emitted again";
}

TEST_F(EventEmitterTest, ManipulateEvents) {
  string buffer;
  ee->subscribers("foo").emplace_back([&](std::any){ buffer += "one"; });
  ee->subscribers("foo").emplace_back([&](std::any){ buffer += "two"; });
  ee->subscribers("foo").emplace_back([&](std::any){ buffer += "three"; });
  ee->emit("foo");
  EXPECT_EQ(buffer, "onetwothree") << "right result";
  auto& evl = ee->subscribers("foo");
  std::reverse(evl.begin(), evl.end());
  ee->emit("foo");
  EXPECT_EQ(buffer, "onetwothreethreetwoone") << "right result";
}

TEST_F(EventEmitterTest, PassByReference) {
  string buffer;
  //std::any cannot store references, so we need reference wrapper or pass by pointer
  ee->on("foo", [](std::any p){ std::any_cast<std::reference_wrapper<string>>(p).get() += "abc"; });
  ee->on("foo", [](std::any p){ std::any_cast<std::reference_wrapper<string>>(p).get() += "123"; });
  EXPECT_EQ(buffer, "") << "no result";
  ee->emit("foo", std::reference_wrapper<string>(buffer));
  EXPECT_EQ(buffer, "abc123") << "right result";
  ee->unsubscribe("foo");
  using evilpair = std::pair<std::reference_wrapper<string>, string>;
  ee->on("foo", [](std::any p){ std::any_cast<evilpair>(p).first.get() += "456"; });
  ee->on("foo", [](std::any p){ std::any_cast<evilpair>(p).first.get() += std::any_cast<evilpair>(p).second; });

  ee->emit("foo", evilpair(std::reference_wrapper<string>(buffer), "zxc"));
  EXPECT_EQ(buffer, "abc123456zxc") << "right result";
}

