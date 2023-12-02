#!/bin/bash

i686-w64-mingw32-gcc -c file_searcher.c -o file_searcher.x86.o
x86_64-w64-mingw32-gcc -c file_searcher.c -o file_searcher.x64.o