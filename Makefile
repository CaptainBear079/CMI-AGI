CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I.
LDFLAGS = -shared
LIB_NAME = libFast3D.so
APP_NAME = out

all: $(LIB_NAME) $(APP_NAME)

$(LIB_NAME): Fast3D.c Fast3D.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ Fast3D.c -lm

$(APP_NAME): main.c Fast3D.h $(LIB_NAME)
	$(CC) $(CFLAGS) -g -o $@ main.c -D_USE_X11 -D_GUI_SUPPORT -L. -lFast3D -lpthread -lX11 -lm -Wl,-rpath,'$$ORIGIN'

clean:
	rm -f $(LIB_NAME) $(APP_NAME)

.PHONY: all clean