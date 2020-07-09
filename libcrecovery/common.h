#ifndef LIBCRECOVERY_COMMON_H
#define LIBCRECOVERY_COMMON_H

#include <stdio.h>

int __system(const char *command);
FILE * __popen(const char *program, const char *type);
int __pclose(FILE *iop);
int dumwolf_create_header(const char* bootimg, const char* directory);
int dumwolf_extract_header(const char* path, const char* directory);
#endif