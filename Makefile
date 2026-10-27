CC = gcc
CFLAGS = -g -Wall -Wextra -fPIC -I.
NAME = cmi-agi
BUILD_DIR = ./build

all: $(NAME)

$(NAME): $(LIB_NAME)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/WM_X11.o ./WindowManager/X11/WindowManagerX11.c -D_W_X11 -D_WM_STD__X11 -lX11
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/EM.o ./EntityManager/EntityManager.c -lpthread
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/saveInterpreter.o ./SaveFileInterpreter/saveInterpreter.c -D_DEBUG
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ main.c $(BUILD_DIR)/WM_X11.o $(BUILD_DIR)/EM.o -D_W_X11 $(BUILD_DIR)/saveInterpreter.o -D_WM_STD__X11 -D_GUI_SUPPORT -L$(BUILD_DIR) -Wl,-rpath,'$$ORIGIN' -lFast3D -lpthread -lX11 -lm

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean