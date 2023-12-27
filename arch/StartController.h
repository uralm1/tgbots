#pragma once

#include "Controller.h"

class StartController : public Controller {
public:
  StartController() { /*std::clog << "in StartController constructor\n";*/ }
  virtual ~StartController() { /*std::clog << "in StartController destructor\n";*/ }

  void start();

}; //class StartController

