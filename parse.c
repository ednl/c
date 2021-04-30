#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp, strncmp
#include <stdint.h>  // uint8_t, int32_t, uint32_t
#include <stdbool.h> // bool, true, false
#include <ctype.h>   // isspace

#define API_SCREEN_W (320U)
#define API_SCREEN_H (240U)
#define API_ERR_OK     (0)

#define BUFLEN       (128U)
#define MAX_ARGS      (13U)
#define MAX_DIGITS     (9U)

////////// Test Input /////////////////////////////////////////////////////////

static const char *script[] = {
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
    "figuur,1,2,3,4,5,6,7,8,9,10,lichtblauw,3",
    "ellips,160,120,150,110,rood"
};

////////// Errors /////////////////////////////////////////////////////////////

typedef enum {
    ERR_OK = 0,
    ERR_LINE_TOO_LONG,
    ERR_INTERNAL_ERROR,
    ERR_COMMAND_NOT_FOUND,
    ERR_MISSING_ARG,
    ERR_TOO_MANY_ARG,
    ERR_NUM_TOO_LARGE,
    ERR_INVALID_NUM,
    ERR_OFF_SCREEN_X,
    ERR_OFF_SCREEN_Y,
    ERR_INVALID_COLOR,
    ERR_INVALID_FONTNAME,
    ERR_INVALID_FONTSTYLE,
    ERR_NO_TEXT,
    ERR_OUT_OF_MEMORY,
    ERR_SYNTAX_ERROR,
    ERR_UNKNOWN_ERROR
} ERR_T;

////////// Arguments //////////////////////////////////////////////////////////

// TODO: add optional arguments
typedef enum {
    ARG_SIGNED = -1,
    ARG_CMD = 0, // a command is one type of argument (the first one on every line)
    ARG_POSX, ARG_POSY, ARG_DIMX, ARG_DIMY,
    ARG_COLOR, ARG_WEIGHT, ARG_FILL, ARG_RADIUS,
    ARG_TEXT, ARG_FONTNAME, ARG_FONTSIZE, ARG_FONTSTYLE,
    ARG_BMPID, ARG_MSEC, ARG_REPNUM, ARG_REPCOUNT,
    ARG_LAST // this has a value equal to the last real enum value
} ARG_T;

// Which arguments are of type int and which are not?
static const bool arg_is_int[ARG_LAST] = {0,1,1,1,1,0,1,1,1,0,0,1,0,1,1,1,1};

////////// Colour Names ///////////////////////////////////////////////////////

typedef enum {
    COL_SIGNED = -1,
    COL_BLACK    = 0b00000000,
    COL_GREY     = 0b10010010,
    COL_WHITE    = 0b11111111,
    COL_RED      = 0b11100000,
    COL_LRED     = 0b11110010,
    COL_GREEN    = 0b00011100,
    COL_LGREEN   = 0b10011110,
    COL_BLUE     = 0b00000011,
    COL_LBLUE    = 0b10010011,
    COL_CYAN     = 0b00010010,
    COL_LCYAN    = 0b00011111,
    COL_MAGENTA  = 0b10000010,
    COL_LMAGENTA = 0b11100011,
    COL_BROWN    = 0b10010000,
    COL_YELLOW   = 0b11111100,
} COL_T;

typedef struct {
    COL_T id;
    char *name;
} COL, *PCOL;

static const COL colornames[] = {
    { .id = COL_BLACK,    .name = "zwart"        },
    { .id = COL_GREY,     .name = "grijs"        },
    { .id = COL_WHITE,    .name = "wit"          },
    { .id = COL_RED,      .name = "rood"         },
    { .id = COL_LRED,     .name = "lichtrood"    },
    { .id = COL_GREEN,    .name = "groen"        },
    { .id = COL_LGREEN,   .name = "lichtgroen"   },
    { .id = COL_BLUE,     .name = "blauw"        },
    { .id = COL_LBLUE,    .name = "lichtblauw"   },
    { .id = COL_CYAN,     .name = "cyaan"        },
    { .id = COL_LCYAN,    .name = "lichtcyaan"   },
    { .id = COL_MAGENTA,  .name = "magenta"      },
    { .id = COL_LMAGENTA, .name = "lichtmagenta" },
    { .id = COL_BROWN,    .name = "bruin"        },
    { .id = COL_YELLOW,   .name = "geel"         },
};

////////// Font Names /////////////////////////////////////////////////////////

