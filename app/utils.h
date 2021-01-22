#ifndef _UTILS_H
#define _UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>

int base64_encode(const char *indata, int inlen, char *outdata);
int base64_decode(const char *indata, int inlen, char *outdata);
int calc_file_len(const char *file_path);
void replace_str(char *new_buf, char *data, char *rep, char *to);
#endif
