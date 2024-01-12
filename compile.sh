#!/bin/bash
gcc `pkg-config --cflags xcb gtk+-3.0 glib-2.0` -o exec main.c `pkg-config --libs xcb gtk+-3.0 glib-2.0`