typedef enum {
    FNM_SIGNED = -1,
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
    FST_SIGNED = -1,
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
    CMD_SIGNED = -1,
    CMD_CLS, CMD_PIXEL, CMD_LINE, CMD_RECT, CMD_FIGURE, CMD_CIRCLE, CMD_ELLIPSE,
    CMD_TEXT, CMD_BITMAP, CMD_WAIT, CMD_REPEAT
} CMD_T;

typedef struct {
    CMD_T id;
    int   argcount;
    ARG_T argtype[MAX_ARGS];
    char *name;
} SYNTAX, *PSYNTAX;

static const SYNTAX syntax[] = {
    { .id = CMD_CLS,     .name = "clearscherm", .argcount =  2, .argtype = {ARG_CMD,ARG_COLOR} },
    { .id = CMD_PIXEL,   .name = "punt"       , .argcount =  4, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_COLOR} },
    { .id = CMD_LINE,    .name = "lijn"       , .argcount =  7, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT} },
    { .id = CMD_RECT,    .name = "rechthoek"  , .argcount =  7, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_DIMX,ARG_DIMY,ARG_COLOR,ARG_FILL} },
    { .id = CMD_FIGURE,  .name = "figuur"     , .argcount = 13, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT} },
    { .id = CMD_CIRCLE,  .name = "cirkel"     , .argcount =  5, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_ELLIPSE, .name = "ellips"     , .argcount =  6, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_TEXT,    .name = "tekst"      , .argcount =  8, .argtype = {ARG_CMD,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_TEXT,ARG_FONTNAME,ARG_FONTSIZE,ARG_FONTSTYLE} },
    { .id = CMD_BITMAP,  .name = "bitmap"     , .argcount =  4, .argtype = {ARG_CMD,ARG_BMPID,ARG_POSX,ARG_POSY} },
    { .id = CMD_WAIT,    .name = "wacht"      , .argcount =  2, .argtype = {ARG_CMD,ARG_MSEC} },
    { .id = CMD_REPEAT,  .name = "herhaal"    , .argcount =  3, .argtype = {ARG_CMD,ARG_REPNUM,ARG_REPCOUNT} },
};

// Complete instruction ready to be run
typedef struct {
    ERR_T   err;
    int     ord, val[MAX_ARGS];
    PSYNTAX syn;
    PFNM    fnm;
    PFST    fst;
    char   *txt;  // dynamically allocated when needed => free after use!
} INSTR, *PINSTR;

////////// Functions //////////////////////////////////////////////////////////

