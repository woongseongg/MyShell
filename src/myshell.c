/* 
 * Main Shell Program 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/builtin_commands.h"

#define BUFFER_SIZ 512 
#define MAX_PATH 256 
#define MAX_TOKEN 512

char cmd_for_builtin[10];

void sigint_handler(int sig) {
	printf("\nsighandler occurred\n\n");
}

void load_environment(char *pathPtr[], int *pathCount) {
	int fd;
	char buffer[BUFFER_SIZ];
	char *path_start, *tok;
	ssize_t nread;

	if((fd = open("config/.myshell", O_RDONLY)) < 0) {
		perror("Failed to open .myshell\n");
		exit(-1);
	}

	if((nread = read(fd, buffer, BUFFER_SIZ - 1)) < 0) {
		perror("Failed to read .myshell\n");
		close(fd);
		exit(-1);
	}
	close(fd);
	
	buffer[nread] = '\0';
	printf("Read paths: %s\n", buffer);

	path_start = strstr(buffer, "PATH=");
	if(!path_start) {
		fprintf(stderr, "Invalid .myshell format\n");
		exit(-2);
	}
	path_start += 5;

	tok = strtok(path_start, ":");
	*pathCount = 0;

	while(tok && *pathCount < MAX_PATH) {
		if(strlen(tok) > 0) {
			pathPtr[*pathCount] = strdup(tok);
			if(!pathPtr[*pathCount]) {
				perror("Failed to allocate memory");
				exit(-3);
			}
			printf("Path added: %s\n", pathPtr[*pathCount]);
			(*pathCount)++;
		}
		tok = strtok(NULL, ":");
	}
}

int extract_command(char *cmd, char *token[], int *token_index) {
	char *start = cmd;
	char *token_start;
	int len = 0;
	*token_index = 0;

	memset(cmd_for_builtin, 0x00, 10);
	while(*start) {
		while(*start == ' ' || *start == '\t') {
			start++;
		}

		if(*start) {
			token_start = start;
			while(*start != ' ' && *start != '\t' && *start != '\0') {
				start++;
			}
			
			len = start - token_start;
			token[*token_index] = (char*)malloc(len + 1);
			if(!token[*token_index]) {
				perror("Failed to allocate memory\n");
				exit(-3);
			}
			strncpy(token[*token_index], token_start, len);
			token[*token_index][len] = '\0';
			(*token_index)++;
		 }
	}
	token[*token_index] = NULL;
	strcpy(cmd_for_builtin, token[(*token_index) - 1]);
	if(*token_index > 0) 
		return *token_index;		
	else
		return -1;		//if return -1, it means *token_index <= 0 and it is the failure
}

int find_command(char *cmd, char *pathPtr[], int pathCount, char *fullPath) {
	for(int i=0;i<pathCount;i++) {
		snprintf(fullPath, MAX_PATH, "%s/%s", pathPtr[i], cmd);
		if(access(fullPath, X_OK) == 0) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	char *pathPtr[MAX_PATH], *token[MAX_PATH], fullPath[BUFFER_SIZ];
	char line[BUFFER_SIZ];
	int pid, status, pathCount = 0, token_index = 0;

	signal(SIGINT, sigint_handler);

	load_environment(pathPtr, &pathCount);

	while(1) {
		printf("wooseong %% ");
		fflush(stdout);

		if(fgets(line, BUFFER_SIZ, stdin) == NULL) {
			if(feof(stdin)) {
				break;
			}
			if(ferror(stdin)) {
				perror("Error reading input");
				clearerr(stdin);
				continue;
			}
		}

		//remove the '\n'
		line[strlen(line) - 1] = '\0';

		if(!extract_command(line, token, &token_index))	{
			continue;
		}

		if(handle_builtin_command(token) != -1) {
			continue;
		}

		if(find_command(token[0], pathPtr, pathCount, fullPath)) {
			if((pid = fork()) == 0) {
				execv(fullPath, token);
				perror("execv failed\n");
				exit(-2);
			}
			else if(pid > 0) {
				wait(&status);
				printf("\n\tChild process termintaed\n\n");
			}
			else {
				perror("fork error\n");
			}
		}

		for(int i=0;i<token_index;i++) {
			free(token[i]);
		}
	}
	for(int i=0;i<pathCount;i++) {
		free(pathPtr[i]);
	}

	return 0;
}
