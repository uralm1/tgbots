#include "PiBotApp.h"

#include <string>
#include <iostream>

using namespace std;
using namespace TgBot;


void PiBotApp::cmd_reboot(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  send(message, "Reboot command accepted.");
  run_without_output(message, "/etc/openhab2/misc/reboot.sh");
}

void PiBotApp::cmd_rebootcheck(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  send(message, "Reboot (with fsck) command accepted.");
  run_without_output(message, "/etc/openhab2/misc/reboot_check.sh");
}

void PiBotApp::cmd_status(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  string service = param2(message);
  if (service == "") {
    run_with_output(message, "uptime;echo;free");

  } else if (service == "ip") {
    run_with_output(message, "ip addr");

  } else if (service == "df") {
    run_with_output(message, "df");

  } else if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion" ||
      service == "sshd") {
    run_with_output(message, "systemctl -q --no-ask-password --no-pager status " + service);
  } else {
    send(message, "Unknown status (empty/ip/df/service).");
  }
}

void PiBotApp::cmd_restart(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  string service = param2(message);
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output(message, "systemctl -q --no-ask-password --no-pager restart " + service);
  } else {
    send(message, "Command error.");
  }
}

void PiBotApp::cmd_start(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  string service = param2(message);
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output(message, "systemctl -q --no-ask-password --no-pager start " + service);
  } else {
    send(message, "Command error.");
  }
}

void PiBotApp::cmd_stop(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  string service = param2(message);
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output(message, "systemctl -q --no-ask-password --no-pager stop " + service);
  } else {
    send(message, "Command error.");
  }
}

