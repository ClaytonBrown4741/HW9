/*
* wyshell.c
* Author: Clayton Brown
* Date: Apr 11, 2023
*
* COSC 3750, Homework 8
*
* This code is used to simulate the experience of using
* a shell. At the moment, however, it doesn't actually
* execute any commands. It simply parses through them
* and looks for errors.
*
*/


#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>

#include"wyscanner.h"
#include"command.h"

int main(){
    // Define my variables
    char input[256];
    struct command currentCommand;
    char *tokens[]={ "QUOTE_ERROR", "ERROR_CHAR", "SYSTEM_ERROR",
                "EOL", "REDIR_OUT", "REDIR_IN", "APPEND_OUT",
                "REDIR_ERR", "APPEND_ERR", "REDIR_ERR_OUT", "SEMICOLON",
                "PIPE", "AMP", "WORD" };
    int commandChecker=0;
    int commandNeeded=1;
    int redirectInput=0;
    int redirectOutput=0;
    int redirectError=0;
    int appendOutput=0;
    int appendError=0;
    int errToOutput=0;
    int error=0;
    int symbolNeeded=0;
    int stringNeeded=1;
    int previousChar=0;

    // This while loop repeats until the user quits the shell
    while(1){
      // Reset all my variables everytime the loop repeats
      stringNeeded=0;
      commandNeeded=1;
      commandChecker=0;
      redirectInput=0;
      redirectOutput=0;
      redirectError=0;
      appendOutput=0;
      appendError=0;
      errToOutput=0;
      symbolNeeded=0;
      error=0;
      previousChar=0;
      strncpy(currentCommand.commandName, "", 200);

      // Get input fromt the user and store it
      printf("?> ");
      int testChecker=0;
      strncpy(input, "", 256);

 
      fgets(input, 256, stdin);

      // If the user is done with the input, then exit
      if (feof(stdin)){
        printf("\n");
        return 0;
      }

      testChecker=parse_line(input);
      // This loop will go through every entry in the input
      while (testChecker != EOL){
        previousChar=testChecker;
        switch(testChecker) {
          // This case handles strings, and decides if they are arguments or commands
          case WORD:
            if (symbolNeeded==1){
                fprintf(stderr, "Error: Expected a special key, not a string\n");
                error=1;
                break;                
            }
            if (stringNeeded==1){
                symbolNeeded=1;
            }
            if (commandChecker==0){
              printf(":--: %s\n",lexeme);
              commandChecker=1;
              commandNeeded=0;
              strncpy(currentCommand.commandName, lexeme, 200);
            }
            else{
              printf(" --: %s\n", lexeme);
            }
            stringNeeded=0;
            break;

          case AMP:
            if (commandChecker==0)
            {
                fprintf(stderr, "Error: & is not allowed before command\n");
                error=1;
                break;
            }
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not &\n");
                error=1;
                break;
            }
            printf(" &\n");
            break;

          // This case handles pipes
          case PIPE:
            if (commandChecker==0)
            {
                fprintf(stderr, "Error: Pipe is not allowed before command\n");
                error=1;
                break;
            }
            if (redirectOutput==1)
            {
                fprintf(stderr, "Error: Already redirected output\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not |\n");
                error=1;
                break;
            }
            stringNeeded=0;
            commandChecker=0;
            commandNeeded=1;
            redirectInput=1;
            redirectOutput=0;
            redirectError=0;
            appendOutput=0;
            appendError=0;
            errToOutput=0;
            error=0;
	    symbolNeeded=0;

            printf(" |  \n");
            commandChecker=0;
            break;


          // This case handles semicolons
          case SEMICOLON:
            if (commandChecker==0)
            {
                fprintf(stderr, "Error: Semicolon is not allowed before command\n");
                error=1;
                break;
            }
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not ;\n");
                error=1;
                break;
            }
            stringNeeded=0;
            commandChecker=0;
            commandNeeded=1;
            redirectInput=0;
            redirectOutput=0;
            redirectError=0;
            appendOutput=0;
            appendError=0;
            errToOutput=0;
            symbolNeeded=0;
            error=0;

            printf(" ;\n");
            commandChecker=0;
            break;


          // This case handles the redirection of input
          case REDIR_IN:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not <\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (redirectInput==0){
                redirectInput=1;
                stringNeeded=1;
                symbolNeeded=0;
                printf(" <\n");
            }
            else{
                fprintf(stderr, "Error: Already declared a redirection of input\n");
                error=1;
                break;
            }
            break;


          // This case handles the redirection of output
          case REDIR_OUT:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not >\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (redirectOutput==0){
                redirectOutput=1;
                stringNeeded=1;
                symbolNeeded=0;
                printf(" >\n");
            }
            else{
                fprintf(stderr, "Error: Already declared a redirection of output\n");
                error=1;
                break;
            }
            break;


          // This case handles the redirection of error output
          case REDIR_ERR_OUT:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not 2>&1\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (errToOutput==0){
                errToOutput=1;
                symbolNeeded=0;
                printf(" 2>&1\n");
            }
            else{
                fprintf(stderr, "Error: Already declared a redirection of error output\n");
                error=1;
                break;
            }
            break;


          // This case handles the redirection of errors
          case REDIR_ERR:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not 2>\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (redirectError==0){
                redirectError=1;
                stringNeeded=1;
                symbolNeeded=0;
                printf(" 2>\n");
            }
            else{
                fprintf(stderr, "Error: Already declared a redirection of error\n");
                error=1;
                break;
            }
            break;


          // This case handles appending to the output
          case APPEND_OUT:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not >>\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (appendOutput==0){
                appendOutput=1;
                stringNeeded=1;
                symbolNeeded=0;
                printf(" >>\n");
            }
            else{
                fprintf(stderr, "Error: Already declared an output append\n");
                error=1;
                break;
            }
            break;


          // This case handles appending to error
          case APPEND_ERR:
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: Was expecting argument, not 2>>\n");
                error=1;
                break;
            }
            if (commandNeeded==1)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                error=1;
                break;
            }
            if (appendError==0){
                appendError=1;
                stringNeeded=1;
                symbolNeeded=0;
                printf(" 2>>\n");
            }
            else{
                fprintf(stderr, "Error: Already declared an error append\n");
                error=1;
                break;
            }
            break;


          // This checks to see if it's an unknown character/error
          case ERROR_CHAR:
            printf("%d: %s\t =%d\n",testChecker,tokens[testChecker%96],error_char);
            error=1;
            break;
          case QUOTE_ERROR:
            error=1;
            fprintf(stderr, "Error: Error reading in quotes\n");
            break;
          case SYSTEM_ERROR:
            fprintf(stderr, "SYSTEM ERROR\n");
            return 0;
          default:
            printf("%d: %s\n",testChecker,tokens[testChecker%96]);
        }
 
        // If there was an error, quit without printing EOL
        if (error==1)
            break;
        testChecker=parse_line(NULL);
        if ((testChecker!=EOL && previousChar==AMP) 
                        && (testChecker!=SEMICOLON && previousChar==AMP)){
            fprintf(stderr, "Error: & must come last in command\n");
	    break;
        }
        // If it's the end of the string, make sure there's no keys without arguments
        if (testChecker == EOL){
            if (stringNeeded==1)
            {
                fprintf(stderr, "Error: No argument given for special key\n");
                break;
            }
            if (commandNeeded==1 && previousChar != SEMICOLON)
            {
                fprintf(stderr, "Error: Command is needed.\n");
                break;
            }
          printf(" --: EOL\n");
        }
      }
    }
}
