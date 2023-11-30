#ifndef MYMD5_H
#define MYMD5_H

void  mymd5        (const char* const message, char* const digest);
char* mymd5_tostr  (const char* const message);
void  mymd5_print  (const char* const message);
void  mymd5_inplace(      char* const message);
void  mymd5_stretch(      char* const message, const int n);

#endif
