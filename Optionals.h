#ifndef Theincgi_Optionals_H
#define Theincgi_Optionals_H

struct OptUL {
  bool present;
  unsigned long value;
  OptUL();
  OptUL(unsigned long v);
};


struct OptUI {
  bool present;
  unsigned int value;
  OptUI();
  OptUI(unsigned int v);
};

#endif
