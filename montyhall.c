#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define DOORS  3
#define REVEAL 1
#define GAMES  1000000

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int doors = DOORS, reveal = REVEAL, games = GAMES;
    int staywin = 0, staylose = 0, switchwin = 0, switchlose = 0;
    int price, pick;

    if (argc > 1) {
        int n = atoi(argv[1]);
        if (n >= 3) {
            doors = n;
            reveal = doors - 2;
        }
        if (argc > 2) {
            n = atoi(argv[2]);
            if (n >= 1 && n <= doors - 2) {
                reveal = n;
            }
        }
        if (argc > 3) {
            n = atoi(argv[3]);
            if (n >= 1) {
                games = n;
            }
        }
    }

    const double toperc = (double)games / 100;
    const int doorsleft = doors - reveal - 1;
    const bool onedoorleft = doorsleft == 1;

    printf("doors=%d reveal=%d games=%d\n\n", doors, reveal, games);
    for (int i = 0; i < games; ++i) {
        price = rand() / ((RAND_MAX + 1u) / doors);
        pick = rand() / ((RAND_MAX + 1u) / doors);
        if (pick == price) {
            staywin++;
            switchlose++;
        } else {
            staylose++;
            if (onedoorleft) {
                switchwin++;
            } else {
                pick = rand() / ((RAND_MAX + 1u) / doorsleft);
                if (pick == 0) {
                    switchwin++;
                } else {
                    switchlose++;
                }
            }
        }
    }
    printf("        |   win%% |  lose%% | total%%\n");
    printf("--------+--------+--------+-------\n");
    printf("stay    | %6.2f | %6.2f | %6.2f\n", (double)staywin / toperc, (double)staylose / toperc, (double)(staywin + staylose) / toperc);
    printf("switch  | %6.2f | %6.2f | %6.2f\n", (double)switchwin / toperc, (double)switchlose / toperc, (double)(switchwin + switchlose) / toperc);
    printf("--------+--------+--------+-------\n\n");
    printf("Better to switch by %.2f pp = %.1fx\n", (double)(switchwin - staywin) / toperc, (double)switchwin / staywin);
    return 0;
}
