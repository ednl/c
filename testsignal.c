#include <stdlib.h>  // exit
#include <signal.h>  // signal

static void intHandler(int sig)
{
    exit(sig);
}

int main(void)
{
    signal(SIGINT, intHandler);
    while (1);
    return 0;
}
