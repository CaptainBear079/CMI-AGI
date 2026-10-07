#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* tempFolder;
extern char* sessionSave;

#define TOKEN__TEXT 0
#define TOKEN__META_INFO 1
#define TOKEN__GROUP 2

typedef struct _AISave_ {} AISave;
typedef struct _ENVSave_ {} ENVSave;

typedef struct _Save_ {
    char* aiSavePath;
    char* envSavePath;
    AISave* aiSave;
    ENVSave* envSave;
} Save;

typedef struct _Token_ {
    int type;
    char* text;
    int textLength;
} Token;

int loadSave(Save* save);
