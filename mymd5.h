#ifndef MYMD5_H
#define MYMD5_H

// Ref.: https://en.wikipedia.org/wiki/MD5#Algorithm
//   message must be null terminated string of any length
//   digest buffer size must be >= 33 (32 hex characters + NUL)
void   mymd5        (const char * const message, char * const digest);

// Return string pointer to md5 hex digest of message
//   message must be null terminated string of any length
// NB: not thread-safe because it uses a single static buffer
char * mymd5_tostr  (const char * const message);

// Print md5 hex digest of message to stdout
//   message must be null terminated string of any length
// NB: not thread-safe because mymd5_str() uses a single static buffer
void   mymd5_print  (const char * const message);

// Transform message to md5 hex digest in-place
//   message must be null terminated string of any length
//   message buffer size must be >= 33 (32 hex characters + NUL)
void   mymd5_inplace(      char * const message);

// Transform message to md5 hex digest in-place, stretched N times
//   message must be null terminated string of any length
//   message buffer size must be >= 33 (32 hex characters + NUL)
//   stretch should be > 0 and not very large
void   mymd5_stretch(      char * const message, const int n);

#endif
