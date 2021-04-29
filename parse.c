#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp, strncmp
#include <stdint.h>  // int32_t, uint32_t
#include <stdbool.h> // bool, true, false
#include <ctype.h>   // isspace

#define API_SCREEN_W (320U)
#define API_SCREEN_H (240U)

#define MAX_DIGITS (9U)

typedef enum {
    ERR_OK = 0,
    ERR_UNKNOWN_CMD,
    ERR_TOO_FEW_ARG,
    ERR_TOO_MANY_ARG,
    ERR_NUM_TOO_LARGE,
    ERR_INVALID_NUM,
    ERR_OFF_SCREEN_X,
    ERR_OFF_SCREEN_Y,
    ERR_INVALID_COL,
    ERR_SYNTAX_ERROR,
    ERR_UNKNOWN_ERROR
} ERR_T;

typedef enum {
    ARG_POSX,ARG_POSY,ARG_DIMX,ARG_DIMY,
    ARG_COLOR,ARG_WEIGHT,ARG_FILL,ARG_RADIUS,
    ARG_TEXT,ARG_FONTNAME,ARG_FONTSIZE,ARG_FONTSTYLE,
    ARG_BMPID,ARG_MSEC,ARG_REPNUM,ARG_REPCOUNT
} ARG_T;

typedef struct {
    char *command;
    int argcount;
    ARG_T argtype[12];
    bool isint[12];
} SYNTAX;

static const SYNTAX syntax[] = {
    {
        .command  = "clearscherm",
        .argcount = 1,
        .argtype  = {ARG_COLOR},
        .isint    = {1}
    },
    {
        .command  = "punt",
        .argcount = 3,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_COLOR},
        .isint    = {1,1,1}
    },
    {
        .command  = "lijn",
        .argcount = 6,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT},
        .isint    = {1,1,1,1,1,1}
    },
    {
        .command  = "rechthoek",
        .argcount = 6,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_DIMX,ARG_DIMY,ARG_COLOR,ARG_FILL},
        .isint    = {1,1,1,1,1,1}
    },
    {
        .command  = "figuur",
        .argcount = 11,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,
                     ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR},
        .isint    = {1,1,1,1,1,1,1,1,1,1,1}
    },
    {
        .command  = "cirkel",
        .argcount = 4,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_COLOR},
        .isint    = {1,1,1,1}
    },
    {
        .command  = "ellips",
        .argcount = 5,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_RADIUS,ARG_COLOR},
        .isint    = {1,1,1,1,1}
    },
    {
        .command  = "tekst",
        .argcount = 7,
        .argtype  = {ARG_POSX,ARG_POSY,ARG_COLOR,ARG_TEXT,
                     ARG_FONTNAME,ARG_FONTSIZE,ARG_FONTSTYLE},
        .isint    = {1,1,1,0,0,1,0}
    },
    {
        .command  = "bitmap",
        .argcount = 3,
        .argtype  = {ARG_BMPID,ARG_POSX,ARG_POSY},
        .isint    = {1,1,1}
    },
    {
        .command  = "wacht",
        .argcount = 1,
        .argtype  = {ARG_MSEC},
        .isint    = {1}
    },
    {
        .command  = "herhaal",
        .argcount = 2,
        .argtype  = {ARG_REPNUM,ARG_REPCOUNT},
        .isint    = {1,1}
    },
};

typedef struct {
    CMD_T cmdid;
    int argint[12];

} COMMAND;

static char *input[] = {
    "cirkel 100 100 50 128",
    " lijn 0",
    "bla 1234 34 3456",
    "punt -1000 12342345567",
    "cirkel100x200_50+10 ",
};

static int parse(char *line)
{
    int cmdid = -1;
    int argcount = -1;
    int intcount = 0;
    char *token = strtok(line, ",");
    while (token)
    {
        argcount++;
        if (argcount == 0)
        {
            for (size_t i = 0; i < sizeof syntax / sizeof *syntax; ++i)
            {
                if (strcmp(token, syntax[i].command) == 0)
                {
                    cmdid = (int)i;
                    break;
                }
            }
            if (cmdid == -1)
                return ERR_UNKNOWN_CMD;
        }
        else if (argcount > syntax[cmdid].argcount)

            return ERR_TOO_MANY_ARG;

        else
        {
            int val;
            if (syntax[cmdid].isint[argcount])
            {
                while (isspace(*token))
                    ++token;
                if (strlen(token) > MAX_DIGITS)
                    return ERR_NUM_TOO_LARGE;
                val = atoi(token);
                if (val == 0 && strcmp(token, "0"))
                    return ERR_INVALID_NUM;
                switch (syntax[cmdid].argtype[argcount])
                {
                case ARG_POSX:
                    if (val < 0 || val >= (int)API_SCREEN_W)
                        return ERR_OFF_SCREEN_X;
                    break;
                case ARG_POSY:
                    if (val < 0 || val >= (int)API_SCREEN_H)
                        return ERR_OFF_SCREEN_Y;
                    break;
                case ARG_COLOR:
                    if (val < 0 || val > UINT8_MAX)
                        return ERR_INVALID_COL;
                    break;

                default:
                    return ERR_SYNTAX_ERROR;
                }
            }
        }
        token = strtok(NULL, ",");
    }
    if (argcount < syntax[cmdid].argcount)
        return ERR_TOO_FEW_ARG;
    return 0;
}

int main(void)
{
    for (size_t i = 0; i < sizeof input / sizeof *input; ++i)
    {
        printf("Foutcode: %i\n\n", parse((char*)input[i]));
    }
    return 0;
}
