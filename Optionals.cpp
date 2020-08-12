#include <Arduino.h>
#include "Optionals.h"

OptUL::OptUL(){
  this->present = false;
}

OptUL::OptUL(unsigned long v){
  this->present = true;
  this->value = v;
}


OptUI::OptUI(){
  this->present = false;
}
OptUI::OptUI(unsigned int v){
  this->present = true;
  this->value = v;
}
