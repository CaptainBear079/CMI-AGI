#include "Tokenizer.h"

Token* tokenize(char* str, size_t maxTokens) {
	// Variables
	Token* tokens = malloc(64 * sizeof(Token)); // Allocate space for 64 Tokens (re-allocated for more tokens if needed)
	size_t allocTokens = 64;
	char buffer[256]; // Buffer for the tokens
	int b = 0; // Index for the buffer
	int t = 0; // Index for the tokens
	bool stringFlag = false; // Inside string flag

	// Tokenize save file
	for(unsigned long long s = 0; str != '\0'; s++) {
		switch((int)str[s]) {
			case (int)' ':
			case (int)'\n': {
				if(b > 0) {
					buffer[b] = '\0';
					b++;
					if(t >= allocTokens) {
						allocTokens += 64;
						realloc(tokens, allocTokens * sizeof(Token));
					}
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					tokens[t].type = TOKEN__TEXT;
					tokens[t].textLength = b;
					t++;
					b = 0;
				}
			} break;
			case (int)'\0': {
				if(b > 0) {
					buffer[b] = '\0';
					b++;
					if(t >= allocTokens) {
						allocTokens += 64;
						realloc(tokens, allocTokens * sizeof(Token));
					}
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					tokens[t].type = TOKEN__TEXT;
					tokens[t].textLength = b;
					t++;
					b = 0;
				}

				if((t + 1) < allocTokens) {
					realloc(tokens, (t + 1) * sizeof(Token));
				}
				return tokens;
			} break;
			case (int)'#':
			case (int)'*':
			case (int)'/':
			case (int)'[':
			case (int)']':
			case (int)':': {
				if(b > 0) {
					buffer[b] = '\0';
					b++;
					if(t >= allocTokens) {
						allocTokens += 64;
						realloc(tokens, allocTokens * sizeof(Token));
					}
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					tokens[t].type = TOKEN__TEXT;
					tokens[t].textLength = b;
					t++;
					b = 0;
				}

				if(str[s] == ':') {
					tokens[t].type = TOKEN__COLON;
				}
				else if(str[s] == '[') {
					tokens[t].type = TOKEN__OPENING_SQARE_BRACKET;
				}
				else if(str[s] == ']') {
					tokens[t].type = TOKEN__CLOSING_SQARE_BRACKET;
				}
				tokens[t].text = NULL;
				tokens[t].textLength = 0;
				t++;
			} break;
			case (int)'\"': {
				if(b > 0 && !stringFlag) {
					buffer[b] = '\0';
					b++;
					if(t >= allocTokens) {
						allocTokens += 64;
						realloc(tokens, allocTokens * sizeof(Token));
					}
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					tokens[t].type = TOKEN__TEXT;
					tokens[t].textLength = b;
					t++;
					b = 0;
				}

				if(!stringFlag) {
					stringFlag = true;
				}
				else {
					if(b > 0) {
						buffer[b] = '\0';
						b++;
						if(t >= allocTokens) {
							allocTokens += 64;
							realloc(tokens, allocTokens * sizeof(Token));
						}
						tokens[t].text = malloc(b * sizeof(char));
						strcpy(tokens[t].text, buffer);
						tokens[t].type = TOKEN__STRING;
						tokens[t].textLength = b;
						t++;
						b = 0;
					}
					stringFlag = false;
				}
			} break;
			default: {
				buffer[b] = str[s];
				b++;
			} break;
		}
	}

	// DEBUG
	#ifdef _DEBUG
	for(int i = 0; i < t; i++) {
		printf("DEBUG: Token: %d, Type: %d, Text: %s\n", i, tokens[i].type, tokens[i].text);
	}
	#endif
	return 0;
}
