#cmake_minimum_required(VERSION 3.10.2)
#project(test_pibot)

add_executable(test_pibot
  main.cpp
  PiUtilsTest.cpp
  ../PiBotApp.cpp
  ../Poller.cpp
  ../Config.cpp
  ../PiCommands.cpp
  ../PiUtils.cpp
)

include_directories(
  ..
  /home/ural/src/test/tgbot-cpp/include
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${YAML_CPP_INCLUDE_DIR}
)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

#link_directories(../build/lib)

target_link_libraries(test_pibot
  gtest
  gmock
  /home/ural/src/test/tgbot-cpp/build/libTgBot.a
  #${CMAKE_THREAD_LIBS_INIT}
  ${YAML_CPP_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)
if (CURL_FOUND)
target_link_libraries(test_pibot ${CURL_LIBRARIES})
endif()

add_test(NAME test_pibot COMMAND test_pibot)

