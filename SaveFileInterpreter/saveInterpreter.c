#include "saveInterpreter.h"

// INDEV
void readSessionSave() {
    Token* tokens[1000] = { 0 };
	char* temp = malloc(sizeof(tempFolder) * sizeof(char));
	strcpy(temp, tempFolder);
	strcat(temp, sessionSave);
	FILE* session_fptr = fopen(temp, "r");
	char c[257];
	c[256] = '\0';
    int t = 0;
	for(int i = 0;(c[i] = fgetc(session_fptr) != EOF) && i < 256; i++) {
		if(c[i] = '[') {
            tokens[t]->type = TOKEN__META_INFO_START;
            continue;
        }
        if(c[i] = ']') {
            tokens[t]->type = TOKEN__META_INFO_END;
        }
        tokens[t]->type = TOKEN__TEXT;
	}
}
