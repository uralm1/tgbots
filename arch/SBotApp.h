#pragma once

#include <string>
//#include <iostream>

#include "App.h"


class SBotApp : public App {
public:
  SBotApp(const std::string& config_file);

  //virtual ~SBotApp() { std::clog << "in SBotApp destructor\n"; }

  virtual void startup() override;

}; //class SBotApp

