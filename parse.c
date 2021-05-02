#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strcmp, strncmp
#include <stdint.h>  // uint8_t, int32_t, uint32_t
#include <stdbool.h> // bool, true, false
#include <ctype.h>   // isspace

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

////////// API Definitions ////////////////////////////////////////////////////

typedef enum {
    API_ERR_SIGNED =   -1,
    API_ERR_OK     =    0,
    API_ERR_COLOR  = 0x80,
    API_ERR_FONTNAME,
    API_ERR_FONTSTYLE,
    API_ERR_RANGE_X,
    API_ERR_RANGE_Y,
} API_Err;

typedef enum {
    API_CMD_SIGNED = -1,
    API_CMD_CLS    =  0,
    API_CMD_PIXEL,
    API_CMD_LINE,
    API_CMD_RECT,
    API_CMD_FIGURE,
    API_CMD_CIRCLE,
    API_CMD_ELLIPSE,
    API_CMD_TEXT,
    API_CMD_BITMAP,
    API_CMD_WAIT,
    API_CMD_REPEAT,
    API_CMD_SCROLL_U,
    API_CMD_SCROLL_D,
    API_CMD_SCROLL_L,
    API_CMD_SCROLL_R,
    API_CMD_COLORS,
} API_Cmd;

typedef enum {
    API_ARG_SIGNED = -1,
    API_ARG_COLOR  =  0,  // text : colour
    API_ARG_FONTNAME,     // text : font name
    API_ARG_FONTSTYLE,    // text : font style
    API_ARG_TEXT,         // text : general text
    API_ARG_INT,          // int  : general integer
    API_ARG_POSX,         // int  : x coordinate
    API_ARG_POSY,         // int  : y coordinate
    API_ARG_DIMX,         // int  : width
    API_ARG_DIMY,         // int  : height
    API_ARG_WEIGHT,       // int  : line weight
    API_ARG_FILLED,       // int  : filled yes/no
    API_ARG_RADIUS,       // int  : radius
    API_ARG_FONTSIZE,     // int  : font size
    API_ARG_BMPID,        // int  : bitmap ID
    API_ARG_MSEC,         // int  : milliseconds
    API_ARG_REPNUM,       // int  : repetition number
    API_ARG_REPCOUNT,     // int  : repetition count
} API_Arg;

typedef enum {
    API_COL_SIGNED   = -1,
    API_COL_BLACK    = 0b00000000,
    API_COL_BLUE     = 0b00000010,
    API_COL_GREEN    = 0b00010000,
    API_COL_CYAN     = 0b00010010,
    API_COL_RED      = 0b10000000,
    API_COL_MAGENTA  = 0b10000010,
    API_COL_BROWN    = 0b01000100,
    API_COL_GREY     = 0b10010010,
    API_COL_DARKGREY = 0b01001001,
    API_COL_LBLUE    = 0b01001111,
    API_COL_LGREEN   = 0b01011101,
    API_COL_LCYAN    = 0b01011111,
    API_COL_LRED     = 0b11101001,
    API_COL_LMAGENTA = 0b11101011,
    API_COL_YELLOW   = 0b11111100,
    API_COL_WHITE    = 0b11111111,
} API_Col;

typedef enum {
    API_FNM_SIGNED = -1,
    API_FNM_ARIAL,
    API_FNM_CONSOLAS,
} API_Fnm;

typedef enum {
    API_FST_SIGNED = -1,
    API_FST_NORMAL,
    API_FST_BOLD,
    API_FST_ITALIC,
} API_Fst;

////////// My Definitions /////////////////////////////////////////////////////

#define BUFLEN     (128U)
#define MAX_ARGS    (12U)
#define MAX_DIGITS   (9U)

////////// My Errors //////////////////////////////////////////////////////////

typedef enum {
    ERR_SIGNED = -1,  // make sure type is signed int
    ERR_OK     =  0,
    ERR_NOTFOUND,
    ERR_SYNTAX,
    ERR_COMMAND,
    ERR_COLOR,
    ERR_FONTNAME,
    ERR_FONTSTYLE,
    ERR_NUMBER,
    ERR_LINE_TOO_LONG,
    ERR_EMPTY_ARG,
    ERR_MISSING_ARG,
    ERR_TOO_MANY_ARG,
    ERR_OUT_OF_MEMORY,
    ERR_UNKNOWN_ERROR,
} ErrType;

