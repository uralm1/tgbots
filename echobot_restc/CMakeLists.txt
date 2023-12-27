cmake_minimum_required(VERSION 3.10.2)
project(echobot1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS "ON")

#find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(restc-cpp REQUIRED PATHS /home/ural/src/test/inst NO_DEFAULT_PATH)
find_package(Boost REQUIRED PATHS /home/ural/src/test/inst/boost COMPONENTS
  system
  program_options
  filesystem
  date_time
  context
  coroutine
  chrono
  log
)
#find_package(CURL)

add_executable(echobot1 main.cpp RestcHttpClient.cpp)

target_include_directories(echobot1 PRIVATE
  /home/ural/src/test/tgbot-cpp/include
  #${OPENSSL_INCLUDE_DIR}
  #${Boost_INCLUDE_DIR}
  /home/ural/src/test/restc-cpp/include
  /home/ural/src/test/restc-cpp/dbuild/generated-include
  /home/ural/src/test/inst/include
)
#if (CURL_FOUND)
#    target_include_directories(echobot1 ${CURL_INCLUDE_DIRS})
#    add_definitions(-DHAVE_CURL)
#endif()

target_link_libraries(echobot1
  /home/ural/src/test/tgbot-cpp/build/libTgBot.a
  #${RESTC_CPP_LIBRARIES}
  /home/ural/src/test/restc-cpp/lib/librestc-cppD.a
  #${CMAKE_THREAD_LIBS_INIT}
  #${Boost_LIBRARIES}
  Boost::system
  Boost::program_options
  Boost::filesystem
  Boost::date_time
  Boost::context
  Boost::coroutine
  Boost::chrono
  ZLIB::ZLIB
  ${OPENSSL_LIBRARIES}
)
