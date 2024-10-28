
/* Declare built-in command function */

#ifndef BUILTIN_COMMANDS_H
#define BUILTIN_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int my_help();
int my_cd(char **path);
int my_exit();
int my_pwd();
int my_echo(char **msg);
int handle_builtin_command(char **cmd);

#endif //BUILTIN_COMMANDS_H
