#ifndef Theincgi_Value_H
#define Theincgi_Value_H

union Value {
  byte bArr1[1];
  byte bArr2[2];
  byte bArr4[4];
  byte b;
  char c;
  int i;
  short s;
  long l;
  float f;
  double d;
  unsigned short us;
  unsigned int ui;
  unsigned long ul;
};

inline void arrayCopy(byte src[], byte dest[], int startSrc, int startDest, int len){
  for(int i = 0; i<len; i++)
    dest[startDest + i] = src[startSrc + i];
}
inline void arrayCopy(String &src, byte dest[], int startSrc, int startDest, int len){
  for(int i = 0; i<len; i++)
    dest[startDest + i] = src[startSrc + i];
} 
inline void arrayCopy(String &src, byte dest[]){
  arrayCopy(src, dest, 0, 0, src.length());
} 
#endif