// Run a fully prepared instruction
static ERR_T run(PINSTR pinstr)
{
    if (pinstr == NULL)
        return ERR_COMMAND_NOT_FOUND;

    if (pinstr->err != ERR_OK)
        return pinstr->err;

    switch (pinstr->syn->id)
    {
    case CMD_CLS:
        // return API_clearscreen(pinstr->val[0]);
        printf("%i: API_clearscreen(%i);\n", pinstr->ord, pinstr->val[0]);
        return API_ERR_OK;
    case CMD_PIXEL:
        // return API_draw_pixel(pinstr->val[0], pinstr->val[1], pinstr->val[2]);
        printf("%i: API_draw_pixel(%i,%i,%i);\n",
            pinstr->ord, pinstr->val[0], pinstr->val[1], pinstr->val[2]);
        return API_ERR_OK;
    case CMD_LINE:
        // return API_draw_line(pinstr->val[0], pinstr->val[1], pinstr->val[2], pinstr->val[3],
        //                      pinstr->val[4], pinstr->val[5], 0);
        printf("%i: API_draw_line(%i,%i,%i,%i,%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1], pinstr->val[2], pinstr->val[3],
            pinstr->val[4], pinstr->val[5], 0);
        return API_ERR_OK;
    case CMD_RECT:
        // return API_draw_rectangle(pinstr->val[0], pinstr->val[1], pinstr->val[2], pinstr->val[3],
        //                           pinstr->val[4], pinstr->val[5], 0, 0);
        printf("%i: API_draw_rectangle(%i,%i,%i,%i,%i,%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1], pinstr->val[2], pinstr->val[3],
            pinstr->val[4], pinstr->val[5], 0, 0);
        return API_ERR_OK;
    case CMD_FIGURE:
        // return API_draw_figure(pinstr->val[0], pinstr->val[1],
        //                        pinstr->val[2], pinstr->val[3],
        //                        pinstr->val[4], pinstr->val[5],
        //                        pinstr->val[6], pinstr->val[7],
        //                        pinstr->val[8], pinstr->val[9],
        //                        pinstr->val[10], pinstr->val[11]);
        printf("%i: API_draw_figure(%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1],
            pinstr->val[2], pinstr->val[3],
            pinstr->val[4], pinstr->val[5],
            pinstr->val[6], pinstr->val[7],
            pinstr->val[8], pinstr->val[9],
            pinstr->val[10], pinstr->val[11]);
        return API_ERR_OK;
    case CMD_CIRCLE:
        // return API_draw_circle(pinstr->val[0], pinstr->val[1],
        //                        pinstr->val[2], pinstr->val[3], 0);
        printf("%i: API_draw_circle(%i,%i,%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1],
            pinstr->val[2], pinstr->val[3], 0);
        return API_ERR_OK;
    case CMD_ELLIPSE:
        // return API_draw_ellipse(pinstr->val[0], pinstr->val[1],
        //                         pinstr->val[2], pinstr->val[3],
        //                         pinstr->val[4]);
        printf("%i: API_draw_ellipse(%i,%i,%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1],
            pinstr->val[2], pinstr->val[3],
            pinstr->val[4]);
        return API_ERR_OK;
    case CMD_TEXT:
        // return API_draw_text(pinstr->val[0], pinstr->val[1], pinstr->val[2],
        //                      pinstr->txt, pinstr->fnm->name,
        //                      pinstr->val[3], pinstr->fst->id, 0);
        printf("%i: API_draw_text(%i,%i,%i,\"%s\",\"%s\",%i,%i,%i);\n",
            pinstr->ord,
            pinstr->val[0], pinstr->val[1], pinstr->val[2],
            pinstr->txt, pinstr->fnm->name,
            pinstr->val[3], pinstr->fst->id, 0);
        return API_ERR_OK;
    case CMD_BITMAP:
        // return API_draw_bitmap(pinstr->val[0], pinstr->val[1], pinstr->val[2]);
        printf("%i: API_draw_bitmap(%i,%i,%i);\n",
            pinstr->ord, pinstr->val[0], pinstr->val[1], pinstr->val[2]);
        return API_ERR_OK;
    case CMD_WAIT:
        // return API_wait(pinstr->val[0]);
        printf("%i: API_wait(%i);\n", pinstr->ord, pinstr->val[0]);
        return API_ERR_OK;
    case CMD_REPEAT:
        // return API_repeat_commands(pinstr->val[0], pinstr->val[1], 0);
        printf("%i: API_repeat_commands(%i,%i,%i);\n",
            pinstr->ord, pinstr->val[0], pinstr->val[1], 0);
        return API_ERR_OK;
    default:
        return ERR_COMMAND_NOT_FOUND;
    }
}

