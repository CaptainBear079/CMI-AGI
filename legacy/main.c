// Interpret command
int CTRL_FUNC__interpretCommand(char buffer[]) {
	int i = 0;
	int bi = 0;
	char buf[257];
	bool str = false;
	buf[256] = '\0';
	if(strcmp("create", tokens[0].text) == 0) {
		if(strcmp("ai", tokens[1].text) == 0) {
			while(true) {
				for(int ci = 0; ci < 5; ci++) {
					infoBuffer[ci] = fgetc(fptr);
					infoBuffer[ci + 1] = '\0';
					if(infoBuffer[ci] == '/') {
						infoBuffer[ci] = '\0';
						break;
					}
					else if(infoBuffer[ci] == '\n' || infoBuffer[ci] == EOF) {
						fprintf(stderr, "[Controller] ERROR: Uncomplete line in \"%s\".\n", string);
						return -1;
					}
				}
				if(strcmp(infoBuffer, "*") == 0) {
					tsize = sizeof(void*);
					size += tsize;
					if(commandsArgs[0][6] == NULL) {
						commandsArgs[0][6] = malloc(size);
					}
					else {
						realloc(commandsArgs[0][6], size);
					}
				}
				else if(strcmp(infoBuffer, "int") == 0) {
					tsize = sizeof(int);
					size += tsize;
					if(commandsArgs[0][6] == NULL) {
						commandsArgs[0][6] = malloc(size);
					}
					else {
						realloc(commandsArgs[0][6], size);
					}
				}
				else {
					fprintf(stderr, "[Controller] ERROR: Invalid or unsupported type in \"%s\".\n", string);
					return -1;
				}
				while(true) {
					c = fgetc(fptr);
					if(c == ':') {
						break;
					}
					if(c == '\n' || c == EOF) {
						fprintf(stderr, "[Controller] ERROR: Uncomplete line in \"%s\".\n", string);
						return -1;
					}
				}
				for(int ci = 0; ci < 7; ci++) {
					infoBuffer[ci] = fgetc(fptr);
					infoBuffer[ci + 1] = '\0';
					if(infoBuffer[ci] == '\n' || infoBuffer[ci] == '#') {
						if(infoBuffer[ci] == '\n') {
							nl = true;
						}
						infoBuffer[ci] = '\0';
						break;
					}
					else if(infoBuffer[ci] == EOF) {
						fprintf(stderr, "[Controller] ERROR: Uncomplete line in \"%s\".\n", string);
						return -1;
					}
				}
				if(strcmp(infoBuffer, "NULL") == 0) {
					*((void**)commandsArgs[0][6] + size - tsize) = NULL;
				}
				else if(strcmp(infoBuffer, "stdin") == 0) {
					if(!nl) {
						for(int ci = 0; ci < 256; ci++) {
							infoBuffer[ci] = fgetc(fptr);
							infoBuffer[ci + 1] = '\0';
							if(infoBuffer[ci] == '\n') {
								infoBuffer[ci] = '\0';
								break;
							}
							else if(infoBuffer[ci] == EOF) {
								c = EOF;
								break;
							}
						}
						printf("[Controller] Input data of %ld symbols: %s\n", size / sizeof(char), infoBuffer);
					}
					*((void**)commandsArgs[0][6] + size - tsize) = fgets(infoBuffer, size / sizeof(char), stdin);
				}
				else {
					fprintf(stderr, "[Controller] ERROR: Invalid or unsupported value in \"%s\".\n", string);
					return -1;
				}
				nl = false;
				if(c == EOF) {
					goto REACHED_EOF;
				}
				else if((c = fgetc(fptr)) == EOF) {
					goto REACHED_EOF;
				}
			}
			REACHED_EOF:
		}
	}
}
