////////// Includes, Defines //////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NAME 64
#define SIZE 250

////////// Globals ////////////////////////////////////////////////////////////

static char *name[SIZE];
static unsigned char ord[SIZE], lot[SIZE], stk[SIZE];
static unsigned char sp = 0, size = 0, allowloop = 1;

////////// Function Declarations //////////////////////////////////////////////

void init(void);
unsigned char swap(unsigned char, unsigned char);
unsigned char push(unsigned char);
unsigned char pop(void);
unsigned char del(unsigned char);
void draw(void);
void print(void);

////////// Function Definitions ///////////////////////////////////////////////

// Initialize random number generator and global arrays
void init(void)
{
	unsigned char i;

	#if defined(__APPLE__) && defined(__MACH__)
	srandomdev();
	#elif defined(__linux__)
	srandom((unsigned int)time(NULL));
	#else
	srand((unsigned int)time(NULL));
	#endif
	for (i = 0; i < SIZE; ++i) {
		name[i] = NULL;
		ord[i] = 0;
		lot[i] = 0;
		stk[i] = 0;
	}
}

// Read data from file, fill stack
unsigned char readfile(void)
{
	unsigned char lines = 0;
	FILE *fp;
	size_t n = 0;

	if ((fp = fopen("lot.txt", "r")) != NULL) {
		while (lines < SIZE && getline(&name[lines], &n, fp) != -1)
			push(lines++);
		fclose(fp);
	} else
		printf("Error: file not found (lot.txt)\n");
	return lines;
}

// Swap two elements on the stack
// Arg: index of the elements to swap
// Ret: 1 = ok, 0 = error
unsigned char swap(unsigned char i, unsigned char j)
{
	unsigned char t;

	if (i != j && i < size && j < size) {
		t = stk[i];
		stk[i] = stk[j];
		stk[j] = t;
		return 1;
	}
	return 0;
}

// Push an element onto the stack
// Arg: value to push
// Ret: 1 = ok, 0 = error
unsigned char push(unsigned char n)
{
	if (sp < SIZE) {
		stk[sp++] = n;
		return 1;  // ok
	}
	return 0;  // error
}

// Pop a random element from the stack
// Ret: value of popped element, or 255 = -1 = error
unsigned char pop(void)
{
	unsigned char k;

	if ((k = sp)) {      // check if not zero, remember current stack pointer
		if (--sp)        // adjust stack pointer, check if not zero
			swap(random() % k, sp);  // swap a random element to the top
		return stk[sp];  // return top element
	}
	return 255;  // error
}

// Remove a specific element from the stack
// Arg: value of element to remove
// Ret: value of removed element, or 255 = -1 = error
unsigned char del(unsigned char n)
{
	unsigned char i;

	for (i = 0; i < sp; ++i)     // traverse whole stack
		if (stk[i] == n) {       // if element found
			stk[i] = stk[--sp];  // adjust stack pointer, save top element
			return n;            // ok
		}
	return 255;  // error
}

// Pull a straw for everyone
void draw(void)
{
	unsigned char i, loopstart = 0, p, q;

	ord[0] = p = pop();
	if (p == 255) {
		printf("Error: draw::pop(0)\n");
		return;
	}

	for (i = 0; i < size - 1; ++i) {

		if (i - loopstart == allowloop && i <= size - allowloop - 2) {
			if (!push(p)) {
				printf("Error: draw::push(%u)\n", i);
				return;
			}
		}

		if (i - loopstart >= allowloop && i == size - allowloop - 2) {

			lot[i] = q = del(p);
			if (q == 255) {
				printf("Error: draw::del(%u)\n", i);
				return;
			}

		} else {

			lot[i] = q = pop();
			if (q == 255) {
				printf("Error: draw::pop(%u)\n", i);
				return;
			}
		}

		if (p == q) {
			ord[i + 1] = p = pop();
			if (p == 255) {
				printf("Error: draw::pop(%u)\n", i + 1);
				return;
			}
			loopstart = i + 1;
		} else
			ord[i + 1] = lot[i];
	}

	lot[size - 1] = p;
}

// Output result
void print(void)
{
	char buf[NAME], *s, *t;
	unsigned char i, n, m = 0;

	for (i = 0; i < size; ++i)
		if ((n = (unsigned char)strlen(name[i])) > m)
			m = n;

	for (i = 0; i < size; ++i) {

		s = buf;
		t = name[ord[i]];
		while (*t && *t != '\r' && *t != '\n')
			*s++ = *t++;
		*s = '\0';
		printf("%*s -> ", m, buf);

		s = buf;
		t = name[lot[i]];
		while (*t && *t != '\r' && *t != '\n')
			*s++ = *t++;
		*s = '\0';
		printf("%-*s\n", m, buf);
	}
}

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	unsigned char i;

	if (argc > 1)
		allowloop = atoi(argv[1]);

	init();
	if ((size = readfile())) {
		draw();
		print();
		for (i = 0; i < size; ++i)
			free(name[i]);
	}
	return 0;
}
