#include <stdlib.h>  // uint32_t
#include <stdio.h>   // printf

unsigned char printnum(uint32_t, unsigned char);

// Print positive integer in base 2..16
// Authour: E. Dronkert, HU Electrical Engineering 2018-2022
// Arg: num  = value of the number to be printed, unsinged int
//      base = base of the printed number, min=2 max=16
// Ret: number of characters printed (strlen)
unsigned char printnum(uint32_t num, unsigned char base)
{
	static char *digit =
		"0123456789ABCDEF";    // 16 different digits for base 16
	char s[32];                // uint32_t in base 2 has 32 digits
	unsigned char len, i = 0;  // position counter

	// stop if base is meaningless or unsupported
	if (base < 2 || base > 16)
		return 0;  // nothing printed

	do {                             // at least one loop (in case num == 0)
		s[i++] = digit[num % base];  // fill array with number in reverse
		num /= base;                 // factor the base out ("shift right")
	} while (num);                   // until nothing left

	len = i;                   // remember the character count
	while (i)                  // for every character (min=1, max=32)
		printf("%c", s[--i]);  // print array in reverse
	return len;                // return number of characters printed
}
/*
// Version for STM32F4 ////////////////////////////////////////////////////////
unsigned char UART_putnum(INT32U num, unsigned char base)
{
	static char *digit =
		"0123456789ABCDEF";    // 16 different digits for base 16
	char s[32];                // INT32U in base 2 has max 32 digits
	unsigned char len, i = 0;  // position counter

	// stop if base is meaningless or unsupported
	if (base < 2 || base > 16)
		return 0;  // nothing printed

	do {                             // at least one loop (in case num == 0)
		s[i++] = digit[num % base];  // fill array with number in reverse
		num /= base;                 // factor the base out ("shift right")
	} while (num);                   // until nothing left

	len = i;     // remember the character count
	while (i)    // for every character (min=1, max=32)
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;    // wait until tx buffer empty
		USART_SendData(USART2, s[--i]);  // print array in reverse
	return len;  // return number of characters printed
}
*/

int main(int argc, char *argv[])
{
	uint32_t i, n = 0xdeadbeef;
	unsigned char len, b = 16;

	if (argc >= 2)
		n = (uint32_t)atoi(argv[1]);

	if (argc == 3)
		b = (unsigned char)atoi(argv[2]);

	for (i = 0; i < 16; ++i)
	{
		printf("%u = ", n);
		len = printnum(n, b);
		printf(" (base=%u len=%u)\n", b, len);
		++n;
	}
	return 0;
}
