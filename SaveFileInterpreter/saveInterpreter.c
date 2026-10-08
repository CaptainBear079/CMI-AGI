#include "saveInterpreter.h"

int readSave(char* save, Token* tokens, int* tokenCount) {
	// Variables
	char c;
	char buffer[256];
	int bIndex = 0;
	tokens = malloc(512 * sizeof(Token));

	// Open save file
	FILE* saveFile = fopen(save, "r");
	if(saveFile == NULL) {
		fprintf(stderr, "[SaveFileInterpreter]: Cannot open save file: %s\n", save);
		return -1;
	}

	// Tokenize save file
	while(!feof(saveFile)) {
		c = fgetc(saveFile);
		switch((int)c) {
			case EOF:
			case (int)' ':
			case (int)'\n': {
				if(bIndex > 0) {
					buffer[bIndex] = '\0';
					tokens[*tokenCount].text = malloc(bIndex * sizeof(char));
					strcpy(tokens[*tokenCount].text, buffer);
					tokens[*tokenCount].type = TOKEN__TEXT;
					tokens[*tokenCount].textLength = bIndex + 1;
					(*tokenCount)++;
					bIndex = 0;
				}
			} break;
			case (int)'[':
			case (int)']':
			case (int)':': {
				if(bIndex > 0) {
					buffer[bIndex] = '\0';
					tokens[*tokenCount].text = malloc(bIndex * sizeof(char));
					strcpy(tokens[*tokenCount].text, buffer);
					tokens[*tokenCount].type = TOKEN__TEXT;
					tokens[*tokenCount].textLength = bIndex + 1;
					(*tokenCount)++;
					bIndex = 0;
				}

				if(c == ':') {
					tokens[*tokenCount].type = TOKEN__GROUP;
				}
				else {
					tokens[*tokenCount].type = TOKEN__META_INFO;
				}
				tokens[*tokenCount].text = NULL;
				tokens[*tokenCount].textLength = 0;
				(*tokenCount)++;
			} break;
			default: {
				buffer[bIndex] = c;
				bIndex++;
			} break;
		}
	}

	// Close save file
	fclose(saveFile);

	// DEBUG
	#ifdef _DEBUG
	for(int i = 0; i < *tokenCount; i++) {
		printf("DEBUG: Token: %d, Type: %d, Text: %s\n", i, tokens[i].type, tokens[i].text);
	}
	#endif
	return 0;
}

// INDEV
int parseSave(Save* save, const Token* tokens, const int tokenCount) {
	// Parse tokens
	if(tokens[0].type == TOKEN__META_INFO) {
		if(
			tokens[1].type == TOKEN__TEXT
			&& tokens[2].type == TOKEN__META_INFO
		) {
			if(strcmp(tokens[1].text, "SESSION") == 0) {
				for(int i = 0; (i + 2) < tokenCount; i++) {
					if(
						tokens[i].type == TOKEN__TEXT
						&& tokens[i + 1].type == TOKEN__GROUP
						&& tokens[i + 2].type == TOKEN__TEXT
					) {
						if(strcmp(tokens[i].text, "ai") == 0) {
							i += 2;
							save->aiSavePath = malloc((tokens[i].textLength) * sizeof(char));
							strcpy(save->aiSavePath, tokens[i].text);
						}
						else if(strcmp(tokens[i].text, "env") == 0) {
							i += 2;
							save->envSavePath = malloc((tokens[i].textLength) * sizeof(char));
							strcpy(save->envSavePath, tokens[i].text);
						}
					}
				}
			}
		}
	}
	return 0;
}

// INDEV
int loadSave(Save* save) {
	// Variables
	Token* tokens;
	int tokenCount = 0;
	// Load session save
	if(readSave("./TEMPLATES/session.cmi_save", tokens, &tokenCount) != 0)
		return -1;
	if(parseSave(save, tokens, tokenCount) != 0)
		return -1;
	
	// Clean up
	for(int i = 0; i < tokenCount; i++) {
		free(tokens[i].text);
	}
	return 0;
}
