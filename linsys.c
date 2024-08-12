#include <stdio.h>
#include <stdlib.h>  // calloc, free
#include <string.h>  // strcmp
#include <stdbool.h>

#define ALPHLEN  ('z' - 'a' + 1)
#define NAMELEN  "3"  // max variable name length as format string
#define NAMESIZE  4   // max varlen+NUL as size parameter
#define MAXDIM 1024   // arbitrary limit for rows/cols

typedef struct var {
    char name[NAMESIZE];
    int id, row, col;
} Var;

static int rows, cols;
static Var *var;  // input matrix variable names
static int *sum;  // sum values of rows and columns

static inline int hashvarname(const char *const s)
{
    return (s[0] - 'a') * ALPHLEN + s[1] - 'a';
}

static int cmp_name(const void *p, const void *q)
{
    const Var *const a = p;
    const Var *const b = q;
    return strcmp(a->name, b->name);
}

static int cmp_pos(const void *p, const void *q)
{
    const Var *const a = p;
    const Var *const b = q;
    if (a->row < b->row) return -1;
    if (a->row > b->row) return  1;
    if (a->col < b->col) return -1;
    if (a->col > b->col) return  1;
    return 0;
}

// Sets: rows, cols, var, sum
static bool readinput(void)
{
    bool ret = false;

    // Open file
    FILE *f = fopen("linsys.txt", "r");
    if (!f) {
        fprintf(stderr, "File not found.\n");
        goto err;
    }

    // Read dimensions
    fscanf(f, "%d %d", &rows, &cols);
    if (rows < 1 || cols < 1 || rows > MAXDIM || cols > MAXDIM) {
        fprintf(stderr, "Wrong input size: %d,%d\n", rows, cols);
        goto err;
    }

    // Allocate memory for matrix with variable names, and sum values
    var = calloc(rows * cols, sizeof *var);
    sum = calloc(rows + cols, sizeof *sum);
    if (!var || !sum) {
        fprintf(stderr, "Out of memory.\n");
        goto err;
    }

    // Read matrix
    int success = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const int k = i * cols + j;
            success += fscanf(f, "%"NAMELEN"s", &var[k].name[0]);
            var[k].id = hashvarname(var[k].name);
            var[k].row = i;
            var[k].col = j;
        }
        success += fscanf(f, "%d", &sum[i]);
    }
    for (int j = 0; j < cols; ++j)
        success += fscanf(f, "%d", &sum[rows + j]);
    if (success != rows * (cols + 1) + cols) {
        fprintf(stderr, "Error reading input data.\n");
        goto err;
    }
    ret = true;
    goto ok;

err:
    rows = 0;
    cols = 0;
    free(var);
    free(sum);
    var = NULL;
    sum = NULL;
ok:
    fclose(f);
    return ret;
}

static void showinput(void)
{
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const int k = i * cols + j;
            printf("%"NAMELEN"s", var[k].name);
        }
        printf(" |%"NAMELEN"d\n", sum[i]);
    }
    for (int j = 0; j < cols; ++j)
        printf("---");
    printf("-+---\n");
    for (int j = 0; j < cols; ++j)
        printf("%"NAMELEN"d", sum[rows + j]);
    printf(" |\n\n");
}

int main(void)
{
    if (!readinput())
        return 1;
    showinput();

    // Count unique variables
    int varcount = 0;
    int curvar = -1;
    qsort(var, rows * cols, sizeof *var, cmp_name);
    for (int i = 0; i < rows * cols; ++i)
        if (var[i].id != curvar) {
            curvar = var[i].id;
            ++varcount;
        }
    if (varcount < 1)
        goto err1;
    int *varids = calloc(varcount, sizeof *varids);
    if (!varids)
        goto err2;

    // Build augmented matrix
    int arows = rows + cols;
    int acols = varcount + 1;
    int *mat = calloc(arows * acols, sizeof *mat);
    if (!mat)
        goto err3;
    qsort(var, rows * cols, sizeof *var, cmp_pos);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const int k = i * cols + j;
        }
    }

err3:
    free(mat);
err2:
    free(varids);
err1:
    free(sum);
    free(var);
    return 0;
}
