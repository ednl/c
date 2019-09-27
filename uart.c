#ifndef EOF
	#define EOF -1
#endif

// Old prototypes
char UART_get     ( void  );
void UART_gets    ( char*, int );
void UART_putchar ( char  );
void UART_puts    ( char* );
// New prototypes
char UART_getchar ( void  );
int  UART_getline ( char[], int );

// char UART_getchar
// Wait forever for a character from the serial port.
// Author: E. Dronkert
// Return: first available character
char UART_getchar(void)
{
	char c;
	while ((c = UART_get()) == EOF);
	return c;
}

// int UART_getline
// Wait for ASCII text from serial port until Enter pressed or maxlen reached.
//   Doesn't include CR/LF, if any. Solves buffer overflow potential
//   of UART_gets(), handles CR+LF, echoes only printable characters.
// Author: E. Dronkert
// Args:   char s[]      array of at least maxlen chars
//         int  maxlen   at most maxlen-1 chars will be read, 1 null char added
// Return: strlen(s)
int UART_getline(char s[], int maxlen)
{
	if (!s || maxlen <= 0)     // check for bogus args
		return 0;

	char c;                    // char read from uart
	int i = 0;                 // counter

	// Use blocking call to get characters from uart, store in array.
	// Pre-decrement to reserve space for \0.
	while (--maxlen && (c = UART_getchar()) != '\r' && c != '\n')
	{
		s[i++] = c;            // store character in array, increment counter
		if ((c >= 32 && c < 128) || c == '\t')
			UART_putchar(c);   // echo readable ASCII character back to terminal
	}

	// Enter can be translated by the terminal as CR, LF, or CR+LF. If Enter was
	// pressed before max length was reached, there might be a LF left in the
	// uart buffer. Use non-blocking call to check next character.
	if (c == '\r' && (c = UART_get()) != EOF && c != '\n') 
		// There was a next char but it wasn't a LF. Shouldn't occur,
		// normally. TODO: put char back in uart buffer, if possible.
		;

	s[i] = '\0';               // terminate the string
	UART_puts("\r\n");         // move terminal cursor to start of next line
	return i;                  // return strlen(s)
}
