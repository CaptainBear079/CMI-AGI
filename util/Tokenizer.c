#include "Tokenizer.h"

// Tokenizer V2
// - tokenCount is optional for defining a size_t value to store the number of tokens
Token* tokenize(char* str, size_t maxTokens, size_t* tokenCount) {
	// Check for invalid input
	if(str == NULL) {
		fprintf(stderr, "[Tokenizer V2]: ERROR: No string provided. Returning NULL.\n");
		return NULL;
	}
	if(maxTokens == 0) {
		fprintf(stderr, "[Tokenizer V2]: ERROR: maxTokens is 0. Returning NULL.\n");
		return NULL;
	}

	// Allocate memory for tokens
	Token* tokens = malloc(maxTokens * sizeof(Token));
	if(tokens == NULL) {
		fprintf(stderr, "[Tokenizer V2]: ERROR: malloc() failed. Returning NULL.\n");
		return NULL;
	}
	size_t t = 0;            // Index for the tokens
	char buffer[257];        // Buffer for the tokens
	buffer[256] = '\0';      // Null-terminate the buffer
	size_t b = 0;            // Index for the buffer
	bool stringFlag = false; // Tokenizing string flag

	// Tokenize string
	for(unsigned long long s = 0; true; s++) {
		switch((int)str[s]) {
			case (int)'\0': {
				if(b > 0) {
					buffer[b] = '\0';
					b++;
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					if(stringFlag) {
						tokens[t].type = TOKEN__STRING;
					}
					else {
						tokens[t].type = TOKEN__TEXT;
					}
					tokens[t].textLength = b;
					if(tokenCount != NULL) {
						*tokenCount = t;
					}
					return tokens;
				}
				else {
					if(tokenCount != NULL) {
						*tokenCount = t;
					}
					return tokens;
				}
			}
			case (int)' ':
			case (int)'\n': {
				if(stringFlag) {
					if(b > 255) {
						fprintf(stderr, "[Tokenizer V2]: ERROR: String token too long. Returning NULL.\n");
						return NULL;
					}
					else {
						buffer[b] = str[s];
						b++;
					}
				}
				else {
					if(b > 0) {
						buffer[b] = '\0';
						b++;
						tokens[t].text = malloc(b * sizeof(char));
						strcpy(tokens[t].text, buffer);
						tokens[t].type = TOKEN__TEXT;
						tokens[t].textLength = b;
						if((t + 1) == maxTokens) {
							if(tokenCount != NULL) {
								*tokenCount = t;
							}
							return tokens;
						}
						t++;
						b = 0;
					}
				}
			} break;

			case (int)'#':
			case (int)'*':
			case (int)'/':
			case (int)':':
			case (int)'[':
			case (int)']': {
				if(stringFlag) {
					if(b > 255) {
						fprintf(stderr, "[Tokenizer V2]: ERROR: String token too long. Returning NULL.\n");
						return NULL;
					}
					else {
						buffer[b] = str[s];
						b++;
					}
					break;
				}
				if(b > 0) {
					buffer[b] = '\0';
					b++;
					tokens[t].text = malloc(b * sizeof(char));
					strcpy(tokens[t].text, buffer);
					tokens[t].type = TOKEN__TEXT;
					tokens[t].textLength = b;
					if((t + 1) == maxTokens) {
						if(tokenCount != NULL) {
							*tokenCount = t;
						}
						return tokens;
					}
					t++;
					b = 0;
				}

				switch((int)str[s]) {
					case (int)'#': { tokens[t].type = TOKEN__HASHTAG; } break;
					case (int)'*': { tokens[t].type = TOKEN__ASTERISK; } break;
					case (int)'/': { tokens[t].type = TOKEN__SLASH; } break;
					case (int)':': { tokens[t].type = TOKEN__COLON; } break;
					case (int)'[': { tokens[t].type = TOKEN__OPENING_SQARE_BRACKET; } break;
					case (int)']': { tokens[t].type = TOKEN__CLOSING_SQARE_BRACKET; } break;
					default: {
						fprintf(stderr, "[Tokenizer V2]: ERROR: Memory corruption detected. Returning NULL.\n");
						return NULL;
					}
				}

				tokens[t].text = NULL;
				tokens[t].textLength = 0;
				if((t + 1) == maxTokens) {
					if(tokenCount != NULL) {
						*tokenCount = t;
					}
					return tokens;
				}
				t++;
			} break;

			case (int)'\"': {
				if(stringFlag) {
					if(b > 0) {
						buffer[b] = '\0';
						b++;
						tokens[t].text = malloc(b * sizeof(char));
						strcpy(tokens[t].text, buffer);
					}
					else {
						tokens[t].text = NULL;
					}
					tokens[t].type = TOKEN__STRING;
					tokens[t].textLength = b;
					if((t + 1) == maxTokens) {
						if(tokenCount != NULL) {
							*tokenCount = t;
						}
						return tokens;
					}
					t++;
					b = 0;
					stringFlag = false;
				}
				else {
					if(b > 0) {
						buffer[b] = '\0';
						b++;
						tokens[t].text = malloc(b * sizeof(char));
						strcpy(tokens[t].text, buffer);
						tokens[t].type = TOKEN__TEXT;
						tokens[t].textLength = b;
						if((t + 1) == maxTokens) {
							if(tokenCount != NULL) {
								*tokenCount = t;
							}
							return tokens;
						}
						t++;
						b = 0;
					}
					stringFlag = true;
				}
			} break;
			default: {
				buffer[b] = str[s];
				b++;
			} break;
		}
	}

	// EXPECTEDLY UNREACHABLE
	// Exited for-loop
	fprintf(stderr, "[Tokenizer V2]: ERROR: Exited for-loop. Returning NULL.\n");
	return NULL;
}
