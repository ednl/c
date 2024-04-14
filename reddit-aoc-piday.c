/**
 * 2024 Pi Day celebration puzzle in the style of Advent of Code.
 * https://www.reddit.com/r/adventofcode/comments/1bejcvc/pi_coding_quest/
 * https://ivanr3d.com/projects/pi/
 * Given: message encoded with a shift cipher based on the first 16 digits
 * of pi repeating. Only the letter characers are shifted.
 * Part 1: Decode the message.
 * Part 2: Find all words of numbers 1 to 10 in the text, possibly split up
 *         by non-letter characters, and multiply the numbers found.
 */

#include <stdio.h>
#include <string.h>  // strstr

#define ALPH ('z' - 'a' + 1)  // 26 letters
#define CASE 32  // lowercase = uppercase | 32
#define BASE 96  // bitmask to select base of alphabet: 'A' - 1 or 'a' - 1
#define NIBBLE ((1 << 4) - 1)  // 4 bits

static const char pi[] = "314159265358979323846264338327950288";  // copied from math.h::M_PI
static const char *num[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
static char msg[] = "Wii kxtszof ova fsegyrpm d lnsrjkujvq roj! Kdaxii svw vnwhj pvugho buynkx tn vwh-gsvw ruzqia."
    " Mrq'x kxtmjw bx fhlhlujw cjoq! Hmg tyhfa gx dwd fdqu bsm osynbn oulfrex, kahs con vjpmd qtjv bx whwxssp cti"
    " hmulkudui f Jgusd Yp Gdz!";

int main(void)
{
    // Decode message and save lowercased letters to continuous string.
    char letters[256] = {0};  // large enough to contain all letters of msg
    int i = 0, count = 0;
    for (char *s = msg; *s; ++s, ++i)
        if (*s >= 'A') {
            const char base = (*s & BASE) | 1;  // 'A' or 'a' (assume ASCII)
            const char shift = pi[i & NIBBLE] & NIBBLE;  // index 0..15 repeating, convert '0'..'9' to 0..9
            *s = base + (*s - base + ALPH - shift) % ALPH;  // decode by subtracting shift amount
            letters[count++] = *s | CASE;  // save lowercase letters
        }
    printf("Part 1: \"%s\"\n", msg);  // The formula for crafting a delightful pie! Cutoff our three golden apples of
                                      // one-four pounds. Don't forget to weighten well! Add sugar as you want and
                                      // invite friends, even the silly ones to network and celebrate a Happy Pi Day!

    // Multiply all number words in message.
    int prod = 1;
    for (int n = 2; n <= 10; ++n) {  // "zero" only for alignment, skip product identity "one"
        const size_t len = strlen(num[n]);
        for (const char *s = letters; (s = strstr(s, num[n])); s += len)  // strstr = NULL when not found
            prod *= n;
    }
    printf("Part 2: %d\n", prod);  // 53760
    return 0;
}
