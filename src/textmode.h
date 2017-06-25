#pragma once

/***************************/
/* 16 Jun 2017 bladez-fate */
/*  Text Mode Emulation    */
/***************************/

inline void clrscr() 
{

}

int Getch()
{
	return 1;
}

void textattr(int attr)
{

}

void gotoxy(int x, int y)
{

}

int Cprintf(const char* fmt, ...)
{
//	char buf[256];
//	snprintf(&buf, sizeof(buf), )
//	va_list ap;
//	va_start(ap, EventID);
//	va_end(ap);
	return 0;
}

int Cscanf(const char* fmt, ...)
{
	return 0;
}