////////// My Dictionaries ////////////////////////////////////////////////////

typedef struct {
    int   id;
    char *name;
} Dict, *pDict;

static const Dict commands[] = {
    { .id = API_CMD_CLS,      .name = "clearscherm"  },
    { .id = API_CMD_PIXEL,    .name = "punt"         },
    { .id = API_CMD_LINE,     .name = "lijn"         },
    { .id = API_CMD_RECT,     .name = "rechthoek"    },
    { .id = API_CMD_FIGURE,   .name = "figuur"       },
    { .id = API_CMD_CIRCLE,   .name = "cirkel"       },
    { .id = API_CMD_ELLIPSE,  .name = "ellips"       },
    { .id = API_CMD_TEXT,     .name = "tekst"        },
    { .id = API_CMD_BITMAP,   .name = "bitmap"       },
    { .id = API_CMD_WAIT,     .name = "wacht"        },
    { .id = API_CMD_REPEAT,   .name = "herhaal"      },
    { .id = API_CMD_SCROLL_U, .name = "omhoog"       },
    { .id = API_CMD_SCROLL_D, .name = "omlaag"       },
    { .id = API_CMD_SCROLL_L, .name = "links"        },
    { .id = API_CMD_SCROLL_R, .name = "rechts"       },
    { .id = API_CMD_COLORS,   .name = "kleuren"      },
};

static const Dict colornames[] = {
    { .id = API_COL_BLACK,    .name = "zwart"        },
    { .id = API_COL_BLUE,     .name = "blauw"        },
    { .id = API_COL_GREEN,    .name = "groen"        },
    { .id = API_COL_CYAN,     .name = "cyaan"        },
    { .id = API_COL_RED,      .name = "rood"         },
    { .id = API_COL_MAGENTA,  .name = "magenta"      },
    { .id = API_COL_BROWN,    .name = "bruin"        },
    { .id = API_COL_GREY,     .name = "grijs"        },
    { .id = API_COL_DARKGREY, .name = "donkergrijs"  },
    { .id = API_COL_LBLUE,    .name = "lichtblauw"   },
    { .id = API_COL_LGREEN,   .name = "lichtgroen"   },
    { .id = API_COL_LCYAN,    .name = "lichtcyaan"   },
    { .id = API_COL_LRED,     .name = "lichtrood"    },
    { .id = API_COL_LMAGENTA, .name = "lichtmagenta" },
    { .id = API_COL_YELLOW,   .name = "geel"         },
    { .id = API_COL_WHITE,    .name = "wit"          },
};

static const Dict fontnames[] = {
    { .id = API_FNM_ARIAL,    .name = "arial"        },
    { .id = API_FNM_CONSOLAS, .name = "consolas"     },
};

static const Dict fontstyles[] = {
    { .id = API_FST_NORMAL,   .name = "normaal"      },
    { .id = API_FST_BOLD,     .name = "vet"          },
    { .id = API_FST_ITALIC,   .name = "cursief"      },
};

////////// My Script Syntax ///////////////////////////////////////////////////

typedef struct {
    API_Cmd id;
    int     argc;
    int     optc;
    API_Arg type[MAX_ARGS];
} Syntax, *pSyntax;

