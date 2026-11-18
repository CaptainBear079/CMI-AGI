#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN__TEXT 0
#define TOKEN__NULL 1
#define TOKEN__STRING 2
#define TOKEN__HASHTAG 3
#define TOKEN__ASTERISK 4
#define TOKEN__SLASH 5
#define TOKEN__COLON 6
#define TOKEN__OPENING_SQARE_BRACKET 7
#define TOKEN__CLOSING_SQARE_BRACKET 8


typedef struct _Token_ {
    int type;
    char* text;
    int textLength;
} Token;

// Tokenizer (tokenCount is optional if you want to know how many tokens where allocated)
Token* tokenize(char* str, size_t maxTokens, size_t* tokenCount);
