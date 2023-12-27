#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <tgbot/tgbot.h>
#include "RestcHttpClient.h"

using namespace std;
using namespace TgBot;

int main() {
    string token("12345678:qwertyuiop");
    printf("Token: %s\n", token.c_str());

    restc_cpp::Request::Properties properties;
    properties.proxy.type = restc_cpp::Request::Proxy::Type::HTTPS;
    properties.proxy.address = "http://1.2.3.4:8080";

    RestcHttpClient httpClient(properties);
    //httpClient.EnableRestcLogging();

    Bot bot(token, httpClient);
    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        //libcurl has its 25s timeout on connection
        TgLongPoll longPoll(bot, 100, 20 /*timeout*/);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
