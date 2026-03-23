//gcc -o solution solution.c -g -Wall -pedantic

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
	int row, col;
	char state;
} position;


int main(int argc, char* argv[]) { 
	// Get the input and store it as a character array
	// -------------------------------------------------------
	
	// Open the file
	int fd = open(argv[1], O_RDONLY);

	// Get the size
	off_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	// malloc the array
	char *unformattedGrid = malloc(size + 1);

	// Read in the input, then null terminate for ease of use
	int readChars = read(fd, unformattedGrid, size);
	unformattedGrid[readChars] = 0;

	if (argc > 2) {
		printf("Unformatted Grid:\n%s\n\n", unformattedGrid);
	}

	// Close the file
	close(fd);
	// -------------------------------------------------------
	


	// Format the grid into a 2d array
	// -------------------------------------------------------

	// Create the size and counter variables, as well as the start position
	int offset = 0;
	int rowStart = 0;
	int rowSize = 0;
	int rows = 1;
	int rowCounter = 0;
	int startColumn = 0;
	char **grid;

	// Get basic data from input
	while(unformattedGrid[offset] != 0) {
		// Number of rows and size of rows
		if (unformattedGrid[offset] == '\n') {
			rows++;
			if (rowSize == 0) { rowSize = offset; }
		}

		// Find the starting column
		if (unformattedGrid[offset] == 'S') {
			startColumn = offset;
		}
		offset++;
	}

	// Malloc the real grid
	grid = malloc(rows * sizeof(char*));
	for (int i = 0; i < rows; i++) {
		grid[i] = malloc(rowSize+1);
	}

	// Copy over all of the data to the 2d array
	rowStart = 0;
	offset = 0;
	while(unformattedGrid[offset] != 0) {
		if (unformattedGrid[offset] == '\n') {
			memcpy(grid[rowCounter], &unformattedGrid[rowStart], rowSize);
			grid[rowCounter++][rowSize] = 0;
			rowStart = offset+1;
		}

		offset++;
	}
	memcpy(grid[rowCounter], &unformattedGrid[rowStart], rowSize);
	grid[rowCounter++][rowSize] = 0;

	// Free the unformatted grid
	free(unformattedGrid);

	// Debug print the formatted grid
	if (argc > 2) {
		printf("Formatted Grid:\n");

		for (int i = 0; i < rows; i++) {
			printf("%s\n", grid[i]);
		}
		printf("\n");
	}

	// -------------------------------------------------------



	// Search for all of the splits
	// 		Core idea is to add splits to the stack
	//		Then if we have reached the bottom row 
	//		and there is nothing on the stack, we are done
	//		otherwise pop a new position off the stack
	//		and go down the other way. 
	//
	//		If a split has been visited, pop off of the stack
	// -------------------------------------------------------

	// Create the stack, we should never be exceeding the number of rows
	// Push the start space to the stack
	int top = 0;
	position stack[rows];
	stack[top++] = (position) {0, startColumn, 2};

	// Create a grid to store sums
	long sum[rows][rowSize];
	memset(sum, 0, sizeof(sum));

	int curRow = 0, curCol = startColumn;
	int numSplits = 0;

	// Main loop
	while (curRow < rows) {
		//printf("exploring (%2d, %2d): ", curRow, curCol);
		
		// Found a splitter
		if (grid[curRow][curCol] == '^') {
			// Mark splitter as visited
			grid[curRow][curCol] = '~';

			// Increment number of splits
			numSplits++;

			// Push splitter to stack
			stack[top++] = (position) {curRow, curCol, 0};
			//printf("pushed (%2d, %2d), size = %d: ", curRow, curCol, top);
			curCol--;
		}

		// Hit a splitter that we've already seen
		if (grid[curRow][curCol] == '~') {
			position pos = stack[--top];
			sum[pos.row][pos.col] += sum[curRow][curCol];

			if (top == 0) { break; }
			if (pos.state == 0) {
				//printf("trying to push (%2d, %2d) at splitter: ", pos.row, pos.col);
				stack[top++] = (position) {pos.row, pos.col, 1};
			} else {
				//printf("trying to go back up the tree from splitter:\n");
				while(top != 0 && pos.state == 1) {
					int tmpRow = pos.row, tmpCol = pos.col;
					pos = stack[--top];
					//printf("                    adding %3d to (%2d, %2d): \n", sum[tmpRow][tmpCol], pos.row, pos.col);
					sum[pos.row][pos.col] += sum[tmpRow][tmpCol];
				}
				if (pos.state == 2) { break; }
				//printf("                    pushing (%2d, %2d): ", pos.row, pos.col);
				stack[top++] = (position) {pos.row, pos.col, 1};
			}
			curRow = pos.row;
			curCol = pos.col+1;
			//printf("splitter popped to (%2d, %2d), size = %d: ", curRow, curCol, top);
			if (top == 0) { break; }
		}

		// Move down the grid
		grid[curRow++][curCol] = '|';

		// At the bottom of the grid
		if (curRow == rows && top != 0) {
			position pos = stack[--top];
			//printf("was bottom, added to (%2d, %2d): ", pos.row, pos.col);
			sum[pos.row][pos.col]++;

			if (top == 0) { break; }
			if (pos.state == 0) {
				//printf("trying to push (%2d, %2d) at bottom: ", pos.row, pos.col);
				stack[top++] = (position) {pos.row, pos.col, 1};
			} else {
				//printf("trying to go back up the tree from bottom:\n");
				while(top != 0 && pos.state == 1) {
					int tmpRow = pos.row, tmpCol = pos.col;
					pos = stack[--top];
					//printf("                    adding %3d to (%2d, %2d): \n", sum[tmpRow][tmpCol], pos.row, pos.col);
					sum[pos.row][pos.col] += sum[tmpRow][tmpCol];
				}
				if (pos.state == 2) { break; }
				//printf("                    pushing (%2d, %2d): ", pos.row, pos.col);
				stack[top++] = (position) {pos.row, pos.col, 1};
			}
			curRow = pos.row;
			curCol = pos.col+1;

			//printf("bottom popped to (%2d, %2d), size = %d: ", curRow, curCol, top);
		}

		//printf("\n");
	}

	// Debug print the formatted grid post splits
	if (argc > 2) {
		printf("\n\nExplored Grid:\n");

		for (int i = 0; i < rows; i++) {
			printf("%s\n", grid[i]);
		}
		printf("\n");

		// printf("Sums:\n");

		// for (int i = 0; i < rows; i++) {
		// 	for (int j = 0; j < rowSize; j++) {
		// 		if (grid[i][j] == '~') {
		// 			printf("%3ld ", sum[i][j]);
		// 		} else {
		// 			printf("    ");
		// 		}
		// 	}
		// 	printf("\n");
		// }
		// printf("\n");
	}

	// Find the timelines from the first splitter hit
	int search = 0;
	while (search < rows && grid[search][startColumn] != '~') { search++; }

	long timelines = search == rows ? 1 : sum[search][startColumn];

	// Print the number of splits found
	printf("Number of splits    : %d\n", numSplits);
	printf("Number of timelines : %ld\n", timelines);
	// -------------------------------------------------------


	// Free the formatted grid
	// -------------------------------------------------------
	for (int i = 0; i < rows; i++) {
		free(grid[i]);
	}
	free(grid);
	// -------------------------------------------------------
}