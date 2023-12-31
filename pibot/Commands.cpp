#include "Controller.h"
#include "PiBotApp.h"

#include <string>
#include <iostream>

using namespace std;
using namespace TgBot;


void Controller::cmd_reboot(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  send("Reboot command accepted.");
  run_without_output("/etc/openhab2/misc/reboot.sh");
  finish();
}

void Controller::cmd_rebootcheck(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  send("Reboot (with fsck) command accepted.");
  run_without_output("/etc/openhab2/misc/reboot_check.sh");
  finish();
}

void Controller::cmd_status(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  string service = param2();
  if (service == "") {
    run_with_output("uptime;echo;free");

  } else if (service == "ip") {
    run_with_output("ip addr");

  } else if (service == "df") {
    run_with_output("df");

  } else if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion" ||
      service == "sshd") {
    run_with_output("systemctl -q --no-ask-password --no-pager status " + service);
  } else {
    send("Unknown status (empty/ip/df/service).");
  }
  finish();
}

void Controller::cmd_restart(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  string service = param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output("systemctl -q --no-ask-password --no-pager restart " + service);
  } else {
    send("Command error.");
  }
  finish();
}

void Controller::cmd_start(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  string service = param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output("systemctl -q --no-ask-password --no-pager start " + service);
  } else {
    send("Command error.");
  }
  finish();
}

void Controller::cmd_stop(Message::Ptr message) {
  app()->controller(message);
  if ( !user_allowed() ) return;

  string service = param2();
  if (service == "apache2" ||
      service == "openhab2" ||
      service == "motion") {
    run_with_output("systemctl -q --no-ask-password --no-pager stop " + service);
  } else {
    send("Command error.");
  }
  finish();
}

