#ifndef Theincgi_Value_H
#define Theincgi_Value_H

union Value {
  byte[] bArr,
  byte b,
  char c,
  int i,
  short s,
  long l,
  unsigned byte ub,
  unsigned short us,
  unsigned int ui,
  unsigned long ul;
}

#endif