// Parse a line of text into a complete instruction
static ERR_T parse(const char *line, PINSTR pinstr)
{
    // Global command counter
    static int cmdcount = 0;

    PSYNTAX this = NULL;
    int argindex = 0;
    int intindex = 0;
    char buf[BUFLEN] = {0};

    if (strlen(line) >= BUFLEN)  // also equal, for null-terminator
    {
        pinstr->err = ERR_LINE_TOO_LONG;
        return ERR_LINE_TOO_LONG;
    }

    // Reset error code
    pinstr->err = ERR_OK;

    // Copy because strtok wants to modify it
    strcpy(buf, line);
    char *token = strtok(buf, ",");
    while (token)
    {
        if (argindex == 0)  // first token should be the command
        {
            // Which command is it?
            for (size_t i = 0; i < sizeof syntax / sizeof *syntax; ++i)
            {
                if (strcmp(token, syntax[i].name) == 0)  // found it
                {
                    this = &syntax[i];
                    pinstr->ord = cmdcount++;
                    pinstr->syn = this;
                    break;  // end of the for loop
                }
            }
            if (this == NULL)
            {
                pinstr->err = ERR_COMMAND_NOT_FOUND;
                return ERR_COMMAND_NOT_FOUND;
            }
        }
        else if (argindex < this->argcount)
        {
            if (arg_is_int[this->argtype[argindex]])
            {
                // Skip spaces to count relevant strlen
                while (isspace(*token))
                    ++token;
                // Check strlen to avoid overflow = undefined result
                if (strlen(token) > MAX_DIGITS)
                {
                    pinstr->err = ERR_NUM_TOO_LARGE;
                    return ERR_NUM_TOO_LARGE;
                }
                int val = atoi(token);
                // Check if zero was really zero
                if (val == 0 && strcmp(token, "0"))
                {
                    pinstr->err = ERR_INVALID_NUM;
                    return ERR_INVALID_NUM;
                }
                // Range checks for each type of argument
                // NOT NECESSARY! ALREADY HAPPENS IN API FUNCTIONS!
                switch (this->argtype[argindex])
                {
                case ARG_POSX:
                    if (val < 0 || val >= (int)API_SCREEN_W)
                    {
                        pinstr->err = ERR_OFF_SCREEN_X;
                        return ERR_OFF_SCREEN_X;
                    }
                    break;
                case ARG_POSY:
                    if (val < 0 || val >= (int)API_SCREEN_H)
                    {
                        pinstr->err = ERR_OFF_SCREEN_Y;
                        return ERR_OFF_SCREEN_Y;
                    }
                    break;
                default:
                    // Other types of argument: no further checks
                    break;
                }
                // It's a good value, but does it fit?
                if (intindex >= (int)MAX_ARGS)
                {
                    pinstr->err = ERR_TOO_MANY_ARG;
                    return ERR_TOO_MANY_ARG;
                }
                // All OK, store it
                pinstr->val[intindex++] = val;
            }
            else
            {
                // Skip leading spaces for text data
                while (isspace(*token))
                    ++token;

                bool found = false;
                switch (this->argtype[argindex])
                {
                case ARG_COLOR:
                    // Does it fit?
                    if (intindex >= (int)MAX_ARGS)
                    {
                        pinstr->err = ERR_TOO_MANY_ARG;
                        return ERR_TOO_MANY_ARG;
                    }
                    // Find it
                    for (size_t i = 0; i < sizeof colornames / sizeof *colornames; ++i)
                    {
                        if (strcmp(token, colornames[i].name) == 0)
                        {
                            found = true;
                            pinstr->val[intindex++] = colornames[i].id;
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pinstr->err = ERR_INVALID_COLOR;
                        return ERR_INVALID_COLOR;
                    }
                    break;
                case ARG_FONTNAME:
                    for (size_t i = 0; i < sizeof fontnames / sizeof *fontnames; ++i)
                    {
                        if (strcmp(token, fontnames[i].name) == 0)
                        {
                            found = true;
                            pinstr->fnm = &fontnames[i];
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pinstr->err = ERR_INVALID_FONTNAME;
                        return ERR_INVALID_FONTNAME;
                    }
                    break;
                case ARG_FONTSTYLE:
                    for (size_t i = 0; i < sizeof fontstyles / sizeof *fontstyles; ++i)
                    {
                        if (strcmp(token, fontstyles[i].name) == 0)
                        {
                            found = true;
                            pinstr->fst = &fontstyles[i];
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pinstr->err = ERR_INVALID_FONTSTYLE;
                        return ERR_INVALID_FONTSTYLE;
                    }
                    break;
                case ARG_TEXT:
                    if (strlen(token) == 0)
                    {
                        pinstr->err = ERR_NO_TEXT;
                        return ERR_NO_TEXT;
                    }
                    if (pinstr->txt != NULL)
                    {
                        free(pinstr->txt);
                        pinstr->txt = NULL;
                    }
                    pinstr->txt = strdup(token);
                    if (pinstr->txt == NULL)
                    {
                        pinstr->err = ERR_OUT_OF_MEMORY;
                        return ERR_OUT_OF_MEMORY;
                    }
                    break;
                default:
                    // Other types of non-int argument: no further processing
                    break;
                }

            }
        }
        else
        {
            pinstr->err = ERR_TOO_MANY_ARG;
            return ERR_TOO_MANY_ARG;
        }
        argindex++;
        token = strtok(NULL, ",");
    }

    // What have we found?
    if (this == NULL)
    {
        pinstr->err = ERR_COMMAND_NOT_FOUND;
        return ERR_COMMAND_NOT_FOUND;
    }
    if (argindex < this->argcount - 1)
    {
        pinstr->err = ERR_MISSING_ARG;
        return ERR_MISSING_ARG;
    }

    // All done & OK
    return ERR_OK;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    ERR_T err = ERR_OK;
    INSTR instr = {0};

    for (size_t i = 0; i < sizeof script / sizeof *script; ++i)
    {
        printf("\nInput: \"%s\"\n", script[i]);
        if ((err = parse(script[i], &instr)) == ERR_OK)
        {
            if ((err = run(&instr)) != ERR_OK)
                printf("Run error: %u\n", err);
        }
        else
            printf("Parse error: %u\n", err);
    }

    // Clean up memory allocation
    if (instr.txt != NULL)
        free(instr.txt);

    return 0;
}
