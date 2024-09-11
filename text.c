#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
	int fd, size;
	char buf[100];

	if((fd = open(".myshell", O_RDONLY)) < 0) {
		perror("file open error\n");
		exit(-1);
	}

	if((size = read(fd, buf, 99)) < 0) {
		perror("file read error\n");
		exit(-2);
	}

	buf[size] = '\0';
	printf("%s\n", buf);

	return 0;
}
