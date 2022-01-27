#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// ANSI escape sequences
#define HOME  "\e[H"
#define CLEAR "\e[2J"
#define HIDECUR "\e[?25l"
#define SHOWCUR "\e[?25h"

#define NEWLINE "\n"

#define MINSIZE    3
#define MAXSIZE   32
#define HISTORY   50
#define DELAY   1000

static uint32_t board[HISTORY][MAXSIZE];
static int width, height;

int  getbit     (uint32_t *, int, int);
void setbit     (int, int);
void flpbit     (int, int);
int  neighbours (uint32_t *, int, int);
int  population (void);
void clearboard (void);
void printboard (int, int);
void seedboard  (int);
int  evolve     (void);
int  str2int    (char *, int, int);

// Get bit value from board a at position (x,y)
int getbit(uint32_t *a, int x, int y)
{
	while (x < 0)
		x += width;
	x %= width;
	while (y < 0)
		y += height;
	y %= height;
	return (a[y] & ((uint32_t)1 << x) ? 1 : 0);
}

// Set bit on board 0 at position (x,y)
void setbit(int x, int y)
{
	uint32_t *a = board[0];
	a[y] |= ((uint32_t)1 << x);
}

// Flip bit on board 0 at position (x,y)
void flpbit(int x, int y)
{
	uint32_t *a = board[0];
	a[y] ^= ((uint32_t)1 << x);
}

// Count neighbours on board a at position (x,y)
int neighbours(uint32_t *a, int x, int y)
{
	int i, j, sum = 0;
	for (j = -1; j <= 1; ++j)      // row offset
		for (i = -1; i <= 1; ++i)  // col offset
			if (i || j)            // don't count self
				sum += getbit(a, x + i, y + j);
	return sum;
}

// Count population of board 0
int population(void)
{
	uint32_t row, *a = board[0];
	int i, j, sum = 0;

	for (j = 0; j < height; ++j) {     // every row
		row = a[j];                    // copy row value
		for (i = 0; i < width; ++i) {  // every column (can't use while: width might be smaller than MAXSIZE)
			sum += row & 1;            // check bit 0
			row >>= 1;                 // shift row value down
		}
	}
	return sum;
}

// Reset board 0
void clearboard(void)
{
	uint32_t *a = board[0];
	for (int i = 0; i < MAXSIZE; ++i)
		a[i] = 0;
}

void printboard(int generation, int population)
{
	static char ruler[MAXSIZE * 2];
	uint32_t *a = board[0];
	int i, j;
	char *pch;

	if (!generation) {
		pch = ruler;
		i = width * 2;
		while (--i)
			*pch++ = '-';
		*pch = '\0';
	}

	printf(HOME "gen%5u pop%4u" NEWLINE, generation, population);
	puts(ruler);
	for (j = 0; j < height; ++j)
		for (i = 0; i < width; ++i)
			printf("%c%s", getbit(a, i, j) ? 'X' : ' ', i == width - 1 ? NEWLINE : " ");
	puts(ruler);
}

void seedboard(int nseeds)
{
	uint32_t *a = board[0];
	int i, j, x1, y1, b1, x2, y2, b2, area = width * height;

	clearboard();
	// Test pattern
	/*
	a[1] = 8+16+32;
	a[3] = 2+128;
	a[4] = 2+128;
	a[5] = 2+128;
	a[7] = 8+16+32;
	*/
	for (i = 0; i < nseeds; ++i) {
		x1 = i % width;
		y1 = i / width;
		setbit(x1, y1);
	}
	for (i = 0; i < area; ++i) {
		j = rand() % (area - 1);
		if (j >= i)
			++j;  // random location other than i
		x1 = i % width;
		y1 = i / width;
		x2 = j % width;
		y2 = j / width;
		b1 = getbit(a, x1, y1);
		b2 = getbit(a, x2, y2);
		if (b1 != b2) {
			flpbit(x1, y1);
			flpbit(x2, y2);
		}
	}
}

int evolve(void)
{
	uint32_t *a = board[0], *b = board[1];
	int i = HISTORY, j, cur, sum;

	// shift history up, make room for new board at board[0]
	while (--i)
		for (j = 0; j < height; ++j)
			board[i][j] = board[i - 1][j];

	// next generation at board[0] from board[1]
	clearboard();
	for (j = 0; j < height; ++j)
		for (i = 0; i < width; ++i) {
			cur = getbit(b, i, j);
			sum = neighbours(b, i, j);
			if (sum == 3 || (sum == 2 && cur))
				setbit(i, j);
		}

	// same pattern in history?
	i = 0;
	while (++i < HISTORY) {
		b = board[i];
		j = 0;
		while (j < height && a[j] == b[j])
			++j;
		if (j == height)
			return i;
	}
	return 0;  // nope
}

int str2int(char *s, int min, int max)
{
	int n = atoi(s);
	if (n < min)
		n = min;
	if (n > max)
		n = max;
	return n;
}

int main(int argc, char *argv[])
{
	int i, ini, pop, gen = 0, rep = 0, delay = 10;
	uint32_t avg = 0;
	clock_t t;
	srand((unsigned int)time(NULL));

	if (argc < 4 || argc > 5 || !argv) {
		puts("Game of Life - Ewoud Dronkert 2019");
		printf("Usage: %s <width %u..%u> <height %u..%u> <initial occupation 0..100> [delay 0..100]\n", argv && argv[0] ? argv[0] : "gameoflife", MINSIZE, MAXSIZE, MINSIZE, MAXSIZE);
		return 1;
	}

	// clear screen, hide cursor
	printf(CLEAR HIDECUR);
	fflush(stdout);

	// Initialise main parameters from command line arguments
	width = str2int(argv[1], MINSIZE, MAXSIZE);
	height = str2int(argv[2], MINSIZE, MAXSIZE);
	avg = ini = pop = str2int(argv[3], 0, 100) * width * height / 100;
	if (argc == 5)
		delay = str2int(argv[4], 0, 100);
	delay *= DELAY;

	seedboard(ini);              // randomised initial population of size 'ini'
	printboard(0, ini);          // show initial situation

	t = clock();                 // 1 s delay to see the initial pattern
	while (clock() - t < 1000000);

	while (pop && !rep) {
		rep = evolve();          // generate next pattern and look for repeating patterns
		pop = population();      // calculate new population size
		avg += pop;
		printboard(++gen, pop);  // show current situation
		if (delay) {
			t = clock();         // delay to see the animation
			while (clock() - t < delay);
		}
	}

	// Summary
	printf("Size of the world  : %4i\n", width * height);
	printf("Initial population : %4i\n", ini);
	printf("Average population : %6.1f\n", (float)avg/(gen + 1));
	printf("Current population : %4i\n", pop);
	printf("Generations        : %4i\n", gen);
	if (!pop)
		puts("All life has ceased to exist.");
	else if (rep == 1)
		puts("Static pattern, evolutionary dead end.");
	else
		printf("Pattern repeating every %i generations.\n", rep);

	// show cursor
	printf(SHOWCUR);
	fflush(stdout);

	return 0;
}
