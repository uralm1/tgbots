cmake_minimum_required(VERSION 3.10.2)
project(pibot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_WARN_NEW_VERSIONS ON)

include(CTest)

find_package(Threads REQUIRED)
#use cmake -DCMAKE_PREFIX_PATH=$HOME/src/cpp/inst
find_package(yaml-cpp REQUIRED)
find_package(OpenSSL REQUIRED)
#use cmake -DBoost_INCLUDE_DIR=$HOME/src/cpp/boost/include
find_package(Boost REQUIRED COMPONENTS system)
find_package(CURL)
#to find tgbot-cpp library define tgbot-cpp_DIR
#cmake -Dtgbot-cpp_DIR=$HOME/src/cpp/tgbot-cpp

include_directories(
  ${tgbot-cpp_DIR}/include
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
  BotApp.cpp
  Poller.cpp
  Config.cpp
  Commands.cpp
  Controller.cpp
)

target_link_libraries(pibot
  ${tgbot-cpp_DIR}/lib/libTgBot.a
  ${CMAKE_THREAD_LIBS_INIT}
  ${YAML_CPP_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
)
if (CURL_FOUND)
target_link_libraries(pibot ${CURL_LIBRARIES})
endif()

if (BUILD_TESTING)
add_subdirectory(test)
enable_testing()
endif()

