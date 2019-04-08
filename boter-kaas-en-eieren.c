#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEEG    0
#define KRUISJE 1
#define RONDJE -1
#define MAXCOOR 2  // LENGTE - 1
#define LENGTE  3
#define VAKKEN  9  // LENGTE * LENGTE

int beurt = 0;
int bord[VAKKEN] = {0,0,0,0,0,0,0,0,0};

typedef struct _Rijinfo {
	int som;
	int aantal;
	int vak[3];
} Rijinfo;
Rijinfo rij[8];

#define COLS 71
char scr[LENGTE][COLS + 1];

void reset();
int wieisaanzet();
void waarderen(int*, int, int);
int meestepunten(int);
void zet(int);
void printbord(int);
int gewonnen();

int main()
{
	int i, j, k, wie;
	srand(time(NULL));

	for (i = 0; i < 100; ++i) {
		// clear screen buffer
		for (j = 0; j < LENGTE; ++j) {
			for (k = 0; k < COLS; ++k)
				scr[j][k] = ' ';
			scr[j][COLS] = '\0';
		}
		for (j = 0; j < 2; ++j) {
			k = j * 9;
			reset();
			while (beurt < VAKKEN) {
				zet(meestepunten(wieisaanzet()));
				printbord(k++);
				if (gewonnen())
					beurt = VAKKEN;
			}
		}
		for (j = 0; j < LENGTE; ++j)
			printf("%s\n", scr[j]);
		printf("\n");
	}
	return 0;
}

void reset()
{
	int i, j;

	beurt = 0;
	for (i = 0; i < VAKKEN; ++i)
		bord[i] = 0;

	for (i = 0; i < 8; ++i)
		rij[i] = (Rijinfo){0,0,{0,0,0}};
	for (i = 0; i < LENGTE; ++i)
		for (j = 0; j < LENGTE; ++j)
			rij[i].vak[j] = j + i * LENGTE;
	for (i = 0; i < LENGTE; ++i)
		for (j = 0; j < LENGTE; ++j)
			rij[i + LENGTE].vak[j] = i + j * LENGTE;
	for (i = 0; i < LENGTE; ++i)
		rij[6].vak[i] = i * 4;
	for (i = 0; i < LENGTE; ++i)
		rij[7].vak[i] = 2 + i * 2;
}

int wieisaanzet()
{
	return (beurt % 2) ? RONDJE : KRUISJE;
}

void waarderen(int* a, int i, int v)
{
	for (int j = 0; j < LENGTE; ++j)
		a[rij[i].vak[j]] += v;
}

int meestepunten(int sym)
{
	int i, j, n;
	int val[VAKKEN] = {0,0,0,0,0,0,0,0,0};
	int opt[VAKKEN] = {0,0,0,0,0,0,0,0,0};

	for (i = 0; i < 8; ++i)
		if (rij[i].som == sym * MAXCOOR)
			waarderen(&val, i, 1000);  // 2 eigen team + 1 leeg
		else if (rij[i].som == -sym * MAXCOOR)
			waarderen(&val, i, 100);   // 2 tegenstanders + 1 leeg
		else if (rij[i].som == sym && rij[i].aantal == 1)
			waarderen(&val, i, 20);    // 1 eigen team + 2 leeg
		else if (rij[i].aantal == 0)
			waarderen(&val, i, 2);     // 3 leeg
		else if (rij[i].som == -sym && rij[i].aantal == 1)
			waarderen(&val, i, 1);     // 1 tegenstander + 2 leeg

	n = -1;
	j = 0;
	for (i = 0; i < VAKKEN; ++i)
		if (bord[i] == LEEG)
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

void zet(int vak)
{
	int sym = wieisaanzet();
	++beurt;

	// bord bijwerken
	bord[vak] = sym;

	// rij bijwerken
	int x = vak % LENGTE;
	int y = vak / LENGTE;

	// horizontale rij
	rij[y].som += sym;
	rij[y].aantal++;

	// vertikale rij
	rij[x + LENGTE].som += sym;
	rij[x + LENGTE].aantal++;

	// schuine rij 1
	if (x == y) {
		rij[6].som += sym;
		rij[6].aantal++;
	}

	// schuine rij 2
	if (x + y == 2) {
		rij[7].som += sym;
		rij[7].aantal++;
	}
}

void printbord(int pos)
{
	int x, y, i, j;
	for (y = MAXCOOR; y >= 0; --y) {
		i = MAXCOOR - y;
		for (x = 0; x <= MAXCOOR; ++x) {
			j = x + pos * (LENGTE + 1);
			switch (bord[x + y * LENGTE])
			{
				case KRUISJE: scr[i][j] = 'x'; break;
				case RONDJE : scr[i][j] = 'o'; break;
				default     : scr[i][j] = '.'; break;
			}
		}
	}
}

int gewonnen()
{
	for (int i = 0; i < 8; ++i)
		if (rij[i].som == KRUISJE * LENGTE)
			return KRUISJE;
		else if (rij[i].som == RONDJE * LENGTE)
			return RONDJE;
	return LEEG;
}
