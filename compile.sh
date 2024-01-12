#!/bin/bash
gcc -Wall main.c -o exec `pkg-config --cflags --libs xcb`

#gcc `pkg-config --cflags gtk+-3.0` -o example-0 example-0.c `pkg-config --libs gtk+-3.0`
#include <gtk/gtk.h>