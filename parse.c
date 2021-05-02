#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp, strncmp
#include <stdint.h>  // uint8_t, int32_t, uint32_t
#include <stdbool.h> // bool, true, false
#include <ctype.h>   // isspace

#define API_SCREEN_W (320U)
#define API_SCREEN_H (240U)

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
    "figuur,1,2,3,4,5,6,7,8,9,10,lichtblauw  ,3",
    "ellips,160,120,150,110,rood"
};

////////// Errors /////////////////////////////////////////////////////////////

typedef enum {
    ERR_OK = 0,

    ERR_INVALID_CMD,
    ERR_INVALID_NUM,
    ERR_INVALID_COL,
    ERR_INVALID_FNM,
    ERR_INVALID_FST,

    ERR_LINE_TOO_LONG,
    ERR_EMPTY_ARG,
    ERR_MISSING_ARG,
    ERR_TOO_MANY_ARG,

    ERR_OFF_SCREEN_X,
    ERR_OFF_SCREEN_Y,

    ERR_OUT_OF_MEMORY,
    ERR_UNKNOWN_ERROR,
} ERR_T;

typedef enum {
    API_ERR_OK = 0,
} API_ERR_T;

////////// Arguments //////////////////////////////////////////////////////////

typedef enum {
    ARG_SIGNED = -1,  // make sure the type is signed int
    ARG_CMD    =  0,  // text (command is a type of argument, first one on every line)
    ARG_INT,          // integer
    ARG_POSX,         // integer
    ARG_POSY,         // integer
    ARG_DIMX,         // integer
    ARG_DIMY,         // integer
    ARG_COLOR,        // text
    ARG_WEIGHT,       // integer
    ARG_FILL,         // integer
    ARG_RADIUS,       // integer
    ARG_TEXT,         // text
    ARG_FONTNAME,     // text
    ARG_FONTSIZE,     // integer
    ARG_FONTSTYLE,    // text
    ARG_BMPID,        // integer
    ARG_MSEC,         // integer
    ARG_REPNUM,       // integer
    ARG_REPCOUNT,     // integer
    ARG_RESERVED,     // integer
    ARG_LAST,         // used to determine last enum value
} ARG_T;

// Which arguments are of type int and which are not? (index 0 = ARG_CMD)
static const bool arg_is_int[ARG_LAST] = {0,1,1,1,1,1,0,1,1,1,0,0,1,0,1,1,1,1,1};

////////// Colour Names ///////////////////////////////////////////////////////

typedef enum {
    COL_SIGNED = -1,
    COL_BLACK    = 0b00000000,
    COL_BLUE     = 0b00000010,
    COL_GREEN    = 0b00010000,
    COL_CYAN     = 0b00010010,
    COL_RED      = 0b10000000,
    COL_MAGENTA  = 0b10000010,
    COL_BROWN    = 0b01000100,
    COL_GREY     = 0b10010010,
    COL_DARKGREY = 0b01001001,
    COL_LBLUE    = 0b01001111,
    COL_LGREEN   = 0b01011101,
    COL_LCYAN    = 0b01011111,
    COL_LRED     = 0b11101001,
    COL_LMAGENTA = 0b11101011,
    COL_YELLOW   = 0b11111100,
    COL_WHITE    = 0b11111111,
} COL_T;

typedef struct {
    COL_T id;
    char *name;
} COL, *PCOL;

static const COL colornames[] = {
    { .id = COL_BLACK,    .name = "zwart"        },
    { .id = COL_BLUE,     .name = "blauw"        },
    { .id = COL_GREEN,    .name = "groen"        },
    { .id = COL_CYAN,     .name = "cyaan"        },
    { .id = COL_RED,      .name = "rood"         },
    { .id = COL_MAGENTA,  .name = "magenta"      },
    { .id = COL_BROWN,    .name = "bruin"        },
    { .id = COL_GREY,     .name = "grijs"        },
    { .id = COL_DARKGREY, .name = "donkergrijs"  },
    { .id = COL_LBLUE,    .name = "lichtblauw"   },
    { .id = COL_LGREEN,   .name = "lichtgroen"   },
    { .id = COL_LCYAN,    .name = "lichtcyaan"   },
    { .id = COL_LRED,     .name = "lichtrood"    },
    { .id = COL_LMAGENTA, .name = "lichtmagenta" },
    { .id = COL_YELLOW,   .name = "geel"         },
    { .id = COL_WHITE,    .name = "wit"          },
};

