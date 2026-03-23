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

	if (argc > 2) {
		printf("Unformatted instructions:\n%s\n\n", unformattedInstructions);
	}

	// Read how many instructions
	int numInstructions = 1; 
	for (int i = 0; i < fileSize; i++) {
		if (unformattedInstructions[i] == '\n') { numInstructions++; }
	}

	short instructions[numInstructions];
	int instructionCounter = 0;
	int startPoint = 0;

	for (int i = 0; i <= fileSize; i++) {
		if (unformattedInstructions[i] == '\n' || unformattedInstructions[i] == 0) {
			int size = i - startPoint + 1;
			char buf[size];

			memcpy(buf, &unformattedInstructions[startPoint], i-startPoint);
			buf[i-startPoint] = 0;

			int dir = buf[0] == 'L' ? -1 : 1;
			int val = atoi(&buf[1]);
			instructions[instructionCounter++] = (dir * val) % 100;

			startPoint = i+1;
			i++;
		}
		
	}

	free(unformattedInstructions);

	if (argc > 2) {
		printf("numInstructions: %d\n", numInstructions);

		printf("Formatted Instructions:\n");

		for (int i = 0; i < numInstructions; i++) {
			printf("%3d\n", instructions[i]);
		}
		printf("\n");
	}


	// Perform the instructions
	int position = 50;
	long zeroCount = 0;

	if (argc > 2) {
		printf("Position of the dial: starting at %d\n", position);
	}

	for (int i = 0; i < numInstructions; i++) {
		position += instructions[i];
		position = position < 0 ? position + 100 : position % 100;
		if (position == 0) { zeroCount++; }

		if (argc > 2) {
			printf("After instruction %d: %3d: Zero count is %ld\n", i+1, position, zeroCount);
		}
	}

	printf("Zero count : %ld\n", zeroCount);
}