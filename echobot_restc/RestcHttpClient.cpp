#include "RestcHttpClient.h"

#include <iostream>

#include "restc-cpp/logging.h"
#include "restc-cpp/RequestBuilder.h"
#include "tgbot/net/HttpParser.h"

using namespace restc_cpp;

namespace TgBot {

RestcHttpClient::RestcHttpClient() :
  //start service thread
  rest_client_(RestClient::Create()) {
}

//Request::Properties properties;
//properties.proxy.type = Request::Proxy::Type::HTTPS;
//properties.proxy.address = "http://1.2.3.4:8080";
//properties.throwOnHttpError = false;
RestcHttpClient::RestcHttpClient(const restc_cpp::Request::Properties& properties) :
  //start service thread
  rest_client_(RestClient::Create(properties)) {
}

RestcHttpClient::~RestcHttpClient() {
  //stop service thread
  rest_client_->CloseWhenReady(false);
}

std::string RestcHttpClient::makeRequest(const Url& url, const std::vector<HttpReqArg>& args) const {
  const std::string u = url.protocol + "://" + url.host + url.path;
  //std::clog << "u = " << u << std::endl;

  auto res = rest_client_->ProcessWithPromiseT<std::string>([&](Context& ctx) {
    if (args.empty()) {
      return RequestBuilder(ctx)
        .Get(u)
        .Header("Connection", "close")
        .Execute()
        ->GetBodyAsString();
    } else {

      HttpParser parser;
      std::string boundary = parser.generateMultipartBoundary(args);
      std::string requestData;
      std::string contentType;
      if (boundary.empty()) {
        contentType = "application/x-www-form-urlencoded";
        requestData = parser.generateWwwFormUrlencoded(args);
      } else {
        contentType = "multipart/form-data; boundary=" + boundary;
        requestData = parser.generateMultipartFormData(args, boundary);
      }

      return RequestBuilder(ctx)
        .Post(u)
        .Header("Connection", "close")
        .Header("Content-Type", contentType)
        .Header("Content-Length", std::to_string(requestData.length()))
        .Data(requestData)
        .Execute()
        ->GetBodyAsString();
    }
  });

  try {
    return res.get();
  } catch (const std::exception& ex) {
    throw std::runtime_error(ex.what());
  }
}

void RestcHttpClient::EnableRestcLogging() {
  // maximum loglevel is limited at compile time in RESTC_CPP_LOG_LEVEL_STR option
  Logger::Instance().SetLogLevel(restc_cpp::LogLevel::TRACE);

  Logger::Instance().SetHandler([](LogLevel level, const std::string& msg) {
    static const std::array<std::string, 6> levels = {"NONE", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::clog << std::put_time(localtime(&now), "%c") << ' '
      << levels.at(static_cast<size_t>(level))
      << ' ' << std::this_thread::get_id() << ' '
      << msg << std::endl;
  });
}

}//namespace TgBot

