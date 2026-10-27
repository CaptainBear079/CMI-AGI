CC = gcc
CFLAGS = -g -Wall -Wextra -fPIC -I.
MODEL_NAME = dev-test-v0.0.1.so
MODEL_SRC = ./AIModels/dev-test-v0.0.1/dev-test-v0.0.1.c
NAME = cmi-agi
BUILD_DIR = ./build


all: $(NAME)

$(MODEL_NAME):
	$(CC) $(CFLAGS) -shared -o $(BUILD_DIR)/$@ $(MODEL_SRC) -lm
	cp $(MODEL_NAME).info ./build/

$(NAME): $(LIB_NAME)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/WM_X11.o ./WindowManager/X11/WindowManagerX11.c -D_W_X11 -D_WM_STD__X11 -lX11
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/EM.o ./EntityManager/EntityManager.c -lpthread
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/saveInterpreter.o ./SaveFileInterpreter/saveInterpreter.c -D_DEBUG
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ main.c $(BUILD_DIR)/WM_X11.o $(BUILD_DIR)/EM.o -D_W_X11 $(BUILD_DIR)/saveInterpreter.o -D_WM_STD__X11 -D_GUI_SUPPORT -L$(BUILD_DIR) -Wl,-rpath,'$$ORIGIN' -lFast3D -lpthread -lX11 -lm

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean