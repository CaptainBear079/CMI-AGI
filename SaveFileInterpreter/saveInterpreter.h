#pragma once

#include <stdio.h>

extern char* tempFolder;
extern char* sessionSave;

#define TOKEN__TEXT 0
#define TOKEN__META_INFO_START 1
#define TOKEN__META_INFO_END 2

typedef struct _Token_ {
    int type;
    char* text;
} Token;
