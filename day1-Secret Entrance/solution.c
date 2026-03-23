//gcc -o solution solution.c -g -Wall -pedantic

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	// Open the file to read
	int fd = open(argv[1], O_RDONLY);

	off_t fileSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	char *unformattedInstructions = malloc(fileSize + 1);

	read(fd, unformattedInstructions, fileSize);
	unformattedInstructions[fileSize] = 0;

	close(fd);



	int position = 50;
}