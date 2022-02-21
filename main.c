#include <stdio.h>
#include <string.h>
#include <windows.h>

#define FG_GRAY (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FG_GOLD (FOREGROUND_RED | FOREGROUND_GREEN)

int mouseParams[3];
HANDLE hConsole = NULL;
int consoleAvailable = 0;
WORD defaultConsoleAttributes = FG_GRAY;

/*
 *
 * Console
 *
 */

void set_console_text_attribute (WORD wAttributes)
{
	if (consoleAvailable)
		SetConsoleTextAttribute(hConsole, wAttributes);
}

void restore_console_text_attributes ()
{
	if (consoleAvailable)
		SetConsoleTextAttribute(hConsole, defaultConsoleAttributes);
}

/*
 *
 * Print
 *
 */

void print_state (const char* const msg)
{
	set_console_text_attribute(FG_GOLD);
	printf("%s ", msg);
	if (mouseParams[2] == TRUE) {
		set_console_text_attribute(FOREGROUND_GREEN);
		printf("ON\n");
	} else {
		set_console_text_attribute(FOREGROUND_RED);
		printf("OFF\n");
	}
	restore_console_text_attributes();
}

void print_usage (const char* const argv[])
{
	set_console_text_attribute(FG_GOLD);          printf("Usage: %s ", argv[0]);
	set_console_text_attribute(FOREGROUND_GREEN); printf("<on");
	set_console_text_attribute(FG_GOLD);          printf("/");
	set_console_text_attribute(FOREGROUND_RED);   printf("off>\n");
	restore_console_text_attributes();
}

void print_error (const char* const msg)
{
	set_console_text_attribute(FOREGROUND_RED);
	DWORD err = GetLastError();
	printf("%s\nError code: %ld (0x%lX)\n", msg, err, err);
	restore_console_text_attributes();
}

/*
 *
 * Main
 *
 */

int main (const int argc, const char* const argv[])
{
	// Initialize console stuff
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if ((consoleAvailable = (hConsole != INVALID_HANDLE_VALUE) && (hConsole != NULL))) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (GetConsoleScreenBufferInfo(hConsole, &info))
			defaultConsoleAttributes = info.wAttributes;
	}

	// Get mouse parameters
	if (!SystemParametersInfoA(SPI_GETMOUSE, 0, mouseParams, 0)) {
		print_error("Failed retrieving mouse paramaters");
		return 1;
	}

	if (argc != 2) {
		print_state("Mouse acceleration is");
		print_usage(argv);
		return 0;
	}

	// Update mouse acceleration
	if      (strcmp(argv[1], "on")  == 0) mouseParams[2] = TRUE;
	else if (strcmp(argv[1], "off") == 0) mouseParams[2] = FALSE;
	else {
		print_usage(argv);
		return 0;
	}

	// Update mouse paramaters
	if (!SystemParametersInfoA(SPI_SETMOUSE, 0, mouseParams, SPIF_SENDCHANGE)) {
		print_error("Failed updating the mouse paramaters");
		return 1;
	}

	print_state("Successfully set mouse acceleration to");
	return 0;
}