static const Syntax syntax[] = {
    { .id = API_CMD_CLS,      .argc =  1, .optc = 0, .type = {API_ARG_COLOR} },
    { .id = API_CMD_PIXEL,    .argc =  3, .optc = 0, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_COLOR} },
    { .id = API_CMD_LINE,     .argc =  6, .optc = 1, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_POSX,API_ARG_POSY,API_ARG_COLOR,API_ARG_WEIGHT} },
    { .id = API_CMD_RECT,     .argc =  6, .optc = 2, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_DIMX,API_ARG_DIMY,API_ARG_COLOR,API_ARG_FILLED} },
    { .id = API_CMD_FIGURE,   .argc = 11, .optc = 1, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_POSX,API_ARG_POSY,API_ARG_POSX,API_ARG_POSY,API_ARG_POSX,API_ARG_POSY,API_ARG_POSX,API_ARG_POSY,API_ARG_COLOR} },
    { .id = API_CMD_CIRCLE,   .argc =  4, .optc = 1, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_RADIUS,API_ARG_COLOR} },
    { .id = API_CMD_ELLIPSE,  .argc =  5, .optc = 1, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_RADIUS,API_ARG_RADIUS,API_ARG_COLOR} },
    { .id = API_CMD_TEXT,     .argc =  7, .optc = 1, .type = {API_ARG_POSX,API_ARG_POSY,API_ARG_COLOR,API_ARG_TEXT,API_ARG_FONTNAME,API_ARG_FONTSIZE,API_ARG_FONTSTYLE} },
    { .id = API_CMD_BITMAP,   .argc =  3, .optc = 0, .type = {API_ARG_BMPID,API_ARG_POSX,API_ARG_POSY} },
    { .id = API_CMD_WAIT,     .argc =  1, .optc = 0, .type = {API_ARG_MSEC} },
    { .id = API_CMD_REPEAT,   .argc =  2, .optc = 1, .type = {API_ARG_REPNUM,API_ARG_REPCOUNT} },
    { .id = API_CMD_SCROLL_U, .argc =  1, .optc = 0, .type = {API_ARG_INT} },
    { .id = API_CMD_SCROLL_D, .argc =  1, .optc = 0, .type = {API_ARG_INT} },
    { .id = API_CMD_SCROLL_L, .argc =  1, .optc = 0, .type = {API_ARG_INT} },
    { .id = API_CMD_SCROLL_R, .argc =  1, .optc = 0, .type = {API_ARG_INT} },
    { .id = API_CMD_COLORS,   .argc =  0, .optc = 0 },
};

////////// My Parsed Command = Instruction ////////////////////////////////////

typedef struct {
    ErrType err;  // error code
    int     ord;  // ordinal command counter
    pDict   cmd;  // pointer to command dict
    pDict   col;  // pointer to colour dict
    pDict   fnm;  // pointer to font name dict
    pDict   fst;  // pointer to font style dict
    int     val[MAX_ARGS];
    char   *txt;  // dynamically allocated when needed => free after use!
} Instr, *pInstr;

////////// Fake API Functions /////////////////////////////////////////////////

static API_Err API_clearscreen(int color)
{
    printf("API_clearscreen(%i)\n", color);
    return API_ERR_OK;
}

static API_Err API_draw_pixel(int x, int y, int color)
{
    printf("API_draw_pixel(%i, %i, %i)\n", x, y, color);
    return API_ERR_OK;
}

static API_Err API_draw_line(int x1, int y1, int x2, int y2, int color, int weight, int reserved)
{
    printf("API_draw_line(%i, %i, %i, %i, %i, %i, %i)\n", x1, y1, x2, y2, color, weight, reserved);
    return API_ERR_OK;
}

static API_Err API_draw_rectangle(int x, int y, int width, int height, int color, int filled, int reserved1, int reserved2)
{
    printf("API_draw_rectangle(%i, %i, %i, %i, %i, %i, %i, %i)\n", x, y, width, height, color, filled, reserved1, reserved2);
    return API_ERR_OK;
}

static API_Err API_draw_text(int x_lup, int y_lup, int color, char *text, char *fontname, int fontsize, int fontstyle, int aligned)
{
    printf("API_draw_text(%i, %i, %i, \"%s\", \"%s\", %i, %i, %i)\n", x_lup, y_lup, color, text, fontname, fontsize, fontstyle, aligned);
    return API_ERR_OK;
}

static API_Err API_draw_bitmap(int x_lup, int y_lup, int bm_nr)
{
    printf("API_draw_bitmap(%i, %i, %i)\n", x_lup, y_lup, bm_nr);
    return API_ERR_OK;
}

static API_Err API_wait(int msecs)
{
    printf("API_wait(%i)\n", msecs);
    return API_ERR_OK;
}

static API_Err API_repeat_commands(int nr_previous_commands, int iterations, int reserved)
{
    printf("API_repeat_commands(%i, %i, %i)\n", nr_previous_commands, iterations, reserved);
    return API_ERR_OK;
}

