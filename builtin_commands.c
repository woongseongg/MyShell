/* 
 * Defining built-in command fuctions 
*/

#include "builtin_commands.h"

// Print help
int my_help() {
	printf("Wooseong's Shell\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n\n");

	printf("\tmy_cd\n");
	printf("\tmy_help\n");
	printf("\tmy_exit\n");
	printf("\tmy_pwd\n");
	printf("\tmy_echo\n");

	return 1;
}

// Change directory
int my_cd(char **path) {
	if(path[1] == NULL) {
		fprintf(stderr, "expected argument to \"cd\"\n");
	}
	else {
		if(chdir(path[1]) != 0) {	//chdir: change directory
			perror("my_cd");
		}
	}
	return 1;
}

// Exit
int my_exit() {
	exit(EXIT_SUCCESS);	//exit successfully
}

// Print Present Directory
int my_pwd() {
	char cwd[512];
	if(getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("%s\n", cwd);
	}
	else {
		perror("my_pwd");
	}
	return 1;
}

// Print text
int my_echo(char **msg) {
	for(int i=1;msg[i]!=NULL;i++) {
		printf("%s ", msg[i]);
	}
	printf("\n");
	return 1;
}

// Match command
int handle_builtin_command(char **cmd) {
	if(cmd[0] == NULL) {
		return -1;
	}

	if(!strcmp(cmd[0], "cd")) {
		return my_cd(cmd);
	}
	else if(!strcmp(cmd[0], "exit")) {
		return my_exit();
	}
	else if(!strcmp(cmd[0], "help")) {
		return my_help();
	}
	else if(!strcmp(cmd[0], "pwd")) {
		return my_pwd();
	}
	else if(!strcmp(cmd[0], "echo")) {
		return my_echo(cmd);
	}

	fprintf(stderr, "Unkown command: %s\n", cmd[0]);
	return -1;
}
