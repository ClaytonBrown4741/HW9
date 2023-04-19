/*
* command.h
* Author: Clayton Brown
* Date: Apr 11, 2023
*
* COSC 3750, Homework 8
*
* This code is a structure that records information about the different
* commands that are encountered in the wyshell.c file.
*
*/

#ifndef COMMAND_
#define COMMAND__

struct command
{
   char commandName[200];
   char arguments[15][100];
   int inputFD;
   int outputFD;
   int errorFD;
   char *in_file;
   char *out_file;
   char *err_file;
};

#endif