static API_Err API_draw_ellipse(int x, int y, int r_hor, int r_ver, int color, int filled)
{
    printf("API_draw_ellipse(%i, %i, %i, %i, %i, %i)\n", x, y, r_hor, r_ver, color, filled);
    return API_ERR_OK;
}

static API_Err API_draw_circle(int x, int y, int radius, int color, int filled)
{
    printf("API_draw_circle(%i, %i, %i, %i, %i)\n", x, y, radius, color, filled);
    return API_ERR_OK;
}

static API_Err API_draw_figure(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5, int color, int weight)
{
    printf("API_draw_figure(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)\n", x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, color, weight);
    return API_ERR_OK;
}

static API_Err API_scroll_up(int amount)
{
    printf("API_scroll_up(%i)\n", amount);
    return API_ERR_OK;
}

static API_Err API_scroll_down(int amount)
{
    printf("API_scroll_down(%i)\n", amount);
    return API_ERR_OK;
}

static API_Err API_scroll_left(int amount)
{
    printf("API_scroll_left(%i)\n", amount);
    return API_ERR_OK;
}

static API_Err API_scroll_right(int amount)
{
    printf("API_scroll_right(%i)\n", amount);
    return API_ERR_OK;
}

static API_Err API_color_chart(void)
{
    printf("API_color_chart()\n");
    return API_ERR_OK;
}

////////// Functions //////////////////////////////////////////////////////////

// Run a fully prepared instruction
static ErrType run_cmd(const pInstr instr)
{
    // Null pointer?
    if (instr == NULL)
        return ERR_COMMAND;

    // Fout in de struct?
    if (instr->err != ERR_OK)
        return instr->err;

    // Command counter
    printf("%i: ", instr->ord);

    // Roep de juiste API-lib functie aan
    switch (instr->cmd->id)
    {
    case API_CMD_CLS:
        return (ErrType)API_clearscreen(
            instr->col->id);               // colour
    case API_CMD_PIXEL:
        return (ErrType)API_draw_pixel(
            instr->val[0], instr->val[1],  // x, y
            instr->col->id);               // colour
    case API_CMD_LINE:
        return (ErrType)API_draw_line(
            instr->val[0], instr->val[1],  // x1, y1
            instr->val[2], instr->val[3],  // x2, y2
            instr->col->id,                // colour
            instr->val[4],                 // weight
            instr->val[5]);                // reserved
    case API_CMD_RECT:
        return (ErrType)API_draw_rectangle(
           instr->val[0], instr->val[1],   // x1, y1
           instr->val[2], instr->val[3],   // width, height
           instr->col->id,                 // colour
           instr->val[4],                  // filled
           instr->val[5], instr->val[6]);  // reserved
    case API_CMD_FIGURE:
        return (ErrType)API_draw_figure(
            instr->val[0], instr->val[1],  // x1, y1
            instr->val[2], instr->val[3],  // x2, y2
            instr->val[4], instr->val[5],  // x3, y3
            instr->val[6], instr->val[7],  // x4, y4
            instr->val[8], instr->val[9],  // x5, y5
            instr->col->id,                // colour
            instr->val[10]);               // weight
    case API_CMD_CIRCLE:
        return (ErrType)API_draw_circle(
            instr->val[0], instr->val[1],  // x, y
            instr->val[2],                 // radius
            instr->col->id,                // colour
            instr->val[3]);                // filled
    case API_CMD_ELLIPSE:
        return (ErrType)API_draw_ellipse(
            instr->val[0], instr->val[1],  // x, y
            instr->val[2], instr->val[3],  // r_hor, r_ver
            instr->col->id,                // colour
            instr->val[4]);                // filled
    case API_CMD_TEXT:
        return (ErrType)API_draw_text(
            instr->val[0], instr->val[1],  // x, y
            instr->col->id,                // colour
            instr->txt,                    // text
            instr->fnm->name,              // font name
            instr->val[2],                 // font size
            instr->fst->id,                // font style
            instr->val[3]);                // aligned
    case API_CMD_BITMAP:
        return (ErrType)API_draw_bitmap(
            instr->val[1], instr->val[2],  // x, y
            instr->val[0]);                // bitmap ID
    case API_CMD_WAIT:
        return (ErrType)API_wait(
            instr->val[0]);                // milliseconds
    case API_CMD_REPEAT:
        return (ErrType)API_repeat_commands(
            instr->val[0],                 // count
            instr->val[1],                 // loops
            instr->val[2]);                // reserved
    case API_CMD_SCROLL_U:
        return (ErrType)API_scroll_up(
            instr->val[0]);                // amount
    case API_CMD_SCROLL_D:
        return (ErrType)API_scroll_down(
            instr->val[0]);                // amount
    case API_CMD_SCROLL_L:
        return (ErrType)API_scroll_left(
            instr->val[0]);                // amount
    case API_CMD_SCROLL_R:
        return (ErrType)API_scroll_right(
            instr->val[0]);                // amount
    case API_CMD_COLORS:
        return (ErrType)API_color_chart();
    default:
        return ERR_COMMAND;
    }
}

