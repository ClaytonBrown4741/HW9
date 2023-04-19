# Makefile
# Author: Clayton Brown
# Date: April 11, 2023
#
# COSC 3750, Homework 8
#
# This is just the Makefile used to compile the executable file
# which is named wyshell

# Declare my variables
CC=gcc
CFLAGS=-Wall -ggdb

# Declare my targets and PHONY's
targets: wyshell
.PHONY: clean

# Compile the main executable
wyshell: wyshell.c wyscanner.c
	$(CC) $(CFLAGS) wyshell.c wyscanner.c -o wyshell

# Define the clean function
clean:
	/bin/rm -f "wyshell"
