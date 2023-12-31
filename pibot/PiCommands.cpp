#include "PiBotApp.h"

#include <string>
#include <iostream>

using namespace std;
using namespace TgBot;


void PiBotApp::cmd_reboot(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  c->send("Reboot command accepted.");
  c->run_without_output("/etc/openhab2/misc/reboot.sh");
  c->finish();
}

void PiBotApp::cmd_rebootcheck(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  c->send("Reboot (with fsck) command accepted.");
  c->run_without_output("/etc/openhab2/misc/reboot_check.sh");
  c->finish();
}

void PiBotApp::cmd_status(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  string service = c->param2();
  if (service == "") {
    c->run_with_output("uptime;echo;free");

  } else if (service == "ip") {
    c->run_with_output("ip addr");

  } else if (service == "df") {
    c->run_with_output("df");

  } else if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion" ||
      service == "sshd") {
    c->run_with_output("systemctl -q --no-ask-password --no-pager status " + service);
  } else {
    c->send("Unknown status (empty/ip/df/service).");
  }
  c->finish();
}

void PiBotApp::cmd_restart(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  string service = c->param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    c->run_with_output("systemctl -q --no-ask-password --no-pager restart " + service);
  } else {
    c->send("Command error.");
  }
  c->finish();
}

void PiBotApp::cmd_start(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  string service = c->param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    c->run_with_output("systemctl -q --no-ask-password --no-pager start " + service);
  } else {
    c->send("Command error.");
  }
  c->finish();
}

void PiBotApp::cmd_stop(Message::Ptr message) {
  auto c = controller(message);
  if ( !c->user_allowed() ) return;

  string service = c->param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    c->run_with_output("systemctl -q --no-ask-password --no-pager stop " + service);
  } else {
    c->send("Command error.");
  }
  c->finish();
}