// Convert to integer
static ErrType str_to_int(const char *s, int *n)
{
    // Check strlen to avoid overflow = undefined result
    if (strlen(s) > MAX_DIGITS)
        return ERR_NUMBER;

    int val = atoi(s);

    // Check if zero was really zero, or if the conversion failed
    // TODO: this will fail on potentially valid represenations such as "+0", "00"
    if (val == 0 && strcmp(s, "0"))
        return ERR_NUMBER;

    *n = val;
    return ERR_OK;
}

// Look up in dictionary
static ErrType find_in_dict(const Dict dict[], size_t items, const char *s, pDict *p)
{
    for (size_t i = 0; i < items; ++i)
        if (strcmp(s, dict[i].name) == 0)
        {
            *p = (pDict)&dict[i];
            return ERR_OK;
        }
    return ERR_NOTFOUND;
}

// Look up in syntax defs
static ErrType find_in_syntax(API_Cmd id, pSyntax *p)
{
    for (size_t i = 0; i < sizeof syntax / sizeof *syntax; ++i)
        if (id == syntax[i].id)
        {
            *p = (pSyntax)&syntax[i];
            return ERR_OK;
        }
    return ERR_SYNTAX;
}

// Parse a line of text into a complete instruction
static ErrType parse(const char *line, pInstr pinstr)
{
    static int cmdcount = 0;  // global command counter

    char buf[BUFLEN] = {0};  // local buffer to modify the line
    pDict pcmd = NULL;       // pointer to current command
    pSyntax psyn = NULL;     // pointer to syntax info of current command
    int argindex = -1;       // current argument index (command doesn't count, so start at -1)
    int intindex = 0;        // current integer argument index
    int val = 0;             // converted integer argument
    ErrType e = ERR_OK;      // error code

    if (strlen(line) >= BUFLEN)  // also check equal to leave space for null-terminator
    {
        pinstr->err = ERR_LINE_TOO_LONG;
        return ERR_LINE_TOO_LONG;
    }

    // Reset error in instruction
    pinstr->err = ERR_OK;

    // Copy line because strtok wants to modify it
    strcpy(buf, line);

    // Find index to start of first field in comma-delimited line.
    // Normally, this would simply be the first char of the line;
    // but this also replaces all commas with null chars.
    char *token = strtok(buf, ",");

    // As long as there are comma-delimited fields in the line
    while (token)
    {
        // Is this the first token? Then it should be a command
        if (argindex == -1)
        {
            // Find the command
            if ((e = find_in_dict(commands, sizeof commands / sizeof *commands, token, &pcmd)) != ERR_OK)
            {
                pinstr->err = e;
                return e;
            }
            // Find corresponding syntax def
            if ((e = find_in_syntax(pcmd->id, &psyn)) != ERR_OK)
            {
                pinstr->err = e;
                return e;
            }
            pinstr->ord = cmdcount++;  // save ordinal number to struct
            pinstr->cmd = pcmd;  // save command pointer to instruction
        }
        else if (argindex < (psyn->argc + psyn->optc))
        {
            // Skip leading spaces
            while (isspace(*token))
                ++token;

            // Skip trailing spaces
            // (token length will be restored at the end of this part)
            char *end = token + strlen(token); // one past the last char
            char *last = end - 1;              // the last char
            // As long as space and we're not before start, go to previous char
            while (last >= token && isspace(*last))
                last--;
            // We're now 1 before the first trailing space, so first compensate
            // by adding one, then check if we're not at the end. If not,
            // we found spaces and they need to be cut.
            if (++last != end)
                *last = '\0';

            // Anything left?
            if (strlen(token) == 0)
            {
                pinstr->err = ERR_EMPTY_ARG;
                return ERR_EMPTY_ARG;
            }

            // Save argument type for convenience
            // Optional arguments are always integer
            API_Arg type = argindex < psyn->argc ? psyn->type[argindex] : API_ARG_INT;

            switch (type)
            {
            case API_ARG_COLOR:
                // Look up in colour names dictionary
                if ((e = find_in_dict(colornames, sizeof colornames / sizeof *colornames, token, &pinstr->col)) != ERR_OK)
                {
                    pinstr->err = ERR_COLOR;
                    return ERR_COLOR;
                }
                break;
            case API_ARG_FONTNAME:
                // Look up in font names dictionary
                if ((e = find_in_dict(fontnames, sizeof fontnames / sizeof *fontnames, token, &pinstr->fnm)) != ERR_OK)
                {
                    pinstr->err = ERR_FONTNAME;
                    return ERR_FONTNAME;
                }
                break;
            case API_ARG_FONTSTYLE:
                // Look up in font styles dictionary
                if ((e = find_in_dict(fontstyles, sizeof fontstyles / sizeof *fontstyles, token, &pinstr->fst)) != ERR_OK)
                {
                    pinstr->err = ERR_FONTSTYLE;
                    return ERR_FONTSTYLE;
                }
                break;
            case API_ARG_TEXT:
                if (pinstr->txt != NULL)
                    free(pinstr->txt);  // free potentially already allocated memory
                pinstr->txt = strdup(token);  // duplicate the text
                if (pinstr->txt == NULL)
                {
                    pinstr->err = ERR_OUT_OF_MEMORY;
                    return ERR_OUT_OF_MEMORY;
                }
                break;
            default:
                // Convert to int
                if ((e = str_to_int(token, &val)) != ERR_OK)
                {
                    pinstr->err = e;
                    return e;
                }

                // So it's a good value, but does it fit?
                if (intindex >= (int)MAX_ARGS)
                {
                    pinstr->err = ERR_TOO_MANY_ARG;
                    return ERR_TOO_MANY_ARG;
                }

                // All OK, store it
                pinstr->val[intindex++] = val;
                break;
            }

            // Restore token length if it had trailing spaces
            if (last != end)
                *last = ' ';
        }
        else
        {
            // More arguments than possible (incl optional)
            pinstr->err = ERR_TOO_MANY_ARG;
            return ERR_TOO_MANY_ARG;
        }

        // Next argument
        argindex++;
        token = strtok(NULL, ",");
    }

    // Did we find *anything*?
    if (pcmd == NULL)
    {
        pinstr->err = ERR_COMMAND;
        return ERR_COMMAND;
    }

    // Enough arguments?
    if (argindex < psyn->argc)
    {
        // Fewer than the mandatory number of args
        pinstr->err = ERR_MISSING_ARG;
        return ERR_MISSING_ARG;
    }

    // Reset missing optional args
    while (argindex++ < (psyn->argc + psyn->optc))
        pinstr->val[intindex++] = 0;

    // All done & OK
    return ERR_OK;
}

////////// Main ///////////////////////////////////////////////////////////////

int main(void)
{
    ErrType err = ERR_OK;
    Instr instr = {0};

    for (size_t i = 0; i < sizeof script / sizeof *script; ++i)
    {
        // Input
        printf("\nInput: \"%s\"\n", script[i]);

        // Parse
        if ((err = parse(script[i], &instr)) != ERR_OK)
        {
            printf("Parse error: %u\n", err);
            continue;
        }

        // Run
        if ((err = run_cmd(&instr)) != ERR_OK)
            printf("Run error: %u\n", err);
    }

    // Clean up memory allocation
    if (instr.txt != NULL)
        free(instr.txt);

    return 0;
}
