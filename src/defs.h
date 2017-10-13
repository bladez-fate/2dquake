/*
DEFS.H
Copyright by Andre 2001
*/

#pragma once

#pragma warning(disable: 4244)

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <utility>
#include "engine/arctic_types.h"
#include "engine/vec2si32.h"

using namespace arctic;

inline Si16 random(Si16 n)
{
	if (n == 0) {
		return 0;
	} else {
		return rand() % n;
	}
}

/* Boolean */
#define FALSE               0
#define TRUE                1

/* NULL */
/*
#if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
#define NULL                0
#else
#define NULL                0L
#endif
*/

/* Declare type (for structs and classes) */
#define DECLARE_TYPE(name)  typedef name far *P##name

/* Declare types */
#define DECLARE_FULL(_old,_new) \
    typedef _old _new; \
    typedef _old *P##_new; \
    typedef _old *HP##_new;

/* Types */
DECLARE_FULL(char,STR);
DECLARE_FULL(Si16,BOOL);
DECLARE_FULL(void,VOID);
DECLARE_FULL(char,CHAR);
DECLARE_FULL(unsigned char,UCHAR);
DECLARE_FULL(short,SHORT);
DECLARE_FULL(unsigned short,USHORT);
DECLARE_FULL(Si16,INT);
DECLARE_FULL(Ui16,UINT);
DECLARE_FULL(Si32,LONG);
DECLARE_FULL(Ui32,ULONG);
DECLARE_FULL(Ui8,BYTE);
DECLARE_FULL(Ui16,WORD);
DECLARE_FULL(Ui32,DWORD);
DECLARE_FULL(float,FLOAT);
DECLARE_FULL(double,DOUBLE);
DECLARE_FULL(long double,LDOUBLE);

/* Function types */
typedef void (*PROC)(void);

/* Misc */
#define ABS(a)              ((a)<0?-(a):(a))
#define sgn(a)              ((a)<0?-1:((a)>0?1:0))
#define odd(a)              ((a)&1)
#define even(a)             (!((a)&1))

/* Get LOW, HIGH data */
#define LO4BITS(b)          ((BYTE)(b)&0x0F)
#define HI4BITS(b)          ((BYTE)(b)>>4)
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)((WORD)(w)>>8))F
#define LOWORD(d)           ((WORD)(d))
#define HIWORD(d)           ((WORD)((DWORD)(d)>>16))

/* Make data */
#define MAKEBYTE(l,h)       ((BYTE)(((BYTE)(l)&0x0F)|((WORD)((BYTE)(h)))<<4))
#define MAKEWORD(l,h)       ((WORD)(((BYTE)(l))|((WORD)((BYTE)(h)))<<8))
#define MAKEDWORD(l,h)      ((DWORD)(((WORD)(l))|((DWORD)((WORD)(h)))<<16))

/* Get data */
#define PEEKBYTE(a,b)       (*((HPBYTE)(a)+(b)))
#define POKEBYTE(a,b,c)     (*((HPBYTE)(a)+(b))=(BYTE)(c))
#define PEEKWORD(a,b)       (*((HPWORD)((HPBYTE)(a)+(b))))
#define POKEWORD(a,b,c)     (*((HPWORD)((HPBYTE)(a)+(b)))=(WORD)(c))
#define PEEKDWORD(a,b)      (*((HPDWORD)((HPBYTE)(a)+(b))))
#define POKEDWORD(a,b,c)    (*((HPDWORD)((HPBYTE)(a)+(b)))=(DWORD)(c))

/* Pointers */
#define MAKEPTR(seg,off)    ((PVOID)MAKEDWORD(off,seg))
#define PTRSEG(ptr)         ((WORD)HIWORD(ptr))
#define PTROFF(ptr)         ((WORD)LOWORD(ptr))

/* A utility function to reverse a string  */
inline void _reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        std::swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}

// Implementation of itoa()
inline char* _itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
    
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
    
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    
    str[i] = '\0'; // Append string terminator
    
    // Reverse the string
    _reverse(str, i);
    
    return str;
}

// Implementation of strlwr()
inline char* _strlwr(char * s)
{
    char *t = s;
    
    if (!s)
    {
        return 0;
    }
    
    while (*t != '\0' )
    {
        if (*t >= 'A' && *t <= 'Z')
        {
            *t = *t + ('a' - 'A');
        }
        t++;
    }
    
    return s;
}
