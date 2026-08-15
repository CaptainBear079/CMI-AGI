all:
	gcc -g -o out main.c -D_USE_X11 -D_GUI_SUPPORT -lpthread -lX11