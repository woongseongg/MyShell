#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	if(access(argv[1], F_OK) != 0) 
		printf("File is not exist.\n");
	else
		printf("File is exist.\n");
}
