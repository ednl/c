#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp, strncmp
#include <stdint.h>  // uint8_t, int32_t, uint32_t
#include <stdbool.h> // bool, true, false
#include <ctype.h>   // isspace

#define API_SCREEN_W (320U)
#define API_SCREEN_H (240U)
#define MAX_ARGS      (12U)
#define MAX_DIGITS     (9U)

////////// Errors /////////////////////////////////////////////////////////////

typedef enum {
    ERR_OK = 0,
    ERR_UNKNOWN_CMD,
    ERR_CMD_NOTFOUND,
    ERR_MISSING_ARG,
    ERR_TOO_MANY_ARG,
    ERR_NUM_TOO_LARGE,
    ERR_INVALID_NUM,
    ERR_OFF_SCREEN_X,
    ERR_OFF_SCREEN_Y,
    ERR_INVALID_COL,
    ERR_SYNTAX_ERROR,
    ERR_UNKNOWN_ERROR
} ERR_T;

////////// Arguments //////////////////////////////////////////////////////////

typedef enum {
    ARG_CMD, // a command is one type of argument (the first one on every line)
    ARG_POSX, ARG_POSY, ARG_DIMX, ARG_DIMY,
    ARG_COLOR, ARG_WEIGHT, ARG_FILL, ARG_RADIUS,
    ARG_TEXT, ARG_FONTNAME, ARG_FONTSIZE, ARG_FONTSTYLE,
    ARG_BMPID, ARG_MSEC, ARG_REPNUM, ARG_REPCOUNT,
    ARG_LAST // this has a value equal to the last real enum value
} ARG_T;

// Which arguments are of type int and which are not?
static const bool arg_is_int[ARG_LAST] = {0,1,1,1,1,1,1,1,1,0,0,1,0,1,1,1,1};

////////// Font Names /////////////////////////////////////////////////////////

typedef enum {
    FNM_ARIAL, FNM_CONSOLAS
} FNM_T;

typedef struct {
    FNM_T id;
    char *name;
} FNM, *PFNM;

static const FNM fontnames[] = {
    { .id = FNM_ARIAL,    .name = "arial"    },
    { .id = FNM_CONSOLAS, .name = "consolas" },
};

////////// Font Styles ////////////////////////////////////////////////////////

typedef enum {
    FST_NORMAL, FST_BOLD, FST_ITALIC
} FST_T;

typedef struct {
    FST_T id;
    char *name;
} FST, *PFST;

static const FST fontstyles[] = {
    { .id = FST_NORMAL, .name = "normaal" },
    { .id = FST_BOLD,   .name = "vet"     },
    { .id = FST_ITALIC, .name = "cursief" },
};

////////// Command Syntax /////////////////////////////////////////////////////

typedef enum {
    CMD_CLS,
    CMD_PIXEL, CMD_LINE, CMD_RECT, CMD_FIGURE, CMD_CIRCLE, CMD_ELLIPSE,
    CMD_TEXT, CMD_BITMAP, CMD_WAIT, CMD_REPEAT
} CMD_T;

typedef struct {
    CMD_T   id;
    char   *name;
    uint8_t argc;
    ARG_T   type[MAX_ARGS];
} SYNTAX, *PSYNTAX;

static const SYNTAX syntax[] = {
    { .id = CMD_CLS,     .name = "clearscherm", .argc =  2, .type = {ARG_CMD,ARG_COLOR} },
    { .id = CMD_PIXEL,   .name = "punt"       , .argc =  4, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_COLOR} },
    { .id = CMD_LINE,    .name = "lijn"       , .argc =  7, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT} },
    { .id = CMD_RECT,    .name = "rechthoek"  , .argc =  7, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_DIMX,ARG_DIMY,ARG_COLOR,ARG_FILL} },
    { .id = CMD_FIGURE,  .name = "figuur"     , .argc = 12, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR} },
    { .id = CMD_CIRCLE,  .name = "cirkel"     , .argc =  5, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_ELLIPSE, .name = "ellips"     , .argc =  6, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_TEXT,    .name = "tekst"      , .argc =  8, .type = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_TEXT,ARG_FONTNAME,ARG_FONTSIZE,ARG_FONTSTYLE} },
    { .id = CMD_BITMAP,  .name = "bitmap"     , .argc =  4, .type = {ARG_CMD,ARG_BMPID,ARG_POSX,ARG_POSY} },
    { .id = CMD_WAIT,    .name = "wacht"      , .argc =  2, .type = {ARG_CMD,ARG_MSEC} },
    { .id = CMD_REPEAT,  .name = "herhaal"    , .argc =  3, .type = {ARG_CMD,ARG_REPNUM,ARG_REPCOUNT} },
};

