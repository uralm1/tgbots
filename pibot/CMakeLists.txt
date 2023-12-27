cmake_minimum_required(VERSION 3.10.2)
project(pibot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)

#include(CTest)

#find_package(Threads REQUIRED)
find_package(yaml-cpp REQUIRED PATHS /home/ural/src/test/inst NO_DEFAULT_PATH)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED PATHS /home/ural/src/test/inst/boost COMPONENTS system)
find_package(CURL)

include_directories(
  /home/ural/src/test/tgbot-cpp/include
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${YAML_CPP_INCLUDE_DIR}
)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

link_directories(
  ${YAML_CPP_LIBRARY_DIR}
)

add_executable(pibot
  main.cpp
  PiBotApp.cpp
  Poller.cpp
  Config.cpp
  PiCommands.cpp
)

target_link_libraries(pibot
  /home/ural/src/test/tgbot-cpp/build/libTgBot.a
  #${CMAKE_THREAD_LIBS_INIT}
  ${YAML_CPP_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CURL_LIBRARIES}
)

#add_subdirectory(test)
#enable_testing()

