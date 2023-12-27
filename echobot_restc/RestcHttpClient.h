#pragma once

#include "tgbot/net/HttpClient.h"
#include "tgbot/net/Url.h"
#include "tgbot/net/HttpReqArg.h"

#include <memory>
#include <string>
#include <vector>

#include "restc-cpp/restc-cpp.h"

namespace TgBot {

class RestcHttpClient : public HttpClient {

public:
  RestcHttpClient();
  RestcHttpClient(const restc_cpp::Request::Properties& properties);
  ~RestcHttpClient() override;

  void EnableRestcLogging();

  /**
   * @brief Sends a request to the url.
   *
   * If there's no args specified, a GET request will be sent, otherwise a POST request will be sent.
   * If at least 1 arg is marked as file, the content type of a request will be multipart/form-data, otherwise it will be application/x-www-form-urlencoded.
   */
  std::string makeRequest(const Url& url, const std::vector<HttpReqArg>& args) const override;

private:
  std::unique_ptr<restc_cpp::RestClient> rest_client_;
}; //class RestcHttpClient

} //namespace TgBot
