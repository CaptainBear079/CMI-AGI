#include "saveInterpreter.h"

// INDEV
Token* readSave(char* save, int maxTokens) {
	// Variables
	char* saveBuffer;
	saveBuffer = malloc(513);
	saveBuffer[511] = '\0';
	int sb = 0;
	int sbSize = 512;
	Token* tokens;

	// Open save file
	FILE* saveFile = fopen(save, "r");
	if(saveFile == NULL) {
		fprintf(stderr, "[SaveFileInterpreter]: Cannot open save file: %s\n", save);
		return NULL;
	}

	while(true) {
		saveBuffer[sb] = fgetc(saveFile);
		if(saveBuffer[sb] == EOF) {
			break;
		}
		if(sb == sbSize) {
			sbSize *= 2;
			saveBuffer = realloc(saveBuffer, sbSize + 1);
		}
	}

	tokens = tokenize(saveBuffer, maxTokens, NULL);

	// Close save file
	fclose(saveFile);

	return tokens;
}

// INDEV
int parseSave(Save* save) {
	return 0;
}

// INDEV
int loadSave(Save* save) {
	return 0;
}
