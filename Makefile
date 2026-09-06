CC = gcc
CFLAGS = -g -Wall -Wextra -fPIC -I.
LIB_NAME = libFast3D.so
LIB_SRC = ./Fast3D/Fast3D.c
NAME = cmi-agi
BUILD_DIR = ./build

all: $(LIB_NAME) $(APP_NAME)

$(LIB_NAME):
	$(CC) $(CFLAGS) -shared -o $@ $(LIB_SRC) -lm

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ main.c -D_W_X11 -D_GUI_SUPPORT -L. -lFast3D -lpthread -lX11 -lm

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean