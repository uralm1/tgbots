#cmake_minimum_required(VERSION 3.10.2)
#project(test_echobot2)

add_executable(test_echobot2
  main.cpp
  EventEmitterTest.cpp
)

include_directories(..)
#link_directories(../build/lib)

target_link_libraries(test_echobot2
  gtest
  gmock
)

add_test(NAME test_echobot2 COMMAND test_echobot2)

