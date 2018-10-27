all: replace.c
	gcc -o replace replace.c -Wall

install:
	cp replace /usr/bin
