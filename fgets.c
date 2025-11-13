#include <stdio.h>
#include <ctype.h>
#include <errno.h>

static char buf[8];

void show(void)
{
    fputc('"', stdout);
    const char *const end = buf + sizeof buf;
    for (const char *c = buf; c != end; ++c) {
        if (isprint(*c)) {
            if (*c == '"')
                fputc('\\', stdout);
            fputc(*c, stdout);
        } else {
            switch (*c) {
                case 0x00: printf("\\0"); break;
                case 0x07: printf("\\a"); break;
                case 0x08: printf("\\b"); break;
                case 0x09: printf("\\t"); break;
                case 0x0a: printf("\\n"); break;
                case 0x0b: printf("\\v"); break;
                case 0x0c: printf("\\f"); break;
                case 0x0d: printf("\\r"); break;
                default: printf("\\x%02x", (unsigned char)*c); break;
            }
        }
        if (!*c)
            break;
    }
    fputc('"', stdout);
    fputc('\n', stdout);
}

int main(void)
{
    printf("?");
    if (fgets(buf, sizeof buf, stdin))
        show();
    else
        fputc('\n', stdout);

    if (feof(stdin))
        puts("End of file");
    if (ferror(stdin))
        puts("Error");
    if (errno) {
        printf("errno=%d: ", errno);
        perror(NULL);
    }
}