typedef struct {
    int ord;
    PSYNTAX syn;
    int val[MAX_ARGS];
    PFNM fnm;
    PFST fst;
    char *text;
} INSTR, *PINSTR;

////////// Test Input /////////////////////////////////////////////////////////

static char *script[] = {
    "clearscherm,zwart",
    "lijn,1,1,100,100,rood,4",
    "lijn,100,1,100,1,groen,3",
    "clearscherm,wit",
    "rechthoek,10,10,300,200,zwart,1",
    "rechthoek,10,10,150,100,lichtblauw,0",
    "rechthoek,160,110,150,100,geel,1",
    "clearscherm,zwart",
    "wacht,100",
    "tekst,10,20,  wit,the quick brown fox jumps over the lazy dog, consolas, 1, normaal",
    "tekst,10,100, magenta,the quick brown fox jumps over the lazy dog, arial, 2, vet",
    "tekst,10,200, lichtcyaan,the quick brown fox jumps over the lazy dog, consolas, 1, cursief",
    "clearscherm,wit",
    "bitmap,0,50,50",
    "clearscherm,wit",
    "bitmap,1,50,50",
    "clearscherm,wit",
    "bitmap,2,50,50",
    "clearscherm,bruin",
    "bitmap,3,50,50",
    "clearscherm,wit",
    "cirkel,120,160,25,groen",
    "clearscherm,magenta",
    "rechthoek,10,20,200,100,zwart, 1",
    "lijn,1,1,100,100,blauw,4",
    "tekst,10,50,groen,Beep boop ik ben een robot, consolas, 2, normaal",
    "wacht,2000",
    "clearscherm,zwart",
    "tekst,10,10,geel,Bijna klaar!!, consolas, 2, vet",
    "tekst,10,60,grijs,nu alleen nog de herhaalfunctie, arial, 1, cursief",
    "wacht,2000",
    "herhaal,9,4",
};

////////// Functions //////////////////////////////////////////////////////////

static ERR_T parse(char *line, PINSTR instr)
{
    static int cmdcount = 0;

    PSYNTAX this = NULL;
    int argcount = 0;
    int intcount = 0;
    char *token = strtok(line, ",");

    while (token)
    {
        if (argcount == 0)  // first token should be the command
        {
            // Which command is it?
            // (syntax is an array, so sizeof is the array length)
            for (size_t i = 0; i < sizeof syntax; ++i)
            {
                if (strcmp(token, syntax[i].name) == 0)  // found it
                {
                    this = &syntax[i];
                    instr->syn = this;
                    break;  // end of the for loop
                }
            }
            if (this == NULL)
                return ERR_UNKNOWN_CMD;
            instr->ord = cmdcount++;
        }
        else if (argcount <= this->argc)
        {
            if (arg_is_int[thiscmd->type[argcount - 1]])
            {
                int val = -1;
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
            else
            {
                //TODO: non-int argument
            }
        }
        else
            return ERR_TOO_MANY_ARG;
        argcount++;
        token = strtok(NULL, ",");
    }

    // What have we found?
    if (thiscmd == NULL)
        return ERR_CMD_NOTFOUND;
    if (argcount < thiscmd->argc)
        return ERR_MISSING_ARG;

    return ERR_OK;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    ERR_T err = ERR_OK;

    for (size_t i = 0; i < sizeof script / sizeof *script; ++i)
    {
        err = parse(script[i]);
    }
    return 0;
}
