#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LENGTH  3

#define EMPTY    0
#define CROSS    1
#define NOUGHT  -1
#define MAXCOOR (LENGTH - 1)
#define FIELDS  (LENGTH * LENGTH)
#define LINES   (LENGTH + LENGTH + 2)
#define COLS    ((FIELDS + 1) * (LENGTH + 1))

int turn = 0;
int board[FIELDS];

typedef struct _Rowinfo {
	int sum;
	int count;
	int field[LENGTH];
} Rowinfo;
Rowinfo row[LINES];

char scr[LENGTH][COLS];

void reset();
int whoseturn();
void givevalue(int*, int, int);
int rank(int);
void move(int);
void printboard();
int whowon();

int main()
{
	int i, j, k, wie;
	srand(time(NULL));

	for (i = 0; i < 100; ++i) {
		reset();
		while (turn < FIELDS) {
			move(rank(whoseturn())); // also turn++
			printboard();
			if ((wie = whowon())) {
				scr[LENGTH - 1][(FIELDS + 1) * (LENGTH + 1) - 2] = (wie == CROSS ? 'x' : 'o');
				turn = FIELDS;
			}
		}
		for (j = 0; j < LENGTH; ++j)
			printf("%s\n", scr[j]);
		printf("\n");
	}
	return 0;
}

void reset()
{
	int i, j;

	// clear board
	turn = 0;
	for (i = 0; i < FIELDS; ++i)
		board[i] = 0;

	// clear screen buffer
	for (i = 0; i < LENGTH; ++i) {
		for (j = 0; j < COLS - 1; ++j)
			scr[i][j] = ' ';
		scr[i][COLS - 1] = '\0';
	}

	for (i = 0; i < LINES; ++i) {
		row[i].sum = 0;
		row[i].count = 0;
		for (j = 0; j < LENGTH; ++j)
			row[i].field[j] = 0;
	}
	for (i = 0; i < LENGTH; ++i)
		for (j = 0; j < LENGTH; ++j)
			row[i].field[j] = j + i * LENGTH;
	for (i = 0; i < LENGTH; ++i)
		for (j = 0; j < LENGTH; ++j)
			row[i + LENGTH].field[j] = i + j * LENGTH;
	for (i = 0; i < LENGTH; ++i)
		row[LINES - 2].field[i] = i * (LENGTH + 1);
	for (i = 0; i < LENGTH; ++i)
		row[LINES - 1].field[i] = MAXCOOR + i * MAXCOOR;
}

int whoseturn()
{
	return (turn % 2) ? NOUGHT : CROSS;
}

void givevalue(int* a, int i, int v)
{
	for (int j = 0; j < LENGTH; ++j)
		a[row[i].field[j]] += v;
}

int rank(int sym)
{
	int i, j, n;
	int val[FIELDS];
	int opt[FIELDS];

	for (i = 0; i < FIELDS; ++i) {
		val[i] = 0;
		opt[i] = 0;
	}

	// own chances
	for (i = 0; i < LINES; ++i) {
		n = 1;
		for (j = 0; j < LENGTH; ++j) {
			if (j == MAXCOOR)
				n *= FIELDS; // extra importance for win in 1
			if (row[i].count == j && row[i].sum == sym * j)
				givevalue(&val, i, n);
			n *= FIELDS;
		}
	}
	// opponent chances
	for (i = 0; i < LINES; ++i) {
		n = 1;
		for (j = 1; j < LENGTH; ++j) {
			if (j == MAXCOOR)
				n *= FIELDS; // extra importance for lose in 1
			if (row[i].count == j && row[i].sum == -sym * j)
				givevalue(&val, i, n);
			n *= FIELDS;
		}
	}

	n = -1;
	j = 0;
	for (i = 0; i < FIELDS; ++i)
		if (board[i] == EMPTY)
			if (n == -1) {
				n = i;
				opt[j++] = i;
			} else if (val[i] == val[n]) {
				opt[j++] = i;
			} else if (val[i] > val[n]) {
				n = i;
				j = 0;
				opt[j++] = i;
			}
	if (j == 1)
		return opt[0];
	if (j > 1)
		return opt[rand() % j];
	return n;
}

void move(int field)
{
	int sym = whoseturn();
	++turn;

	// board update
	board[field] = sym;

	// row update
	int x = field % LENGTH;
	int y = field / LENGTH;

	// horizontal row
	row[y].sum += sym;
	row[y].count++;

	// vertical row
	row[x + LENGTH].sum += sym;
	row[x + LENGTH].count++;

	// diagonal 1
	if (x == y) {
		row[LINES - 2].sum += sym;
		row[LINES - 2].count++;
	}

	// diagonal 2
	if (x + y == MAXCOOR) {
		row[LINES - 1].sum += sym;
		row[LINES - 1].count++;
	}
}

void printboard()
{
	int x, y, i, j;
	for (y = MAXCOOR; y >= 0; --y) {
		i = MAXCOOR - y;
		for (x = 0; x <= MAXCOOR; ++x) {
			j = x + (turn - 1) * (LENGTH + 1);
			switch (board[x + y * LENGTH])
			{
				case CROSS: scr[i][j] = 'x'; break;
				case NOUGHT : scr[i][j] = 'o'; break;
				default     : scr[i][j] = '.'; break;
			}
		}
	}
}

int whowon()
{
	for (int i = 0; i < LINES; ++i)
		if (row[i].sum == CROSS * LENGTH)
			return CROSS;
		else if (row[i].sum == NOUGHT * LENGTH)
			return NOUGHT;
	return EMPTY;
}
