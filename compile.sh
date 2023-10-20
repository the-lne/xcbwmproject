#!/bin/bash
gcc -Wall main.c -o exec `pkg-config --cflags --libs xcb`