////////// Font Names /////////////////////////////////////////////////////////

typedef enum {
    FNM_SIGNED = -1,
    FNM_ARIAL,
    FNM_CONSOLAS,
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
    FST_NORMAL,
    FST_BOLD,
    FST_ITALIC,
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
    CMD_TEXT, CMD_BITMAP, CMD_WAIT, CMD_REPEAT,
    CMD_SCROLL_U, CMD_SCROLL_D, CMD_SCROLL_L, CMD_SCROLL_R,
} CMD_T;

typedef struct {
    CMD_T id;
    int   argcount, optcount;
    ARG_T argtype[MAX_ARGS];
    char *name;
} CMD, *PCMD;

static const CMD commands[] = {
    { .id = CMD_CLS,      .name = "clearscherm", .argcount =  1, .optcount = 0, .argtype = {ARG_COLOR} },
    { .id = CMD_PIXEL,    .name = "punt"       , .argcount =  3, .optcount = 0, .argtype = {ARG_POSX,ARG_POSY,ARG_COLOR} },
    { .id = CMD_LINE,     .name = "lijn"       , .argcount =  6, .optcount = 1, .argtype = {ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT} },
    { .id = CMD_RECT,     .name = "rechthoek"  , .argcount =  6, .optcount = 2, .argtype = {ARG_POSX,ARG_POSY,ARG_DIMX,ARG_DIMY,ARG_COLOR,ARG_FILL} },
    { .id = CMD_FIGURE,   .name = "figuur"     , .argcount = 12, .optcount = 0, .argtype = {ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_POSX,ARG_POSY,ARG_COLOR,ARG_WEIGHT} },
    { .id = CMD_CIRCLE,   .name = "cirkel"     , .argcount =  4, .optcount = 0, .argtype = {ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_ELLIPSE,  .name = "ellips"     , .argcount =  5, .optcount = 0, .argtype = {ARG_POSX,ARG_POSY,ARG_RADIUS,ARG_RADIUS,ARG_COLOR} },
    { .id = CMD_TEXT,     .name = "tekst"      , .argcount =  7, .optcount = 0, .argtype = {ARG_POSX,ARG_POSY,ARG_COLOR,ARG_TEXT,ARG_FONTNAME,ARG_FONTSIZE,ARG_FONTSTYLE} },
    { .id = CMD_BITMAP,   .name = "bitmap"     , .argcount =  3, .optcount = 0, .argtype = {ARG_BMPID,ARG_POSX,ARG_POSY} },
    { .id = CMD_WAIT,     .name = "wacht"      , .argcount =  1, .optcount = 0, .argtype = {ARG_MSEC} },
    { .id = CMD_REPEAT,   .name = "herhaal"    , .argcount =  2, .optcount = 0, .argtype = {ARG_REPNUM,ARG_REPCOUNT} },
    { .id = CMD_SCROLL_U, .name = "omhoog"     , .argcount =  0, .optcount = 1, .argtype = {} },
    { .id = CMD_SCROLL_D, .name = "omlaag"     , .argcount =  0, .optcount = 1, .argtype = {} },
    { .id = CMD_SCROLL_L, .name = "links"      , .argcount =  0, .optcount = 1, .argtype = {} },
    { .id = CMD_SCROLL_R, .name = "rechts"     , .argcount =  0, .optcount = 1, .argtype = {} },
};

////////// Parsed Command = Instruction ///////////////////////////////////////

typedef struct {
    ERR_T   err;
    int     ord, val[MAX_ARGS];
    PSYNTAX syn;  // pointer to const struct
    PCOL    col;  // pointer to const struct
    PFNM    fnm;  // pointer to const struct
    PFST    fst;  // pointer to const struct
    char   *txt;  // dynamically allocated when needed => free after use!
} INS, *PINS;

////////// Fake Functions /////////////////////////////////////////////////////

static API_ERR_T API_clearscreen(int color)
{
    printf("API_clearscreen(%i)", color);
    return ERR_OK;
}

static API_ERR_T API_draw_pixel(int x, int y, int color)
{
    printf("API_draw_pixel(%i, %i, %i)", x, y, color);
    return ERR_OK;
}

static API_ERR_T API_draw_line(int x1, int y1, int x2, int y2, int color, int weight, int reserved)
{
    printf("API_draw_line(%i, %i, %i, %i, %i, %i, %i)", x1, y1, x2, y2, color, weight, reserved);
    return ERR_OK;
}

static API_ERR_T API_draw_rectangle(int x, int y, int width, int height, int color, int filled, int reserved1, int reserved2)
{
    printf("API_draw_rectangle(%i, %i, %i, %i, %i, %i, %i, %i)", x, y, width, height, color, filled, reserved1, reserved2);
    return ERR_OK;
}

static API_ERR_T API_draw_text(int x_lup, int y_lup, int color, char *text, char *fontname, int fontsize, int fontstyle, int aligned)
{
    printf("API_draw_text(%i, %i, %i, \"%s\", \"%s\", %i, %i, %i)", x_lup, y_lup, color, text, fontname, fontsize, fontstyle, aligned);
    return ERR_OK;
}

static API_ERR_T API_draw_bitmap(int x_lup, int y_lup, int bm_nr)
{
    printf("API_draw_bitmap(%i, %i, %i)", x_lup, y_lup, bm_nr);
    return ERR_OK;
}

static API_ERR_T API_wait(int msecs)
{
    printf("API_wait(%i)", msecs);
    return ERR_OK;
}

static API_ERR_T API_repeat_commands(int nr_previous_commands, int iterations, int reserved)
{
    printf("API_repeat_commands(%i, %i, %i)", nr_previous_commands, iterations, reserved);
    return ERR_OK;
}

static API_ERR_T API_draw_ellipse(int x, int y, int r_hor, int r_ver, int color, int filled)
{
    printf("API_draw_ellipse(%i, %i, %i, %i, %i, %i)", x, y, r_hor, r_ver, color, filled);
    return ERR_OK;
}

static API_ERR_T API_draw_circle(int x, int y, int radius, int color, int filled)
{
    printf("API_draw_circle(%i, %i, %i, %i, %i)", x, y, radius, color, filled);
    return ERR_OK;
}

static API_ERR_T API_draw_figure(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5, int color, int weight)
{
    printf("API_draw_figure(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)", x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, color, weight);
    return ERR_OK;
}

static API_ERR_T API_scroll_up(int amount)
{
    printf("API_scroll_up(%i)", amount);
    return ERR_OK;
}

static API_ERR_T API_scroll_down(int amount)
{
    printf("API_scroll_down(%i)", amount);
    return ERR_OK;
}

static API_ERR_T API_scroll_left(int amount)
{
    printf("API_scroll_left(%i)", amount);
    return ERR_OK;
}

static API_ERR_T API_scroll_right(int amount)
{
    printf("API_scroll_right(%i)", amount);
    return ERR_OK;
}

static API_ERR_T API_color_chart(void)
{
    printf("API_color_chart()");
    return ERR_OK;
}

////////// Functions //////////////////////////////////////////////////////////

// Run a fully prepared instruction
static ERR_T run_cmd(PINSTR pins)
{
    // Null pointer?
    if (pins == NULL)
        return ERR_INVALID_CMD;

    // Fout in de struct?
    if (pins->err != ERR_OK)
        return pins->err;

    // Command counter
    printf("%i: ", pins->ord);

    // Roep de juiste API-lib functie aan
    switch (pins->syn->id)
    {
    case CMD_CLS:
        return API_clearscreen(
            pins->val[0]);               // colour
    case CMD_PIXEL:
        return API_draw_pixel(
            pins->val[0], pins->val[1],  // x, y
            pins->val[2]);               // colour
    case CMD_LINE:
        return API_draw_line(
            pins->val[0], pins->val[1],  // x1, y1
            pins->val[2], pins->val[3],  // x2, y2
            pins->col->id,               // colour
            pins->val[4],                // weight
            0);                          // reserved
    case CMD_RECT:
        return API_draw_rectangle(
           pins->val[0], pins->val[1],   // x1, y1
           pins->val[2], pins->val[3],   // width, height
           pins->col->id,                // colour
           pins->val[4],                 // filled
           0, 0);                        // reserved
    case CMD_FIGURE:
        return API_draw_figure(
            pins->val[0], pins->val[1],  // x1, y1
            pins->val[2], pins->val[3],  // x2, y2
            pins->val[4], pins->val[5],  // x3, y3
            pins->val[6], pins->val[7],  // x4, y4
            pins->val[8], pins->val[9],  // x5, y5
            pins->col->id,               // colour
            pins->val[10]);              // weight
    case CMD_CIRCLE:
        return API_draw_circle(
            pins->val[0], pins->val[1],  // x, y
            pins->val[2],                // radius
            pins->col->id,               // colour
            0);                          // filled
    case CMD_ELLIPSE:
        return API_draw_ellipse(
            pins->val[0], pins->val[1],  // x, y
            pins->val[2], pins->val[3],  // r_hor, r_ver
            pins->col->id,               // colour
            0);                          // filled
    case CMD_TEXT:
        return API_draw_text(
            pins->val[0], pins->val[1],  // x, y
            pins->col->id,               // colour
            pins->txt,                   // text
            pins->fnm->name,             // font name
            pins->val[2],                // font size
            pins->fst->id,               // font style
            1);                          // aligned
    case CMD_BITMAP:
        return API_draw_bitmap(
            pins->val[1], pins->val[2],  // x, y
            pins->val[0]);               // bitmap ID
    case CMD_WAIT:
        return API_wait(
            pins->val[0]);               // milliseconds
    case CMD_REPEAT:
        return API_repeat_commands(
            pins->val[0],                // count
            pins->val[1],                // loops
            0);                          // reserved
    case CMD_SCROLL_U:
        return API_scroll_up(
            pins->val[0]);               // amount
    case CMD_SCROLL_D:
        return API_scroll_down(
            pins->val[0]);               // amount
    case CMD_SCROLL_L:
        return API_scroll_left(
            pins->val[0]);               // amount
    case CMD_SCROLL_R:
        return API_scroll_right(
            pins->val[0]);               // amount
    default:
        return ERR_INVALID_CMD;
    }
}

static ERR_T str_to_int(const char *s, int *n)
{
    // Check strlen to avoid overflow = undefined result
    if (strlen(s) > MAX_DIGITS)
        return ERR_INVALID_NUM;

    int val = atoi(s);

    // Check if zero was really zero, or if the conversion failed
    // TODO: this will fail on potentially valid represenations such as "+0", "00"
    if (val == 0 && strcmp(s, "0"))
        return ERR_INVALID_NUM;

    *n = val;
    return ERR_OK;
}

// Check integer value against allowed range per type
static ERR_T range_check(const int n, const ARG_T type)
{
    switch (type)
    {
    case ARG_POSX:
        return n >= 0 && n < (int)API_SCREEN_W ? ERR_OK : ERR_OFF_SCREEN_X;
    case ARG_POSY:
        return n >= 0 && n < (int)API_SCREEN_H ? ERR_OK : ERR_OFF_SCREEN_Y;
    }
    return ERR_OK;
}

// Look up in command syntax dictionary
static ERR_T find_cmd(const char *s, PSYNTAX psyn)
{
    bool found = false;
    for (size_t i = 0; i < sizeof syntax / sizeof *syntax; ++i)
    {
        if (strcmp(s, syntax[i].name) == 0)
        {
            found = true;
            psyn = &syntax[i];
            break;  // end of the for loop
        }
    }
    return found ? ERR_OK : ERR_INVALID_CMD;
}

// Look up in colour name dictionary
static ERR_T find_cmd(const char *s, PCOL pcol)
{
    bool found = false;
    for (size_t i = 0; i < sizeof colornames / sizeof *colornames; ++i)
    {
        if (strcmp(s, colornames[i].name) == 0)
        {
            found = true;
            pcol = &colornames[i];
            break;  // end of the for loop
        }
    }
    return found ? ERR_OK : ERR_INVALID_COL;
}

// Parse a line of text into a complete instruction
static ERR_T parse(const char *line, PINSTR pins)
{
    // Global command counter
    static int cmdcount = 0;

    PSYNTAX this = NULL;  // pointer to syntax info of current command
    int argindex = 0;
    int intindex = 0;
    char buf[BUFLEN] = {0};
    ERR_T e = ERR_OK;

    if (strlen(line) >= BUFLEN)  // also check equal to leave space for null-terminator
    {
        pins->err = ERR_LINE_TOO_LONG;
        return ERR_LINE_TOO_LONG;
    }

    // Reset error code in struct
    pins->err = ERR_OK;

    // Copy because strtok wants to modify it
    strcpy(buf, line);

    // Find index to start of first field in comma-delimited line.
    // Normally, this would simply be the first char of the line;
    // but also, this replaces all commas with null chars.
    char *token = strtok(buf, ",");

    while (token)
    {
        // Is this the first token? Then it should be a command
        if (argindex == 0)
        {
            if ((e = find_cmd(token, this)) == ERR_OK)
            {
                pins->ord = cmdcount++;  // save ordinal number to struct
                pins->syn = this;  // save syntax pointer to struct
            }
            else
            {
                pins->err = e;
                return e;
            }
        }
        else if (argindex < this->argcount)
        {
            // Skip leading spaces to be able to count relevant strlen
            while (isspace(*token))
                ++token;

            // Skip trailing spaces and set null-terminator
            // (token length will be restored at the end of this part)
            char *end = token + strlen(token);
            char *last = end - 1;
            while (last >= token && isspace(*last))
                last--;
            if (++last != end)
                *last = '\0';

            // Anything left?
            if (strlen(token) == 0)
            {
                pins->err = ERR_EMPTY_ARGUMENT;
                return ERR_EMPTY_ARGUMENT;
            }

            // Save argument type for convenience
            ARG_T argtype = this->argtype[argindex];

            if (arg_is_int[argtype])
            {
                // Convert to int
                int val = 0;
                if ((e = str_to_int(token, val)) != ERR_OK)
                {
                    pins->err = e;
                    return e;
                }

                // Range checks for each type of argument
                // NOT NECESSARY! ALREADY HAPPENS IN API FUNCTIONS!
                if ((e = range_check(val, argtype)) != ERR_OK)
                {
                    pins->err = e;
                    return e;
                }

                // So it's a good value, but does it fit?
                if (intindex >= (int)MAX_ARGS)
                {
                    pins->err = ERR_TOO_MANY_ARG;
                    return ERR_TOO_MANY_ARG;
                }
                // All OK, store it
                pins->val[intindex++] = val;
            }
            else
            {
                bool found = false;
                switch (argtype)
                {
                case ARG_COLOR:
                    // Look up in colour names dictionary
                    for (size_t i = 0; i < sizeof colornames / sizeof *colornames; ++i)
                    {
                        if (strcmp(token, colornames[i].name) == 0)
                        {
                            found = true;
                            pins->col = &colornames[i];  // save the colorname pointer
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pins->err = ERR_INVALID_COLOR;
                        return ERR_INVALID_COLOR;
                    }
                    break;
                case ARG_FONTNAME:
                    // Look up in font names dictionary
                    for (size_t i = 0; i < sizeof fontnames / sizeof *fontnames; ++i)
                    {
                        if (strcmp(token, fontnames[i].name) == 0)
                        {
                            found = true;
                            pins->fnm = &fontnames[i];  // save the fontname pointer
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pins->err = ERR_INVALID_FONTNAME;
                        return ERR_INVALID_FONTNAME;
                    }
                    break;
                case ARG_FONTSTYLE:
                    // Look up in font styles dictionary
                    for (size_t i = 0; i < sizeof fontstyles / sizeof *fontstyles; ++i)
                    {
                        if (strcmp(token, fontstyles[i].name) == 0)
                        {
                            found = true;
                            pins->fst = &fontstyles[i];  // save the fontstyle pointer
                            break;  // end of for loop
                        }
                    }
                    if (!found)
                    {
                        pins->err = ERR_INVALID_FONTSTYLE;
                        return ERR_INVALID_FONTSTYLE;
                    }
                    break;
                case ARG_TEXT:
                    free(pins->txt);  // free potentially already allocated memory
                    pins->txt = strdup(token);  // duplicate the text
                    if (pins->txt == NULL)
                    {
                        pins->err = ERR_OUT_OF_MEMORY;
                        return ERR_OUT_OF_MEMORY;
                    }
                    break;
                default:
                    // Other types of non-int argument: no further processing
                    break;
                }
            }
            // Restore token length if it had trailing spaces
            if (last != end)
                *last = ' ';
        }
        else
        {
            pins->err = ERR_TOO_MANY_ARG;
            return ERR_TOO_MANY_ARG;
        }
        argindex++;
        token = strtok(NULL, ",");
    }

    // What have we found?
    if (this == NULL)
    {
        pins->err = ERR_COMMAND_NOT_FOUND;
        return ERR_COMMAND_NOT_FOUND;
    }
    if (argindex < this->argcount - 1)
    {
        pins->err = ERR_MISSING_ARG;
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
            if ((err = run_cmd(&instr)) != ERR_OK)
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
