CC = gcc
CFLAGS = -g -Wall -Wextra
DESKTOP ?= X11
CLIENT = cmi-client
CLIENT_DEFINES = -D_W_$(DESKTOP) -D_WM_STD__$(DESKTOP) -D_GUI_SUPPORT
SERVER_DEFINES = -D_DEBUG
SERVER = cmi-server
BUILD_DIR = build
VP = Client/Viewport/viewport.c
WM = Client/WindowManager/$(DESKTOP)/WindowManager$(DESKTOP).c
EM = Server/EntityManager/EntityManager.c
SFI = Server/SaveFileInterpreter/saveInterpreter.c

all: create-build-dir build-util $(CLIENT) $(SERVER)

create-build-dir:
	mkdir -p $(BUILD_DIR)

$(CLIENT): build-util
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/WM_$(DESKTOP).o $(WM) $(CLIENT_DEFINES) -l$(DESKTOP)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/VP.o $(VP) $(CLIENT_DEFINES) -lFast3D
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ Client/client.c $(BUILD_DIR)/util.o $(BUILD_DIR)/WM_$(DESKTOP).o $(BUILD_DIR)/VP.o $(CLIENT_DEFINES) -lFast3D -lpthread -l$(DESKTOP) -lm

$(SERVER): build-util
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/EM.o $(EM) $(SERVER_DEFINES) -lpthread
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/SFI.o $(SFI) $(SERVER_DEFINES)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ Server/server.c $(BUILD_DIR)/util.o $(BUILD_DIR)/EM.o $(BUILD_DIR)/SFI.o $(SERVER_DEFINES) -lpthread -lFast3D

build-util:
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/util.o util/Tokenizer.c

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean