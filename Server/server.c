#include "server.h"

// Global variables
short args = 0;             // Argument bitmap
Save save;                  // Save file data
int commands[6];            // Command queue
void** commandsArgs[6];     // Argument lists for commands
EM entityManager;           // Entity handle
bool noNextCommand = false; // Skip the printing of "\n$ "
bool exitFlag = false;      // Shutdown flag

// Control function Command interpreter (CTRL_FUNC__interpretCommand V2)
int CTRL_FUNC__interpretCommand(char buffer[]) {
	Token* tokens = tokenize(buffer, 8);
	int t = 8;

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
				for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
					free(tokens[i2].text);
				}
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

			long size = strlen(tokens[4].text) + 6;
			char* tempPATH = malloc(size * sizeof(char));
			strcpy(tempPATH, tokens[4].text);
			tempPATH[size - 6] = '.';
			tempPATH[size - 5] = 'i';
			tempPATH[size - 4] = 'n';
			tempPATH[size - 3] = 'f';
			tempPATH[size - 2] = 'o';
			tempPATH[size - 1] = '\0';

			size = 0;

			FILE* fptr = fopen(tempPATH, "r");
			if(fptr == NULL) {
				fprintf(stderr, "[Controller]: ERROR: Couldn't open file \"%s\".\n", tempPATH);
				for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
					free(tokens[i2].text);
				}
				free(tempPATH);
				free(commandsArgs[0][0]);
				free(commandsArgs[0][1]);
				free(commandsArgs[0][2]);
				free(commandsArgs[0][3]);
				free(commandsArgs[0][4]);
				free(commandsArgs[0][5]);
				free(commandsArgs[0]);
				return 0;
			}

			fseek(fptr, 0, SEEK_END);
			size = ftell(fptr);

			if(size < 0) {
				fprintf(stderr, "[Controller]: ERROR: File \"%s\" is empty.\n", tempPATH);
				fclose(fptr);
				free(tempPATH);
				free(commandsArgs[0][0]);
				free(commandsArgs[0][1]);
				free(commandsArgs[0][2]);
				free(commandsArgs[0][3]);
				free(commandsArgs[0][4]);
				free(commandsArgs[0][5]);
				free(commandsArgs[0]);
				for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
					free(tokens[i2].text);
				}
				return 0;
			}

			rewind(fptr);

			char* buf = malloc((size + 1) * sizeof(char));
			size_t bytes_read = fread(buf, 1, (size_t)size, fptr);
			fclose(fptr);
			buf[bytes_read] = '\0';
			free(tempPATH);

			Token* tempTOKENS = tokenize(buf, 1024);

			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_AI;
		}
		else if(strcmp("3d", tokens[2].text) == 0) {
			// INDEV
			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_3D;
		}
		else {
			fprintf(stderr, "[Controller]: ERROR: Invalid command.");
			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return 0; // Invalid command
		}
	}
	else {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}

		if(strcmp("start", tokens[0].text) == 0) {
			return CMI__CTRL_FUNC__START_SIMULATION;
		}
		else if(strcmp("kill", tokens[0].text) == 0) {
			return CMI__CTRL_FUNC__KILL_SIMULATION;
		}
		else if(strcmp("quit", tokens[0].text) == 0) {
			return CMI__CTRL_FUNC__QUIT;
		}
	}
	
	fprintf(stderr, "[Controller]: ERROR: No or Invalid command.");
	return 0; // No/Invalid command
}

// Control function
int controlFunction() {
	// Variables
	char buffer[257];
	buffer[256] = '\0';
	fd_set cmd, cmd_copy;
	FD_ZERO(&cmd);
	FD_SET(STDIN_FILENO, &cmd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	// Print first command prompt
	printf("\n$ ");
	fflush(stdout);

	// Command loop
	while(!exitFlag) {
		// Reset noNextCommand flag
		noNextCommand = false;

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
		}

		// Execute commands
		for(int i = 0; i < 6; i++) {
			switch(commands[i]) {
				// No command
				case CMI__CTRL_FUNC__NO_COMMAND: { if(i == 0) { noNextCommand = true; } } break;

				// Create AI
				case CMI__CTRL_FUNC__CREATE_AI: {
					EM__createEntity(
						&entityManager, (char*)commandsArgs[i][0], *((unsigned int*)commandsArgs[i][1]), (char*)commandsArgs[i][2],
						*((unsigned long long*)commandsArgs[i][3]), *((unsigned long long*)commandsArgs[i][4]), *((unsigned long long*)commandsArgs[i][5])
					);
					EM__setDataSection(&entityManager, entityManager.entityCount - 1, commandsArgs[i][6], commandsArgs[i][7]);
					for(int i2 = 0; i2 < 8; i2++) {
						free(commandsArgs[i][i2]);
					}
					free(commandsArgs[i]);
				} break;

				// INDEV
				// Import AI
				case CMI__CTRL_FUNC__IMPORT_AI: {} break;

				// INDEV
				// Export AI
				case CMI__CTRL_FUNC__EXPORT_AI: {} break;

				// Remove AI
				case CMI__CTRL_FUNC__REMOVE_AI: {
					EM__removeEntity(&entityManager, commandsArgs[i][0], false);
					free(commandsArgs[i][0]);
					free(commandsArgs[i]);
				} break;

				// INDEV
				// Create 3D Model
				case CMI__CTRL_FUNC__CREATE_3D: {} break;

				// INDEV
				// Import 3D Model
				case CMI__CTRL_FUNC__IMPORT_3D: {} break;

				// INDEV
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

				// Quit program
				case CMI__CTRL_FUNC__QUIT: { exitFlag = true; } break;
				default: {
					fprintf(stderr, "[Controller]: ERROR: Invalid command. Exit code: %d\n", CMI_ERROR__INVALID_COMMAND);
					return CMI_ERROR__INVALID_COMMAND;
				} break;
			}

			// Reset command buffer entry
			commands[i] = 0;
		}

		// Print command prompt
		if(!noNextCommand) {
			printf("\n$ ");
			fflush(stdout);
		}
		else if(exitFlag) {
			printf("\n");
			fflush(stdout);
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	//
	// Setup
	//
	printf("[Setup]: Interpreting arguments...\n");
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

	//
	// Main loop
	//
	int exitCode = controlFunction();

	//
	// Cleanup
	//
	return 0;
}
