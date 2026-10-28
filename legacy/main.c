#include "main.h"

Save save;                              // Save file data
bool noNextCommand = false;             // Skip the printing of "\n$ "

// 3D environment_thread
void* env_thread(void* arg) {
	init_cube();
	Fast3D__addMesh(renderer, &cube);
}

// Interpret command
int CTRL_FUNC__interpretCommand(char buffer[]) {
	Token tokens[8];
	int i = 0;
	int bi = 0;
	int t = 0;
	char buf[257];
	bool str = false;
	buf[256] = '\0';
	while(t < 8) {
		if(str) {
			if(bi != 0 && (buffer[i] == '\n' || buffer[i] == '\0')) {
				buf[bi] = '\0';
				tokens[t].text = malloc((i + 1) * sizeof(char));
				strcpy(tokens[t].text, buf);
				t++;
				break;
			}
			else if(buffer[i] == '\"') {
				str = false;
			}
			else {
				buf[bi] = buffer[i];
				bi++;
			}
		}
		else {
			if(bi != 0 && (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\0')) {
				buf[bi] = '\0';
				tokens[t].text = malloc((i + 1) * sizeof(char));
				strcpy(tokens[t].text, buf);
				if(!(buffer[i] == ' ')) {
					break;
				}
				t++;
				bi = 0;
			}
			else if(buffer[i] == '\"') {
				str = true;
			}
			else {
				buf[bi] = buffer[i];
				bi++;
			}
		}
		i++;
	}

	if(strcmp("create", tokens[0].text) == 0) {
		if(strcmp("ai", tokens[1].text) == 0) {
			commandsArgs[0] = malloc(7 * sizeof(void**));
			commandsArgs[0][0] = malloc((strlen(tokens[2].text) + 1) * sizeof(char));
			strcpy((char*)commandsArgs[0][0], tokens[2].text);

			commandsArgs[0][1] = malloc(sizeof(unsigned int));
			if(strcmp("cplugin", tokens[3].text) == 0) {
				(*(unsigned int*)commandsArgs[0][1]) = EM_ENTITY__TYPE_CPLUGIN;
			}
			else if(strcmp("engine", tokens[3].text) == 0) {
				(*(unsigned int*)commandsArgs[0][1]) = EM_ENTITY__TYPE_ENTITY_ENGINE;
			}
			else {
				fprintf(stderr, "[Controller] ERROR: Invalid command.\n");
				return 0;
			}

			commandsArgs[0][2] = malloc((strlen(tokens[4].text) + 1) * sizeof(char));
			strcpy((char*)commandsArgs[0][2], tokens[4].text);

			commandsArgs[0][3] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][3]) = atoll(tokens[5].text);

			commandsArgs[0][4] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][4]) = atoll(tokens[6].text);

			commandsArgs[0][5] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][5]) = atoll(tokens[7].text);

			int stringlen = strlen(tokens[4].text) + 6;
			char* string = malloc(stringlen * sizeof(char));
			strcpy(string, tokens[4].text);
			string[stringlen - 6] = '.';
			string[stringlen - 5] = 'i';
			string[stringlen - 4] = 'n';
			string[stringlen - 3] = 'f';
			string[stringlen - 2] = 'o';
			string[stringlen - 1] = '\0';
			FILE* fptr = fopen(string, "r");
			if(fptr == NULL) {
				fprintf(stderr, "[Controller] ERROR: The .info file for \"%s\" couldn't be opened.\n", tokens[4].text);
				return -1;
			}
			commandsArgs[0][6] = NULL;
			int size = 0;
			int tsize = 0;
			char infoBuffer[257];
			infoBuffer[256] = '\0';
			char c;
			bool nl;
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

			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_AI;
		}
		else if(strcmp("3d", tokens[1].text) == 0) {
			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_3D;
		}
	}
	else if(strcmp("start", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__START_SIMULATION;
	}
	else if(strcmp("kill", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__KILL_SIMULATION;
	}
	else if(strcmp("quit", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__QUIT;
	}
	return 0;
}

// Control function Command interpreter (CTRL_FUNC__interpretCommand V2)
int CTRL_FUNC__interpretCommand(char buffer[]) {
	return -1; // No/Invalid command
}

// Control function
int control_function() {
	// Variables
	char buffer[257];
	buffer[256] = '\0';
	fd_set cmd, cmd_copy;
	FD_ZERO(&cmd);
	FD_SET(STDIN_FILENO, &cmd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;
	// Command interpretion
	printf("\n$ ");
	fflush(stdout);
	while(!exitFlag) {
		// Reset buffer
		for(int i = 0; i < 256; i++) {
			buffer[i] = '\0';
		}

		// Check for stdin input
		cmd_copy = cmd;
		if(select(STDIN_FILENO + 1, &cmd_copy, NULL, NULL, &timeout) < 0) {
			fprintf(stderr, "[Controller] Error while waiting for input from stdin.\n");
			return 1;
		}

		// Read input
		if(FD_ISSET(STDIN_FILENO, &cmd_copy)) {
			fgets(buffer, sizeof(buffer), stdin);

			// Interpret command
			commands[0] = CTRL_FUNC__interpretCommand(buffer);
			if(commands[0] == -1) {
				return -1;
			}
		}

		// Check for and run commands (both stdin and control_thread)
		for(int i = 0; i < 6; i++) {
			noNextCommand = false;
			switch(commands[i]){
				// No command
				case CMI__CTRL_FUNC__NO_COMMAND: {
					noNextCommand = true;
				} break;

				// Create AI
				case CMI__CTRL_FUNC__CREATE_AI: {
					EM__createEntity(
						&entityManager, entityManager.entityCount,
						(char*)commandsArgs[i][0], *((unsigned int*)commandsArgs[i][1]), (char*)commandsArgs[i][2],
						*((unsigned long long*)commandsArgs[i][3]), *((unsigned long long*)commandsArgs[i][4]), *((unsigned long long*)commandsArgs[i][5])
					);
					EM__setDataSection(&entityManager, entityManager.entityCount - 1, commandsArgs[i][6]);
					for(int i2 = 0; i2 < 8; i2++) {
						free(commandsArgs[i][i2]);
					}
				} break;

				// Import AI
				case CMI__CTRL_FUNC__IMPORT_AI: {} break;

				// Export AI
				case CMI__CTRL_FUNC__EXPORT_AI: {} break;

				// Remove AI
				case CMI__CTRL_FUNC__REMOVE_AI: {} break;

				// Create 3D Model
				case CMI__CTRL_FUNC__CREATE_3D: {} break;

				// Import 3D Model
				case CMI__CTRL_FUNC__IMPORT_3D: {} break;

				// Export 3D Model
				case CMI__CTRL_FUNC__EXPORT_3D: {} break;

				// Remove 3D Model
				case CMI__CTRL_FUNC__REMOVE_3D: {} break;

				// Start Simulation
				case CMI__CTRL_FUNC__START_SIMULATION: {
					printf("[Controller] Starting simulation...");
					fflush(stdout);
					EM__startSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Kill Simulation
				case CMI__CTRL_FUNC__KILL_SIMULATION: {
					printf("[Controller] Ending simulation...");
					fflush(stdout);
					EM__quitSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Quit
				case CMI__CTRL_FUNC__QUIT: {
					noNextCommand = true;
					printf("\n");
					return 0;
				} break;
				default: { printf("Invalid command.\n"); } break;
			}
			if(!noNextCommand) {
				printf("\n$ ");
				fflush(stdout);
			}
			commands[i] = 0;
		}
	}
	printf("\n");
	return 1; // Unexpected error, exitFlag set by other thread
}

// Control thread
int main(int argc, char* argv[]) {
	printf("[Setup]: Interpreting argumentss...\n");
	// - Interpreting arguments
	for(int i = 1; i < argc; i++) {
		if(strcmp("--restore", argv[i]) == 0) {
			args = args | CMI__ARG_RESTORE_SESSION;
		}
	}
	if(args & CMI__ARG_RESTORE_SESSION) {
		loadSave(&save); // INDEV
	}

	printf("[Setup]: Preparing entity manager...\n");
	// - Prepare entity manager
	EM__init(&entityManager, (args & CMI__ARG_GUI_MODE));
}
