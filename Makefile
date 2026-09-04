CC = gcc
CFLAGS = -g -Wall -Wextra -fPIC -I.
LIB_NAME = libFast3D.so
NAME = cmi-agi
BUILD_DIR = ./build

all: $(LIB_NAME) $(APP_NAME)

$(LIB_NAME): Fast3D.c Fast3D.h
	$(CC) $(CFLAGS) -shared -o $@ Fast3D.c -lm

all: $(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ main.c -D_W_X11 -D_GUI_SUPPORT -L. -lFast3D -lpthread -lX11 -lm

clean:
	rm -f $(BUILD_DIR)/*

.PHONY: all clean