#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util/Tokenizer.h"

extern char* tempFolder;
extern char* sessionSave;

typedef struct _AISave_ {} AISave;
typedef struct _ENVSave_ {} ENVSave;

typedef struct _Save_ {
    char* aiSavePath;
    char* envSavePath;
    AISave* aiSave;
    ENVSave* envSave;
} Save;

int loadSave(Save* save);
