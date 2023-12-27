#include "PiBotApp.h"

#include "command.h"

#include <cstdlib>
#include <string>
//#include <iostream>

using namespace std;
using namespace TgBot;


void PiBotApp::cmd_reboot(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  send(message, "Reboot command accepted.");
  int r = system("/etc/openhab2/misc/reboot.sh");
  send(message, "Command status: " + to_string(r) + ".");
}

void PiBotApp::cmd_rebootcheck(Message::Ptr message) {
  if ( !user_allowed(message) ) return;

  send(message, "Reboot (with fsck) command accepted.");
  int r = system("/etc/openhab2/misc/reboot_check.sh");
  send(message, "Command status: " + to_string(r) + ".");
}

