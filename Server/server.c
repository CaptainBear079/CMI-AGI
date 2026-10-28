#include "server.h"

// Global variables
short args = 0;   // Argument bitmap
Save save;        // Save file data
EM entityManager; // Entity handle

int main(int argc, char* argv[]) {
    //
    // Setup
    //
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

    //
    // Main loop
    //

    //
    // Cleanup
    //
    return 0;
}
