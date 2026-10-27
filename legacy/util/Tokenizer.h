#pragma once

#include <stdbool.h>
#include <stdlib.h>

#define TOKEN__TEXT 0
#define TOKEN__STRING 1
#define TOKEN__HASHTAG 2
#define TOKEN__ASTERISK 3
#define TOKEN__SLASH 4
#define TOKEN__COLON 5
#define TOKEN__OPENING_SQARE_BRACKET 6
#define TOKEN__CLOSING_SQARE_BRACKET 7


typedef struct _Token_ {
    int type;
    char* text;
    int textLength;
} Token;

Token* tokenize(char* str, size_t maxTokens);
