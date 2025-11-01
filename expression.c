//-my-c-file
// expression.c
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include <errno.h>
#define windows 1
#ifdef windows
   #define INT_32 int32_t
   #define INT_64 int64_t
   #define DOUBLE_REAL  double
#else
   #define INT_32 int32_t
   #define INT_64 long long
   #define DOUBLE_REAL long double
#endif


// define these, either prior to including this file, or using -D xxxx on the gcc command line

#define Separate_ll_function  // if defined, we will use the separate long long version for more precision
//#define Main yes              // includes the main test program if defined

// --------------------------------------------------------
//#define Debug  // Debug if set, enables a printf output of a trace
//#define Debuga // Debuga if set, adds 2 members to the operand_d and operand_ll structs for debugging

/*

Lookup codes. Note that when it's faster and easier to just check the ascii character 
then that is done with a litteral. When checking if something is a valid number, then
these arrays are used. 

*/




#define NOT_IN_SET_7F  0x7f  //token value meaning not in the set
#define DEC_PNT_7D     0x7d  //the decimal point
#define SCI_EXP_6E     0x6e      // e/E notation
#define SKIP_CHAR_7E   0x7e      // underlines and commas

#define EXP_OPER      	'x'      // this is used for a ** operator, where the '*' is already used
#define SHIFTLEFT_OPER	'l'      // this is used for a << operator
#define SHIFTRIGHT_OPER	'r'      // this is used for a >> operator

#define INVALID_0     0  // to check for any chars that are not allowed, all the ok on
#define REGULAR_1     1  // to check for any chars that are not allowed, all the ok on
#define LEFTPAREN_2   2 // unused, just checks for the litteral, but the validity checker
#define RIGHTPAREN_3  3 // can 
#define NULL_4        4 // not really used

#define ERROR_PAREN_NOT_BALANCED 2
#define ERROR_RECURSION 3
#define STATUS_OK 0

#define HIGHEST_PREC 6 // highest level, used to allocate stack and set unary's

#ifndef RECURSION_MAX
#define RECURSION_MAX 10 // maximum recursion depth
#endif
/*

Token typing arrays. To return a value for an input character
e.g. numbers hex, '0'-'9' return 0..9 and 'a'-'f' and 'A'-'F' both return 10-15


*/
#define GENERATE_ARRAYSx
#ifdef GENERATE_ARRAYS

static unsigned char Letters[128]; // filled by setupascii one time on startup, valid characters for quick check
static unsigned char Numbers[128]; // either the value of an ascii digit ('0' = 0) or  NOT_IN_SET_7F  for scanning numbers
static unsigned char Numbersf[128]; // same but with a decimal point
static unsigned char Numbershex[128]; // with a-f A-F
static unsigned char Numbersbin[128]; // just 0,1
static unsigned char Numbersoct[128]; // just 0..7
static unsigned char Operators[128]; // just the set of valid characters for an operator or terminal 
static int Defined = 0;  // indicates these are not setup, setupascii will have to init them
#else
static int Defined = 1;  // indicates these are setup below, by doing the dump, so don't need to setup at runtime



static unsigned char Letters[128] = {
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x02, 0x03, 
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 
0x00, 0x00};

static unsigned char Numbers[128] = {
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f};

static unsigned char Numbersf[128] = {
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7e, 0x7f, 0x7d, 0x7f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x6e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x6e, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f};

static unsigned char Numbershex[128] = {
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f};

static unsigned char Numbersbin[128] = {
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x00, 0x01, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f};

static unsigned char Numbersoct[128] = {
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f};

static unsigned char Operators[128] = {
0x01, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x01, 0x7f, 0x7f, 0x01, 
0x01, 0x01, 0x7f, 0x01, 0x7f, 0x01, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x7f, 0x01, 0x7f, 0x01, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x01, 0x7f, 
0x7f, 0x7f};

#endif

static DOUBLE_REAL fractions_of_ten [25] = {1.0 , 
0.1,
0.01,
0.001,
0.0001,
0.00001,
0.000001,
0.0000001,
0.00000001,
0.000000001,
0.0000000001,
0.00000000001,
0.000000000001,
0.0000000000001,
0.00000000000001,
0.000000000000001,
0.0000000000000001,
0.00000000000000001,
0.000000000000000001,
0.0000000000000000001,
0.00000000000000000001,
0.000000000000000000001,
0.0000000000000000000001,
0.00000000000000000000001,
0.000000000000000000000001}; // total of 25

static DOUBLE_REAL multiples_of_ten [25] = {1.0 , 
10.0,
100.0,
1000.0,
10000.0,
100000.0,
1000000.0,
10000000.0,
100000000.0,
1000000000.0,
10000000000.0,
100000000000.0,
1000000000000.0,
10000000000000.0,
100000000000000.0,
1000000000000000.0,
10000000000000000.0,
100000000000000000.0,
1000000000000000000.0,
10000000000000000000.0,
100000000000000000000.0,
1000000000000000000000.0,
10000000000000000000000.0, // total of 25
100000000000000000000000.0}; // total of 25



static int nfractions = sizeof(fractions_of_ten) / sizeof (DOUBLE_REAL);
static int nmultiples = sizeof(multiples_of_ten) / sizeof (DOUBLE_REAL);


/* 

local version of power function for INT_32  and INT_64 and strcmp (this only compares from 2nd char onward)

*/

static int mystrcmp (const char *p1, const char *p2)
{
  const unsigned char *s1 = (const unsigned char *) p1+1; // we already compared the first letter
  const unsigned char *s2 = (const unsigned char *) p2+1;
  unsigned char c1, c2;
  
  do {
    
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c2 == '\0')break;
        
  } while (c1 == c2);
  

  return c1 - c2;
}


static INT_32 Powl(INT_32 base,  INT_32 exp) {
    int i;
    INT_32 result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }
static INT_64 Powll(INT_64 base,  INT_64 exp) {
    int i;
    INT_64 result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }
 

 
// --------------------------------------------------------
/* 

local version of string to DOUBLE_REAL, since if it's an integer only, we can be so much faster,
But if there's an . or e/E we will be slower, but can process it now using our mystrtod function

*/
static INT_32 mystol (char*, char**);
static INT_64 mystoll(char*, char**);

static DOUBLE_REAL mystrtod(char* stringIn, char** endp) {
	char *s = stringIn;
	int isneg = 0;
	register INT_64  l = 0;
	INT_64 value;
	register INT_64  code;
	unsigned char * number_table; char *p;
	
	int digits = 0; // count the number of digits after the decimal point
	INT_64 m = 0;	// acumulate the fractionsl value if any
	int exponent = 0; 
	DOUBLE_REAL final_double;

	if ( *s == '-' ) {
		isneg = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}
	while (*s == ' ') s++; // skip spaces, we don't allow tabs here

	number_table = Numbersf;
    l = (INT_64 )number_table[*s++];
    if ( l == 0 ) {
        if (*s == 'x' || *s == 'X' || *s == 'b' || *s == 'B' ||  *s == 'o' || *s == 'O' ||   *s == 'd'  || *s == 'D' ) {
    		value = mystoll(stringIn,&p);
    		if (endp) *endp = (char*)p;
    		return (DOUBLE_REAL) value; 
    	}
	}

    if (l ==  NOT_IN_SET_7F ) { // first time, no need to x 10 just check for invalid char (not a number)
        if (endp) *endp = (char*)stringIn; // this is an error, return with end pointer = start
        return 0;
    } else if (l ==  SKIP_CHAR_7E ) { // _ or , at the beginning, so zero our accumulating l
        l = 0;
    } else if (l ==  DEC_PNT_7D ) { // its a .
//    	return strtod(stringIn,endp); // nothing for us to do, just pass on to strtod for the entire .xxx 
		s--; // backup so the dot is grabbed in the while loop, we just have a value of 0.xxx
        l = 0; // start with left of dot being 0
	}
	while(1) {
		code = (INT_64 )number_table[*s++];
		if (code ==  DEC_PNT_7D  || code ==  SCI_EXP_6E ) { // found a decimal point or the letter e/E
			if ( code ==  SCI_EXP_6E  ) { char *p=NULL;
				exponent =  mystol(s, &p);
				s = p+1; // this gets decremented at the end
				break;
			} else {
				
			}
			code = (INT_64 )number_table[*s++];
			if ( code <= 10 ) { // if it isn't a digit, it could be undersscore or other, so in the below while we take care of that
				m = code; // but it was a digit (no hex etc. here) so start with that
				digits++;
			} else {
				s--; // backup one, we probably have a decimal 123. with nothing after it, we'll just read the . again below
			}
			while(1) { 
				code = (INT_64 )number_table[*s++];
				if       ( code ==  DEC_PNT_7D   || code ==  NOT_IN_SET_7F  ||  code ==  SCI_EXP_6E  ) { // we're done on another dot or othere non digits
					break;
				} else if ( code ==  SKIP_CHAR_7E  ) { // skip on under or comma
					continue;
				} else {
					m = m * 10 + code; // no hex or other base in decimal numbers
					digits++;
					continue;				
				}
 				
			}
			if ( code ==  SCI_EXP_6E  ) {
                s--;
				continue; // saw the e go back and get the exponent
			} else {
				break;    // we're done, it was either a dot that didn't belong there, or it was the next operator
			}
		} else if ( code ==  NOT_IN_SET_7F  ) { // end of a number
			break;
		} else if (code ==  SKIP_CHAR_7E ) { //just skip these
			continue;
		} else { INT_64 tempL;
			tempL = l * 10;// no hex or other base in decimal numbers
			if ( tempL < l ) {
//				final_double = strtod(stringIn,endp);
//				return final_double;
				if (endp) *endp = stringIn; // indicate an error on overflow
				return 0;
			}
			l =tempL + code;
		}
	}

   if (endp) *endp = (char*)s-1;
	if ( digits == 0 && exponent == 0) { // we got only an integer part, but could be an E following
										// it's only an integer, in l and no exponent or it was 0, 
	} else { // combine the parts
//		printf(" ok now wrap it up we got an l, an m, possible exponent and digits\n"  );
		if ( digits > 24 ) {
			final_double = (DOUBLE_REAL) l;
		} else {
			final_double = fractions_of_ten[digits] * (DOUBLE_REAL) m + (DOUBLE_REAL) l;
		   
		}
		if ( isneg ) {
			final_double = -final_double;
		}
		if ( exponent < 0) {int i;
			// ok, need to do some multiplies or divides by 10 or .1
			if ( exponent > -nmultiples ) {
				final_double = final_double * fractions_of_ten[-exponent];
			} else {
				for (i=0; i< abs(exponent) ; i++) {
					final_double = final_double * (DOUBLE_REAL) .1;
				}
			}
		} else if (exponent > 0) {int i; 
			if ( exponent < nmultiples ) {
				final_double = final_double * multiples_of_ten[exponent];
			} else {
				for (i=0; i< exponent ; i++) {
					final_double = final_double * (DOUBLE_REAL) 10.0;
				}
			}
		}
 		return (DOUBLE_REAL)final_double;			
	}

	// here we finish with an integer part only, or we did the above
	// combining of the int.frac parts in l and m, either way just check for -

	if ( isneg ) {
		l = -l;
	}
    return (DOUBLE_REAL)l;
}
#if 0
static DOUBLE_REAL mystrtod_old(char* stringIn, char** endp) { // this just for reference use to compare results between old and new versions
	char *s = stringIn;
	int isneg = 0;
	register INT_64  l = 0;
	INT_64 value;
	register INT_64  code;
	char * number_table; char *p;

	if ( *s == '-' ) {
		isneg = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}
	while (*s == ' ') s++; // skip spaces, we don't allow tabs here

	number_table = Numbersf;
    l = (INT_64 )number_table[*s++];
    if ( l == 0 ) {
        if (*s == 'x' || *s == 'X' || *s == 'b' || *s == 'B' ||  *s == 'o' || *s == 'O' ||   *s == 'd'  || *s == 'D' ) {
    		value = mystoll(stringIn,&p);
    		if (endp) *endp = (char*)p;
    		return (DOUBLE_REAL) value; 
    	}
	}

    if (l ==  NOT_IN_SET_7F ) {// first time, no need to x 10 just check for invalid char (not a number)
        if (endp) *endp = (char*)stringIn; // this is an error, return with end pointer = start
        return 0;
    } else if (l ==  SKIP_CHAR_7E ) { // _ or , at the beginning, so zero our accumulating l
        l = 0;
    } else if (l ==  DEC_PNT_7D  || l ==  SCI_EXP_6E ) { // starts with a . or e
    	return strtod(stringIn,endp); // nothing for us to do, just pass on to strtod for the entire .xxx 
	}
	while(1) {
        code = (INT_64 )number_table[*s++];
		if (code ==  DEC_PNT_7D  || code ==  SCI_EXP_6E ) { // found a decimal point or the letter e/E, so gotta use strtod anyway, oh well, we tried!
			return strtod(stringIn,endp); // if it had an exponent, we give up and just do it the slow way 
		} else if ( code ==  NOT_IN_SET_7F  ) { // end of a number
			break;
		} else if (code ==  SKIP_CHAR_7E ) { //just skip these
			continue;
		} else {
            l = l * 10 + code; // no hex or other base in decimal numbers
		}
	}

	
   if (endp) *endp = (char*)s-1;

	if ( isneg ) {
		l = -l;
	}
    return (DOUBLE_REAL)l;
}
#endif

// --------------------------------------------------------
/* 

local version of string to INT_64 , we can also handle the 4 bases

*/


static INT_64 mystoll(char* stringIn, char** endp) {
	char *s = stringIn;
	int isneg = 0;
	register INT_64  l = 0;
	register INT_64  code,base;
	unsigned char * number_table;

	if ( *s == '-' ) {
		isneg = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}
    while (*s == ' ') s++; // skip spaces, we don't allow tabs here
    base = 10;
	number_table = Numbers;
	if ( *s == '0' ) {
		if       ( s[1] == 'x' || s[1] == 'X' ) {
			base = 16;
			number_table = Numbershex;
			s++;s++;

		} else if ( s[1] == 'b'|| s[1] == 'B' ) {
			base = 2;
			number_table = Numbersbin;
			s++;s++;
		
		} else if ( s[1] == 'o' || s[1] == 'O' ) {
			base = 8;
			number_table = Numbersoct;
            s++; s++;
		} else if ( s[1] == 'd' || s[1] == 'D' ) {
            s++; s++;
		}
	}
    l = (INT_64 )number_table[*s++];
    if (l ==  NOT_IN_SET_7F ) {// first time, no need to x 10 just check for invalid char (not a number)
        if (endp) *endp = (char*)stringIn; // this is an error, return with end pointer = start
        return 0;
    } else if (l ==  SKIP_CHAR_7E ) { // _ or , at the beginning, so zero our accumulating l
        l = 0;
	}
	while(1) {
        code = (INT_64 )number_table[*s++];
		if ( code ==  NOT_IN_SET_7F  ) { // end of a number
			break;
		} else if (code ==  SKIP_CHAR_7E ) { //just skip these
			continue;
		} else {INT_64 tempL;
//			l = (l<<3) +l +l + code; // times 10, but doesn't look faster, so don't use it
			tempL = l * base;
			if ( tempL < l ) {
//                l = strtoll(stringIn,endp,(int)base);
//				return l;
				if (endp) *endp = stringIn; // indicate an error on overflow
				return 0;
			}
			
			l = tempL + code;
		}
	}

	
   if (endp) *endp = (char*)s-1;

	if ( isneg ) {
		l = -l;
	}
    return l;
}


// --------------------------------------------------------
/* 

local version of string to INT_32, we can also handle the 4 bases

*/




static INT_32 mystol(char* stringIn, char** endp) {
	char *s = stringIn;
	int isneg = 0;
	register INT_32  l = 0;
	register INT_32  code,base;
	unsigned char * number_table;

	if ( *s == '-' ) {
		isneg = 1;
		s++;
	} else if (*s == '+') {
		s++;
	}
	base = 10;
	number_table = Numbers;
	if ( *s == '0' ) {
		if       ( s[1] == 'x' || s[1] == 'X' ) {
			base = 16;
			number_table = Numbershex;
			s++;s++;

		} else if ( s[1] == 'b'|| s[1] == 'B' ) {
			base = 2;
			number_table = Numbersbin;
			s++;s++;
		
		} else if ( s[1] == 'o' || s[1] == 'O' ) {
			base = 8;
			number_table = Numbersoct;
            s++; s++;
		} else if ( s[1] == 'd' || s[1] == 'D' ) {
            s++; s++;
		}
	}
    l = (INT_32 )number_table[*s++];
    if (l ==  NOT_IN_SET_7F ) {// first time, no need to x 10 just check for invalid char (not a number)
        if (endp) *endp = (char*)stringIn; // this is an error, return with end pointer = start
        return 0;
    } else if (l ==  SKIP_CHAR_7E ) { // _ or , at the beginning, so zero our accumulating l
        l = 0;
	}
	while(1) {
        code = (INT_32 )number_table[*s++];
		if ( code ==  NOT_IN_SET_7F  ) { // end of a number
			break;
		} else if (code ==  SKIP_CHAR_7E ) { //just skip these
			continue;
		} else {
//			l = (l<<3) +l +l + code; // times 10, but doesn't look faster, so don't use it
            l = l * base + code;
		}
	}

	
   if (endp) *endp = (char*)s-1;

	if ( isneg ) {
		l = -l;
	}
    return l;
}





/* 

setup the arrays with values for valid characters and numbers
This would need to be modified to add an operator, to let that
operator be a valid ascii character; the numbers array is used
to build an array to fast lookup the value of a hex digit

The dump_array function can turn the results here into a set
of C array declarations with their values as initializers

Those declarations then become defined at compile time and this
function need not be called. However, if changes are made here,
then this needs to be run once. The linux main program has an
option to run this and also dump_arrays out to a file. One would
then copy/paste that above.

However, this is still called one time and Defined is set to 1 so
further calls won't occur. This lets one test with this dynamic
version until it's deemed correct.

*/



static void setupascii() { // this is our table creator used to tokenize values by their ascii char value
	if ( Defined ) {
		return;
	}
    int i;
	char *ascii = "()\n ^*/%+-.abcdefghijklmnopqrstuvwxyzABCDEF0123456789_,&|!<>:"; // note . is allowed here for floating point, but will error out in our int routines
	char *numbers = "0123456789abcdefABCDEF"; // all the valid numbers up to hex digits
	char *operators = ":+-*/)&|^<>%\n"; // all the valid operators or terminals, we'll set null manually
	
	for (i=0; i< 128 ; i++) { // first clear all to 0 or 7f, then fill in from the above ascii and numbers
		Letters[i] = 0;
		Numbers[i] =  NOT_IN_SET_7F ;	// need to use this code since 0 is valid
		Numbersf[i] =  NOT_IN_SET_7F ;	// decimal point numbers
		Numbershex[i] =  NOT_IN_SET_7F ;
	 	Numbersbin[i] =  NOT_IN_SET_7F ;
		Numbersoct[i] =  NOT_IN_SET_7F ;
		Operators[i]  =  NOT_IN_SET_7F ;
	}
	Numbersf['.'] =  DEC_PNT_7D ;      //  the decimal point  
	Numbersf['e'] =  SCI_EXP_6E ;      //  the exponent  letters
	Numbersf['E'] =  SCI_EXP_6E ;      //  the exponent  
	Numbersf['_'] =  SKIP_CHAR_7E ;      //  our underscore or comma  
	Numbersf[','] =  SKIP_CHAR_7E ;      //  our underscore or comma  
	
	Numbers['_'] =  SKIP_CHAR_7E ;	// _ and , allowed in numbers, we just skip over them
	Numbers[','] =  SKIP_CHAR_7E ;
		
	Numbershex['_'] =  SKIP_CHAR_7E ;	// _ and , allowed in numbers, we just skip over them
	Numbershex[','] =  SKIP_CHAR_7E ;	
	
	Numbersbin['_'] =  SKIP_CHAR_7E ;	// _ and , allowed in numbers, we just skip over them
	Numbersbin[','] =  SKIP_CHAR_7E ;	
	
	Numbersoct['_'] =  SKIP_CHAR_7E ;	// _ and , allowed in numbers, we just skip over them
	Numbersoct[','] =  SKIP_CHAR_7E ;	
	
	size_t len = strlen(ascii);
	for (i=0; i< len ; i++) {
		Letters[ascii[i]] = 1;	
	}
	len = strlen(operators);
	for (i=0; i< len ; i++) {
		Operators[operators[i]] = 1;	
	}
    Operators[0] = 1;
	Letters['('] = LEFTPAREN_2; // for functions or plain parenthesis
	Letters[')'] = RIGHTPAREN_3;
	Letters[0] =   4; // if we see a null byte, code it as 4 - not used for now
	
	for (i=0; i< 16 ; i++) { // these are table lookup for the 4 bases, so we don't have to do number - '0' to get the number from the ascii code
		if ( i < 2 ) {
			Numbersbin[numbers[i]] = (unsigned char) i;	
		}
		if ( i < 8 ) {
			Numbersoct[numbers[i]] = (unsigned char) i;	
		}
		if ( i < 10 ) {
			Numbers[numbers[i]] = (unsigned char) i;	
			Numbersf[numbers[i]] = (unsigned char) i;	// decimal point same as decimal, but allows a decimal point also
		}
		if ( i < 16 ) {
			Numbershex[numbers[i]] = (unsigned char) i;	// lowercase
			if ( i > 9 ) {
				Numbershex[numbers[i+6]] = (unsigned char) i;	// lowercase
			}
		}
		
	}
	Defined = 1; // we only do this once this data is static
}

 
static void dump_arrays(char *filename) {
static int Defined = 0;  // indicates these are setup

static unsigned char* arry[] = { Letters,Numbers,Numbersf,Numbershex, Numbersbin , Numbersoct ,Operators};
static unsigned char* arryname[] = { "Letters","Numbers","Numbersf","Numbershex", "Numbersbin" , "Numbersoct" ,"Operators"};
int i,j,k,byte;



#include <errno.h>
#include <stdio.h>
#include <string.h>

    FILE* io = fopen(filename, "w");
    if (io == NULL) {
        fprintf(stderr, "cannot open file '%s': %s\n",
            filename, strerror(errno));
        return;
    }
    for (i=0; i< 7 ; i++) {
        printf("\nstatic unsigned char %s[128] = {\n", arryname[i]);
        fprintf(io,"\nstatic unsigned char %s[128] = {\n", arryname[i]);
        for (j = 0,k=1; j < 128; j++,k++) {
            byte = (int)arry[i][j];
            printf("0x%02x", byte);
            fprintf(io,"0x%02x", byte);
            if (j != 127) {
                printf(", ");
                fprintf(io,", ");
            }
            if (k > 20) {
                printf("\n");
                fprintf(io,"\n");
                k = 0;
            }
        }
        printf("};\n");
        fprintf(io,"};\n");
    }
    fclose(io);
}
 
 
// --------------------------------------------------------

/* 

checks the input string against the  Letters array, kind is not implemented (was to be float/int)

*/


static int eval0x( char* stringIn,int kind) { // quick check for unbalanced parens and any bad chars in expression
    int parlev = 0;
    register int position = 0; // don't know if compiler honors register, but it can't hurt
    register char* s = stringIn;
    register int code;
    for (;*s != '\0'; ) {
		if ((*s & 0x80) != 0) {
    		goto error; // no chars over 0x7f
    	}
    	
		code = Letters[*s++];
		if       ( code == 1 ) { // most will come through here
			continue;
		} else if ( code == 2 ) {
			parlev++;
			continue;
		} else if ( code == 3 ) {
			if ( --parlev < 0 ) {
				return ERROR_PAREN_NOT_BALANCED; // if it goes negative, we've got a close not matching an open ( ))  
			}
			continue;
		} else {
error:
			position = (int) (s - stringIn); // compute position of the error, return +0x2000, since unbal parens returns a 2
			return position +0x2000;
		}
		
    }
    if ( parlev != 0 ) {
        return ERROR_PAREN_NOT_BALANCED;
    }
    return STATUS_OK; // ok
}
 
 
// --------------------------------------------------------

/* 

first version of input checking, w/o opt on compiler, this was much slower
perhaps it would work better with opt, since it was the switch statement that
was slowing it down

*/


static int eval0( char* stringIn,int kind) {
    int parlev = 0;
    register int position = 0;
    register char* s = stringIn;
    for (;*s != '\0'; s++,position++) {
        switch (*s) {
            case '(': parlev++ ; break;
            case ')': parlev-- ; break;
            
            case '\n':case ' ':case '^':case '*':case '/':case '%':case '+':case '-':case '.':

            case 'a':case 'b':case 'c':case 'd':case 'e':
            case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':
            case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':
            case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
            
//            case 'A':break;case 'B':break;case 'C':break;case 'D':break;case 'E':break;
//            case 'F':break;case 'G':break;case 'H':break;case 'I':break;case 'J':break;case 'K':break;case 'L':break;
//            case 'M':break;case 'N':break;case 'O':break;case 'P':break;case 'Q':break;case 'R':break;case 'S':break;
//            case 'T':break;case 'U':break;case 'V':break;case 'W':break;case 'X':break;case 'Y':break;case 'Z':break;
            
            case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
            break;


            default: return position +0x2000;
            break;
        }
        
    }
    if ( parlev != 0 ) {
        return 2;
    }
    return 0; // ok
}

/*

To add a function is simple. There are 3 steps. 

1 First, add an enum like below.
2 Then go to afunc and add a mystrcmp with the name to be used by this evaluator
3 Then go to the switch statement in  evaluate_d and  where it says HERE and add a function like the others

*/


enum function_codes {eERROR=0,eABS,eINT,eSIN,eCOS, eROUND, eSQRT, eRAD, eFLOOR,
					eLT, eGT, eEQ, eNE, eLE, eGE}; // return one of these for function or an error



// --------------------------------------------------------

/* 

determine which function is being called, news is the number of chars scanned

*/


static int afunc(char* stringIn, int* news) { // parse a function up to opening paren
    char buf[10];           // accumulate function name
    char* p = buf;          // points to where to store next char
    int paren = 0;          // flag for having seen the opening paren
    int i;                  // count of chars not including spaces, limits what we write to buf
    char c;
    char* s = stringIn;
    for (i=0; i<= 7 ; )  { // this is less than the size of buf, should use a symbol however
    	 c = *s;
         if (  c >= 'a' &&  c <= 'z') {
            *p++ = *s++;
            i++;
            continue;
        } else if (  c == '(' ) {
            paren++;
            break;
        } else if (  c == ' ' ) {
            s++;
            continue;
        } else if (  c == '\0' ) {
            break;
        } else {
            return eERROR;
        }
    }
    if ( i < 2 || i > 5 || paren !=1 ) {
    	return eERROR;
    }
    *p = '\0';                      // null terminate the buffer
    *news =  (int) ( s - stringIn); // how many chars did we scan, return to caller
    
    
    switch (buf[0]) { // so we only do one or two mystrcmp's, based on the first letter,  Note mystrcmp starts comparing at the 2nd char
        case 'a':
            if ( mystrcmp(buf,"abs") == 0 ) {
                return eABS;
            }  
            break;
        case 'e':
            if ( mystrcmp(buf,"eq") == 0) {
                return eEQ;
            }  
            break;
        case 'f':
            if ( mystrcmp(buf,"floor") == 0) {
                return eFLOOR;
            }  
            break;
        
        case 'g':
            if ( mystrcmp(buf,"ge") == 0) {
                return eGE;
            }  
            if ( mystrcmp(buf,"gt") == 0) {
                return eGT;
            }  
            break;
        case 'i':
            if ( mystrcmp(buf,"int") == 0 ) {
                return eINT;
            }  
            break;
        case 'l':
            if ( mystrcmp(buf,"le") == 0) {
                return eLE;
            }  
            if ( mystrcmp(buf,"lt") == 0) {
                return eLT;
            }  
            break;
        case 'n':
            if ( mystrcmp(buf,"ne") == 0) {
                return eNE;
            }  
            break;
        case 's':
            if ( mystrcmp(buf,"sqrt") == 0  ) {
                return eSQRT;
            }  
            if ( mystrcmp(buf,"sin") == 0  ) {
                return eSIN;
            }  
            break;
       case 'c':
        
            if ( mystrcmp(buf,"cos") == 0  ) {
                return eCOS;
            }  
            break;
        case 'r':
            if ( mystrcmp(buf,"round") == 0  ) {
                return eROUND;
            }  
            if ( mystrcmp(buf,"rad") == 0  ) {
                return eRAD;
            }  
            break;
        default:
        break;
    }
    return eERROR;
    
    
}


// --------------------------------------------------------

/* 

Evaluate an expression, recursive decent parser, single left
to right, with recursion for ()'s and fucntion()'s only

Originally we had 3 versions of the evaluator, for double, int, and long long
Then we wrapped the double version for the 2 integer versions
This works for 32 bit ints since double float has enough bits to make
it precise. However, long long's were not working well with large values
such as 2**63-1 was the same as 2**63-10 etc. 

So, we have now restored the more precise version of the long long, but
use a conditional compilation to decide if we include it or not.
the macro is Separate_ll_function

double floating version, biggest slowdown WAS the strtod function

So, we have our own version, which if there's no decimal point does an
integer conversion. That also checks for a 0x, 0b, etc. and if found
it can't be a float, so it calls the integer conversion. If it does
run into the decimal point or the exponent, e, then it has to
do some more computations. Still faster than strtod. The increase in speed
over strtod is still a puzzle since our method isn't all that clever. We
just use integer math on both sides of the decimal point and later use a
multiply by 10's table to addjust the fractional part. This reduces the
amount of floating point we need to do.

Error codes returned in thestatus are, 0=ok, 2=unbalanced parens, 3=recursion depth exceeded
Other errors are in hex, 0zxxx e.g. 0x1003 where the z, is 1,2, or 3 and xxx is the character
position of the error. z=1 is general error, z=2 invalid character, e.g. $ or { z=3 bad function name
z=4 divide by zero
*/


static DOUBLE_REAL evaluate_d( char* stringIn, char** endp, int* thestatus,int level) {
    struct operand_d {
																							#ifdef Debuga
																							        void *address; // for debug
																							        long  pushed;
																							#endif
        DOUBLE_REAL val  ; // the value of the current computation, either on the stack or in x
        char   op   ; // this is one of the litteral operators, but could be any constant
        char   prec ; // precedence level, currently 6 is the max
    } stack[HIGHEST_PREC+1] = { 
																							#ifdef Debuga
																							    NULL, 0,
																							#endif
    0.,0,0 },x, *sp; // stack pointer and the active opperand we're building up 
																							#ifdef Debuga
																							    stack[0].address = (void *)&stack[0].address; // debug
																							    stack[1].address = (void *)&stack[1].address;
																							    stack[2].address = (void *)&stack[2].address;
																							    stack[3].address = (void *)&stack[3].address;
																							    stack[4].address = (void *)&stack[3].address;
																							#endif
    DOUBLE_REAL fval; // the value of the argument of a function
    char* p;     // pointer to input string text, next token after call to functrion arg or mystrtod
    char* s;     // pointer to current location in the input string
    char *fsave = stringIn; // the pointer of a function call name, in case of an error
    int news;    // used to compute the new s after seeing a function( 
	int status,error_code = 0x1000;
    enum function_codes f; 
    char c;      // used to hold the current character, to avoid another *s

    s  = stringIn;
    while (*s == ' ') s++; // skip whitespace
    
    c = *s;
    sp = stack;
    
    if ( ++level > RECURSION_MAX) {
																							#ifdef Debug
																							    printf("%*s  evaluate_d: exceeded recursion depth %d at %s\n",level*3,".",level,s);
																							#endif
    	*thestatus = ERROR_RECURSION;
    	return 0.0;
    }
    
																							#ifdef Debuga
																							    x.pushed = (long)0xbeefcafe; // debug
																							#endif
																							#ifdef Debug
																							    printf("%*s  evaluate_d: depth=%d /%3zd byt stringIn= [%s] \n",level*3,".",level ,(thestatus - &status)*sizeof(int),s);
																							#endif
    if (c  == '+' || c == '-') { // check for initial unitary + or - 
    	x.val = 0.0;
    	x.op  = c;
    	x.prec = HIGHEST_PREC;  // give it highest priority 
    	*sp++ = x;  // pretend we started with a 0, so it's 0+... or 0-...
        s++;
	
																							#ifdef Debug
                                                                                                printf("%*s  push a 0.0 with unary [%c] op at prec(%d)\n", level*3, ".", c, x.prec);
																							#endif
    }
    while (1) {
        
        while (*s == ' ') s++; // skip over spaces, we don't allow tabs here, much faster than using isspace
        if (*s == '(') {       // opening parens, we know they are balanced, we did that check separately
																							#ifdef Debug
																								printf("%*s  opening parens  \n" ,level*3,".");
																							#endif
            x.val = evaluate_d(s + 1, &p,&status,level); // recursive call
            s = p;
            if (status != STATUS_OK) {
            	if ( status < 0x1000 ) {
	            	*thestatus = status;
	                return 0.0;
            	}
            	goto error; // return a status value that also returns the position of the error
            }
            if (*s == ')')s++;
        } else if ( *s >= 'a' && *s <= 'z') { // a function is just a pair of ()'s with a value
            f = afunc(s,&news);               // functions are the only thing that has a-z letters
            fsave = s + 1;
            if ( f == eERROR ) {
                error_code = 0x3000;
            	goto error;// return a status value that also returns the position of the error
            }
																							#ifdef Debug
																								printf("%*s  func begins with [%c%c...] recur... \n" ,level*3,".",s[0],s[1]);
																							#endif
            s = s+news;
            fval = evaluate_d(s + 1, &p,&status,level); // evaluate what's in the parens, recursively
            s = p;
            if (status != STATUS_OK) {
            	if ( status < 0x1000 ) {
	            	*thestatus = status;
	                return 0.0;
            	}
            	goto error; // return a status value that also returns the position of the error
            }
            if (*s == ')')s++;
             
  
             switch (f) { // do the corresponding function on our value HERE to add additional functions
    
                case eABS:       
                    if ( fval < 0.0 ) {
                        fval = -fval;
                    }
                    x.val = fval;
                    break;
                case eINT:    
					if ( fval < 0.0 ) {
                		x.val = floor(fval + 1.0);
					} else {
                		x.val = floor(fval);
					}
                	break;
                case eEQ:    x.val = (fval  == 0 ) ? 1 : 0     ; break;                                  
                case eNE:    x.val = (fval  != 0 ) ? 1 : 0     ; break;
                case eLT:    x.val = (fval  <  0 ) ? 1 : 0     ; break;
                case eLE:    x.val = (fval  <= 0 ) ? 1 : 0     ; break;
                case eGT:    x.val = (fval  >  0 ) ? 1 : 0     ; break;
                case eGE:    x.val = (fval  >= 0 ) ? 1 : 0     ; break;
                case eFLOOR: x.val = floor(fval)      ; break;                                  
                case eSIN:    x.val = sin(fval);      ; break;
                case eCOS:    x.val = cos(fval);      ; break;
                case eSQRT:   x.val = sqrt(fval);     ; break;
                case eROUND:  x.val = round(fval)     ; break;
                case eRAD:    x.val =fval * (3.141592653589793238462643/180.)     ; break;
                default: s=fsave; goto error;    break; // syntax error
            }


        } else { // if not parens or a function, it must be a number

            x.val = mystrtod(s, &p); // my own version, lots faster, don't know why however
//           x.val = strtod(s, &p); // converts the text to a number, and tell us where it stopped
																								#ifdef Debug
																									printf("%*s  got next value  x.val=%.17g  characters scanned %d\n",level*3,".", x.val ,(int) (p-s) );
																								#endif
			if ( s == p ) {
				goto error; // if we couldn't scan past anything, it was either garbage or overflow
			}
            while (*p == ' ') {  // skipping whitespace but incrementing s as well
                s++; p++;
            }
            c = *p;
            if ( (c & 0x80)  || Operators[c] == NOT_IN_SET_7F) {
                s++;
                goto error;// return a status value that also returns the position of the error
            }
            if ( s  == p && s[1] != '(') { // this means we have a bad function name
            	goto error;// return a status value that also returns the position of the error
            }
            s = p;
        }
        while (*s == ' ') s++; // skip whitespace
        c = *s;
        if ( c == '*' ) {  // handle 2 char operators, ** << >>
        	if ( s[1] == '*' ) { // we see ** then eat one more * and set the operator token
        		s++;
        		c = EXP_OPER;
        	}
		} else if ( c == '<') {
        	if ( s[1] == '<' ) { // ditto with <<
        		s++;
        		c = SHIFTLEFT_OPER;
        	}
		} else if ( c == '>') {
        	if ( s[1] == '>' ) { // and with >>
        		s++;
        		c = SHIFTRIGHT_OPER;
        	}
		}

        
        s++; // now we must have an operator, it's allways number op, or () op, or func() op
																								#ifdef Debug
																									printf("%*s  got next operator or terminal  [%c%c] \n",level*3,".",c=='\n' ? 'n' : c, c=='\n' ? 'l' : ' ');
																								#endif
        switch (x.op = c) { // only 7 precendence levels so only need a stack of 6, but we allocate 7 for safety
            case EXP_OPER: 			x.prec = 6; break; // don't forget with unary +/- we set the prec to a litteral 6, above
            									       // if we add more operators with more prec levels, make sure to adjust that too
            case '*':
            case '/':
            case '%': 				x.prec = 5; break;
            
            case '+':
            case '-':				x.prec = 4; break;
            
            case SHIFTLEFT_OPER:
            case SHIFTRIGHT_OPER: 	x.prec = 3; break;
            
            case '&':
            case '^': // exclusive or
            case '|':				x.prec = 2; break;

            case ':':				x.prec = 1; break;
           
            case '\n':
            case '\0':
            case ')': 				x.prec = 0; x.op = 0; s--; break; // here on close paren or null char or newline
            default:  goto error; // here on a non valid operator and return a status value that also returns the position of the error
        }
																								#ifdef Debug
																									printf("%*s  %s  [%c%c] prec(%d)\n",level*3,".",x.prec==0? "terminal": "operator",c=='\n' ? 'n' : c, c=='\n' ? 'l' : ' ',x.prec);
																								#endif
																								#ifdef Debuga
																								        x.address = NULL;
																								#endif
        while (sp > stack && x.prec <= sp[-1].prec) { 
																								#ifdef Debug
																								   printf("%*s  stack accum operation sp : [%zd]  sp-op(%c)  sp-prec( %d) x.val=%.17g\n",level*3,".", (sp - stack),sp[-1].op ,sp[-1].prec,x.val );
																								#endif
             switch ((--sp)->op) {                       // unwind the stack of operations and accum the pending values
                case '^': 			 x.val = (DOUBLE_REAL) (llrint(sp->val) ^  llrint(x.val)); break;
                case '&': 			 x.val = (DOUBLE_REAL) (llrint(sp->val) &  llrint(x.val)); break;
                case '|': 			 x.val = (DOUBLE_REAL) (llrint(sp->val) |  llrint(x.val)); break;
                case SHIFTLEFT_OPER: x.val = (DOUBLE_REAL) (llrint(sp->val) << llrint(x.val)); break;
                case SHIFTRIGHT_OPER: x.val =(DOUBLE_REAL) (llrint(sp->val) >> llrint(x.val)); break;
                case EXP_OPER: x.val = pow(sp->val, x.val); break;
                case '%': x.val = fmod(sp->val, x.val); break;
                case '*': x.val = sp->val * x.val; break;
                case '/': 
                    if ( x.val == 0.0 ) {
                	    error_code = 0x4000; // divide by zero floating error
                	    goto error;
                    }
		    x.val = sp->val / x.val;
                break;
                case ':': 
                    if (sp->val > x.val) {
                	    x.val = 1;
                    } else if (sp->val < x.val) {
                        x.val = -1;
                    } else {
                        x.val = 0;
                    }
                break;
                case '+': x.val = sp->val + x.val; break;
                case '-': x.val = sp->val - x.val; break;
            }
																								#ifdef Debug
																								   printf("%*s  stack pop opcode (%c) sp->val=%.17g    new value of x.val=%.17g\n",level*3,".",sp[0].op,sp->val ,x.val);
																									#ifdef Debuga
																									   sp[0].pushed = 0;
																									   sp[0].address = &sp[0];
																									#endif
																								#endif
        }
        if (!x.op) break;

        *sp++ = x;
																								#ifdef Debug
																								   printf("%*s  pushed sp :  [%zd] x.val=%.17g x.op (%c) prec(%d)\n",level*3,".", (sp - stack),x.val, x.op,x.prec   ); 
																								#endif
    }
    if (endp) *endp = (char*)s;
    *thestatus = 0;
																								#ifdef Debug
																								   printf("%*s  done return value x.val=%.17g\n",level*3,".",x.val);
																							    #endif
    return x.val;
error:
    if (endp) *endp = (char*)s;
    *thestatus = (int) ( s-stringIn) +  error_code;
																								#ifdef Debug
																								   printf("%*s  error return %d\n",level*3,".",*thestatus);
																								#endif
    return 0.0;
} // end evaluate_d

 
#ifdef Separate_ll_function
static INT_64 evaluate_ll( char* stringIn, char** endp, int* thestatus,int level) {
    struct operand_ll {
																							#ifdef Debuga
																							        void *address; // for debug
																							        long  pushed;
																							#endif
        INT_64 val  ; // the value of the current computation, either on the stack or in x
        char   op   ; // this is one of the litteral operators, but could be any constant
        char   prec ; // precedence level, currently 5 is the max
    } stack[HIGHEST_PREC+1] = { 
																							#ifdef Debuga
																							    NULL, 0,
																							#endif
    0,0,0 },x, *sp; // stack pointer and the active opperand we're building up 
																							#ifdef Debuga
																							    stack[0].address = (void *)&stack[0].address; // debug
																							    stack[1].address = (void *)&stack[1].address;
																							    stack[2].address = (void *)&stack[2].address;
																							    stack[3].address = (void *)&stack[3].address;
																							    stack[4].address = (void *)&stack[3].address;
																							#endif
    INT_64 fval; // the value of the argument of a function
    char* p;     // pointer to input string text, next token after call to functrion arg or mystrtod
    char* s;     // pointer to current location in the input string
    char *fsave = stringIn; // the pointer of a function call name, in case of an error
    int news;    // used to compute the new s after seeing a function( 
	int status,error_code = 0x1000;
    enum function_codes f; 
    char c;      // used to hold the current character, to avoid another *s

    s  = stringIn;
    while (*s == ' ') s++; // skip whitespace
    
    c = *s;
    sp = stack;
    
    if ( ++level > RECURSION_MAX) {
																							#ifdef Debug
																							    printf("%*s  evaluate_d: exceeded recursion depth %d at %s\n",level*3,".",level,s);
																							#endif
    	*thestatus = ERROR_RECURSION;
    	return 0;
    }
    
																							#ifdef Debuga
																							    x.pushed = (long)0xbeefcafe; // debug
																							#endif
																							#ifdef Debug
																							    printf("%*s  evaluate_d: depth=%d /%3zd byt stringIn= [%s] \n",level*3,".",level ,(thestatus - &status)*sizeof(int),s);
																							#endif
    if (c  == '+' || c == '-') { // check for initial unitary + or - 
    	x.val = 0;
    	x.op  = c;
    	x.prec = HIGHEST_PREC;  // give it highest priority 
    	*sp++ = x;  // pretend we started with a 0, so it's 0+... or 0-...
        s++;
	
																							#ifdef Debug
                                                                                                printf("%*s  push a 0 with unary [%c] op at prec(%d)\n", level*3, ".", c, x.prec);
																							#endif
    }
    while (1) {
        
        while (*s == ' ') s++; // skip over spaces, we don't allow tabs here, much faster than using isspace
        if (*s == '(') {       // opening parens, we know they are balanced, we did that check separately
																							#ifdef Debug
																								printf("%*s  opening parens  \n" ,level*3,".");
																							#endif
            x.val = evaluate_ll(s + 1, &p,&status,level); // recursive call
            s = p;
            if (status != STATUS_OK) {
            	if ( status < 0x1000 ) {
	            	*thestatus = status;
	                return 0;
            	}
            	goto error; // return a status value that also returns the position of the error
            }
            if (*s == ')')s++;
        } else if ( *s >= 'a' && *s <= 'z') { // a function is just a pair of ()'s with a value
            f = afunc(s,&news);               // functions are the only thing that has a-z letters
            fsave = s + 1;
            if ( f == eERROR ) {
                error_code = 0x3000;
            	goto error;// return a status value that also returns the position of the error
            }
																							#ifdef Debug
																								printf("%*s  func begins with [%c%c...] recur... \n" ,level*3,".",s[0],s[1]);
																							#endif
            s = s+news;
            fval = evaluate_ll(s + 1, &p,&status,level); // evaluate what's in the parens, recursively
            s = p;
            if (status != STATUS_OK) {
            	if ( status < 0x1000 ) {
	            	*thestatus = status;
	                return 0;
            	}
            	goto error; // return a status value that also returns the position of the error
            }
            if (*s == ')')s++;
             
  
             switch (f) { // do the corresponding function on our value HERE to add additional functions
    
                case eABS:       
                    if ( fval < 0 ) {
                        fval = -fval;
                    }
                    x.val = fval;
                    break;
                //case eINT:    
					//if ( fval < 0 ) {
                		//x.val = floor(fval + 1.0);
					//} else {
                		//x.val = floor(fval);
					//}
                	//break;
                case eEQ:    x.val = (fval  == 0 ) ? 1 : 0     ; break;                                  
                case eNE:    x.val = (fval  != 0 ) ? 1 : 0     ; break;
                case eLT:    x.val = (fval  <  0 ) ? 1 : 0     ; break;
                case eLE:    x.val = (fval  <= 0 ) ? 1 : 0     ; break;
                case eGT:    x.val = (fval  >  0 ) ? 1 : 0     ; break;
                case eGE:    x.val = (fval  >= 0 ) ? 1 : 0     ; break;
                
                //case eRAD:    x.val =fval * (3.141592653589793238462643/180.)     ; break;
                //case eFLOOR: x.val = floor(fval)      ; break;                                  
                //case eSIN:    x.val = sin(fval);      ; break;
                //case eCOS:    x.val = cos(fval);      ; break;
                //case eSQRT:   x.val = sqrt(fval);     ; break;
                //case eROUND:  x.val = round(fval)     ; break;
                //case eRAD:    x.val =fval * (3.141592653589793238462643/180.)     ; break;
                default: s=fsave; goto error;    break; // syntax error
            }


        } else { // if not parens or a function, it must be a number

            x.val = mystoll(s, &p); // my own version, lots faster, don't know why however
//           x.val = strtod(s, &p); // converts the text to a number, and tell us where it stopped
																								#ifdef Debug
																									printf("%*s  got next value  x.val=%lld  characters scanned %d\n",level*3,".", x.val ,(int) (p-s) );
																								#endif
			if ( s == p ) {
				goto error; // if we couldn't scan past anything, it was either garbage or overflow
			}

            while (*p == ' ') {  // skipping whitespace but incrementing s as well
                s++; p++;
            }
            c = *p;
            if ( (c & 0x80)  || Operators[c] == NOT_IN_SET_7F) {
                s++;
                goto error;// return a status value that also returns the position of the error
            }
            if ( s  == p && s[1] != '(') { // this means we have a bad function name
            	goto error;// return a status value that also returns the position of the error
            }
            s = p;
        }
        while (*s == ' ') s++; // skip whitespace
        c = *s;
        if ( c == '*' ) {  // handle 2 char operators, ** << >>
        	if ( s[1] == '*' ) { // we see ** then eat one more * and set the operator token
        		s++;
        		c = EXP_OPER;
        	}
		} else if ( c == '<') {
        	if ( s[1] == '<' ) { // ditto with <<
        		s++;
        		c = SHIFTLEFT_OPER;
        	}
		} else if ( c == '>') {
        	if ( s[1] == '>' ) { // and with >>
        		s++;
        		c = SHIFTRIGHT_OPER;
        	}
		}

        
        s++; // now we must have an operator, it's allways number op, or () op, or func() op
																								#ifdef Debug
																									printf("%*s  got next operator or terminal  [%c%c] \n",level*3,".",c=='\n' ? 'n' : c, c=='\n' ? 'l' : ' ');
																								#endif
        switch (x.op = c) { // only 7 precendence levels so only need a stack of 6, but we allocate 7 for safety
            case EXP_OPER: 			x.prec = 6; break; // don't forget with unary +/- we set the prec to a litteral 6, above
            									       // if we add more operators with more prec levels, make sure to adjust that too
            case '*':
            case '/':
            case '%': 				x.prec = 5; break;
            
            case '+':
            case '-':				x.prec = 4; break;
            
            case SHIFTLEFT_OPER:
            case SHIFTRIGHT_OPER: 	x.prec = 3; break;
            
            case '&':
            case '^': // exclusive or
            case '|':				x.prec = 2; break;

            case ':':				x.prec = 1; break;
            
            case '\n':
            case '\0':
            case ')': 				x.prec = 0; x.op = 0; s--; break; // here on close paren or null char or newline
            default:  goto error; // here on a non valid operator and return a status value that also returns the position of the error
        }
																								#ifdef Debug
																									printf("%*s  %s  [%c%c] prec(%d)\n",level*3,".",x.prec==0? "terminal": "operator",c=='\n' ? 'n' : c, c=='\n' ? 'l' : ' ',x.prec);
																								#endif
																								#ifdef Debuga
																								        x.address = NULL;
																								#endif
        while (sp > stack && x.prec <= sp[-1].prec) { 
																								#ifdef Debug
																								   printf("%*s  stack accum operation sp : [%zd]  sp-op(%c)  sp-prec( %d) x.val=%lld\n",level*3,".", (sp - stack),sp[-1].op ,sp[-1].prec,x.val );
																								#endif
             switch ((--sp)->op) {                       // unwind the stack of operations and accum the pending values
                case '^': 			 x.val = (INT_64) ((sp->val) ^  (x.val)); break;
                case '&': 			 x.val = (INT_64) ((sp->val) &  (x.val)); break;
                case '|': 			 x.val = (INT_64) ((sp->val) |  (x.val)); break;
                case SHIFTLEFT_OPER: x.val = (INT_64) ((sp->val) << (x.val)); break;
                case SHIFTRIGHT_OPER: x.val =(INT_64) ((sp->val) >> (x.val)); break;
                case EXP_OPER: x.val = Powll(sp->val, x.val); break;
                case '%': x.val = sp->val % x.val;break;
                case '*': x.val = sp->val * x.val; break;
                case '/': 
	                if ( x.val == 0 ) {
	                	error_code = 0x4000; // divide by zero integer error
	                	goto error;
	                }
	                x.val = sp->val / x.val;
                break; 
                case ':': 
                    if (sp->val > x.val) {
                	    x.val = 1;
                    } else if (sp->val < x.val) {
                        x.val = -1;
                    } else {
                        x.val = 0;
                    }
                break;
                case '+': x.val = sp->val + x.val; break;
                case '-': x.val = sp->val - x.val; break;
            }
																								#ifdef Debug
																								   printf("%*s  stack pop opcode (%c) sp->val=%lld    new value of x.val=%lld\n",level*3,".",sp[0].op,sp->val ,x.val);
																									#ifdef Debuga
																									   sp[0].pushed = 0;
																									   sp[0].address = &sp[0];
																									#endif
																								#endif
        }
        if (!x.op) break;

        *sp++ = x;
																								#ifdef Debug
																								   printf("%*s  pushed sp :  [%zd] x.val=%lld x.op (%c) prec(%d)\n",level*3,".", (sp - stack),x.val, x.op,x.prec   ); 
																								#endif
    }
    if (endp) *endp = (char*)s;
    *thestatus = 0;
																								#ifdef Debug
																								   printf("%*s  done return value x.val=%lld\n",level*3,".",x.val);
																							    #endif
    return x.val;
error:
    if (endp) *endp = (char*)s;
    *thestatus = (int) ( s-stringIn) +  error_code;
																								#ifdef Debug
																								   printf("%*s  error return %d\n",level*3,".",*thestatus);
																								#endif
    return 0;
} // end evaluate_ll

#endif


/* 

double  version of the evaluator

*/



int evaluate_d_expression(char* stringIn ,DOUBLE_REAL* result) { // evaluate an expression, 
    int status;
    if ( !Defined ) { // first time we define our letters and numbers
    	setupascii();
    }
    status = eval0x(stringIn, 1); // quick syntax check for illegal characters and unbalanced parens
    //status = eval0 (stringIn, 1); // quick syntax check for illegal characters and unbalanced parens
    if (status  != 0 ) {
    	return status; // error codes, 2=unbalanced parens, 0x1000+ = error with pointer to problem +0x1000
    }
    *result = evaluate_d(stringIn,NULL,&status,0); // now do the evaluate
    return status;
}



/* 

INT_32  version of the evaluator

*/


int evaluate_l_expression(char* stringIn ,INT_32* result) { // evaluate an expression, 
    int status;
    if ( !Defined ) { // first time we define our letters and numbers
    	setupascii();
    }
    status = eval0x(stringIn, 2); // quick syntax check for illegal characters and unbalanced parens
    //status = eval0 (stringIn, 2); // quick syntax check for illegal characters and unbalanced parens
    if (status  != 0 ) {
    	return status; // error codes, 2=unbalanced parens, 0x1000+ = error with pointer to problem +0x1000
    }
    *result = (INT_32)lrint(evaluate_d(stringIn,NULL,&status,0)); // now do the evaluate
    return status;
}
/* 

INT_64  version of the evaluator

*/


int evaluate_ll_expression(char* stringIn ,INT_64 * result) { // evaluate an expression, 
    int status;
    if ( !Defined ) { // first time we define our letters and numbers
    	setupascii();
    }
    status = eval0x(stringIn, 2); // quick syntax check for illegal characters and unbalanced parens
    //status = eval0 (stringIn, 2); // quick syntax check for illegal characters and unbalanced parens
    if (status  != 0 ) {
    	return status; // error codes, 2=unbalanced parens, 0x1000+ = error with pointer to problem +0x1000
    }
#ifdef Separate_ll_function
	*result = evaluate_ll(stringIn,NULL,&status,0); // now do the evaluate, lost too much precision converting from double
#else
    *result = (INT_64)llrint(evaluate_d(stringIn,NULL,&status,0));; // now do the evaluate
#endif    
    return status;
}

/* 

main test program to test for timing and debuggin all 3 varity's of expressions

*/

#ifdef Main
#define atest(Name,Type) Type Name(Type i,char *str) {\
	Type j;              \
	j = (Type)strlen(str);    \
	return j+i;           \
}

atest(theint,int)

atest(thelong,long)




int main(int argc, char* argv[]) {
int iii;

iii = theint(10,"a string");

long lll;

lll = thelong(10,"a string");
    printf("overflow checks:\n");
#ifdef windows
    INT_32 my_32bit_int = 0x7fffffff;
    INT_64 my_64bit_int = 0x7fffffffffffffff;
    printf("w   32 = %ld   %lx\n", my_32bit_int, my_32bit_int);
    my_32bit_int++;
    printf("+1  32 = %ld  %lx\n", my_32bit_int, my_32bit_int);

    printf("    64 = %lld    %llx\n", my_64bit_int, my_64bit_int);
    my_64bit_int++;
    printf("+1  64 = %lld   %llx\n", my_64bit_int, my_64bit_int);
#else
    INT_32 my_32bit_int = 0x7fffffff;
    INT_64 my_64bit_int = 0x7fffffffffffffff;
    printf("L  32 = %d    %x\n", my_32bit_int, my_32bit_int);
    my_32bit_int++;
    printf("+1 32 = %d   %x\n", my_32bit_int, my_32bit_int);
    
    printf("   64 = %lld    %llx\n", my_64bit_int, my_64bit_int);
    my_64bit_int++;
    printf("+1 64 = %lld   %llx\n\n", my_64bit_int, my_64bit_int);
#endif



    int status,n,i;
    char buf[100];
    DOUBLE_REAL ansd;
    INT_64 ansll;
    INT_32 ansl; 

    int doint = 0; // doing integer mode 1 = INT_32, 2 = INT_64, else DOUBLE_REAL float (0)

    char* newp = NULL;
    static  char badbuf[] = {"10000/0\n"};
    badbuf[3] = 0x81;
    //setupascii();
   // dump_arrays("a:\\arrays.h");
    //char* Sin; // used for debug only
	//status = evaluate_ll_expression(badbuf,&ansll);
	//status = evaluate_ll_expression("10000/0\n",&ansll);
	status = evaluate_ll_expression("eq(5:4)*10+lt(4:6)*10\n",&ansll);
	//status = evaluate_ll_expression("0+0x10000000000000000\n",&ansll);
	//status = evaluate_d_expression("1+100000000000000000000000000000000000\n",&ansd);
	//status = evaluate_d_expression("1-100000000000000000000000000000000000\n",&ansd);
	//status = evaluate_d_expression("2<<4\n",&ansd);
	//status = evaluate_d_expression("2>>4\n",&ansd);
	//status = evaluate_d_expression("2^4\n",&ansd);
	//status = evaluate_d_expression("sin(rad(90))\n",&ansd);
	//status = evaluate_d_expression("sin(rad(45))\n",&ansd);
	//status = evaluate_d_expression("cos(rad(45))\n",&ansd);
	//status = evaluate_d_expression("cos(rad(0))\n",&ansd);
    //status = evaluate_d_expression(")abs(abs(abs(22)))\n",&ansd);
    //status = evaluate_d_expression("abs(abs(abs(-1+(1-2)+(4*3))))\n",&ansd);
    //status = evaluate_d_expression("abs(-1) 22\n",&ansd);
    //status = evaluate_d_expression("123+456 -abs(-0x5_55) 22\n",&ansd);
    // ansd = mystrtod_old(Sin = "123 xx", &newp); // _old only here to compare with mystrtod, currently #if'd out
    //ansd = mystrtod_old(Sin = "1e-2+", &newp);
    //ansd = mystrtod(Sin = "1e-2+", &newp);
    //ansd = mystrtod_old(Sin = "1e+2+", &newp);
    //ansd = mystrtod(Sin = "1e+2+", &newp);
    //ansd = mystrtod(Sin = "123.+", &newp);
    //ansd = mystrtod(Sin = "1233.0000000000321+", &newp);
    //ansd = mystrtod(Sin = "1231.000000000000321+", &newp);
    //ansd = mystrtod(Sin = "1232.0000000000000000000000321+", &newp);
    //ansd = mystrtod(Sin = "123+", &newp);
    //ansd = mystrtod(Sin = "123.4_56+", &newp);
    //ansd = mystrtod(Sin = "123.456e2+", &newp);
    //ansd = mystrtod(Sin = "0xff_ff+", &newp);
    //ansd = mystrtod(Sin = "0xff_ff+", &newp);
    //ansd = mystrtod(Sin = "-456+", &newp);
    //ansd = mystrtod(Sin = "456+", &newp);
    //ansd = mystrtod(Sin = "0+", &newp);
    //ansd = mystrtod(Sin="-123e+10+",&newp);
    //ansd = mystrtod(Sin="-123e+10+",&newp);
    //ansd = mystrtod(Sin = "-123.456E+2+", &newp);
    //ansd = mystrtod(Sin = "123.456e2+", &newp);
    //ansd = mystrtod(Sin = ".456+", &newp);
    //ansd = mystrtod(Sin = "-.456+", &newp);
 //   int foobar = 1 * + + -  * 2;
    
 //   ansll=mystoll("-123,456",&newp);
 //   ansll=mystoll("0xbe_EF", &newp);
 //   ansll=mystoll("0b1111_1111 ", &newp);
    //    printf("begin %d\n" ,argc);fflush(stdout);
    printf("begin %d\n" ,666);fflush(stdout);

	if (/* argc > 2  && */  0) { // selects  timing mode or interactive mode 0 = interactive

		//        printf("0 [%s] \n", argv[0] );
		//        printf("1 [%s] \n", argv[1] );
		//        printf("2 [%s] \n", argv[2] );
		//        n  = mystol(argv[1],NULL);
		
		 while (1) {
			 if ( doint == 1 ) { // 1 is integer eval
			 
				n = 30*1000*1000;
//                n = 100*1000*1000;
                char* expr = "10**2*100+100*(abs(-100))";
//                 expr = "2*5+1";
                printf("start l\n");
				for (i = 0; i < n; i++) {
					status = evaluate_l_expression(expr, &ansl);
					if (ansl >= 0 && i < 4 || i > n-5) {
						printf("i=[%d] stat=%d %d\n", i, status, ansl);
					}
				}
				printf("end l\n");

			} else if ( doint == 2 ) {// 2 is INT_64 eval

				n = 30*1000*1000;
//                n = 100*1000*1000;
                char* expr = "10**2*100+100*(abs(-100))";
//                 expr = "2*5+1";
                printf("start ll\n");
				for (i = 0; i < n; i++) {
					status = evaluate_ll_expression(expr, &ansll);
					if (ansll > 0 && i < 4 || i > n-5) {
						printf("i=[%d] stat=%d %lld\n", i, status, ansll);
					}
				}
				printf("end ll\n");
			 } else {
				   
				n = 30*1000*1000;
				char* expr = "-floor(10.01**2.00001*100+100*(abs(-100.)))";
				printf("start d\n");
				for (i = 0; i < n; i++) {
					status = evaluate_d_expression(expr, &ansd);
					if (ansd > 0 && i < 4 || i > n-5) {
						printf("i=[%d] stat=%d %.17g \n", i, status, ansd);
					}
				}
				printf("end d\n");
				
			 }  

		 } // end while     
			
	} else { // --------------------------- debugging interactively ----------------
		char *reason;
		 printf("\nInteractive mode output dec hex octal (q for quit):\n");
		 if ( doint == 0) { // DOUBLE_REAL
		 
//		   status = evaluate_d_expression("1.5+abs(-2-1)",&ansd);
//		   printf(" -> %.17g  status=%d\n", ansd,status );
			for (;;) {
				printf("eval- d> ");
				fflush(stdout);
				if (!fgets(buf, sizeof buf, stdin) || (buf[0] == 'q' && buf[1] == '\n')) {
					break;
				}
				status = evaluate_d_expression(buf,&ansd);
				if ( status == 0 ) {
					printf(" -> %.17g  status=%d ok\n", ansd, status);
				} else {
					if (status >= 0x1000) {
						if       ( status >= 0x4000 ) {
							reason = "Divide by zero";
						} else if ( status >= 0x3000 ) {
							reason = "invalid function";
						} else if ( status >= 0x2000 ) {
							reason = "invalid character";
						
						} else {
							reason = "grammar error";
						
						}
						printf("      %*s  ^^^^ %x = %s\n", status & 0xff, "", status,reason);
						continue;
					} else if (status == ERROR_PAREN_NOT_BALANCED) {
						printf("Unbalanced parens\n");
						continue;
					} else if (status == ERROR_RECURSION) {
						printf("exceeded recursion depth %d\n",RECURSION_MAX);
						continue;
					}
					printf(" -> %.17g  status = %x  error @ %d\n", ansd, status,status & 0xff);
					
				}
			}
		 } else if ( doint == 2 ) {// 2 is INT_64 eval
			status = evaluate_ll_expression("1+abs(-2-1)",&ansll);
			printf(" -> %lld  status=%d\n", ansll,status );
			for (;;) {
				printf("eval-ll> ");
				fflush(stdout);
				if (!fgets(buf, sizeof buf, stdin) || (buf[0] == 'q' && buf[1] == '\n')) {
					break;
				}
				status = evaluate_ll_expression(buf,&ansll);
				if ( status == 0 ) {
					printf(" -> %lld %016llx %022llo status=%d ok\n", ansll,ansll,ansll, status);
				} else {
					if (status >= 0x1000) {
						if       ( status >= 0x4000 ) {
							reason = "Divide by zero";
						} else if ( status >= 0x3000 ) {
							reason = "invalid function";
						} else if ( status >= 0x2000 ) {
							reason = "invalid character";
						
						} else {
							reason = "grammar error";
						
						}
						printf("      %*s  ^^^^ %x = %s\n", status & 0xff, "", status,reason);
						continue;
					} else if (status == ERROR_PAREN_NOT_BALANCED) {
						printf("Unbalanced parens\n");
						continue;
					} else if (status == ERROR_RECURSION) {
						printf("exceeded recursion depth %d\n",RECURSION_MAX);
						continue;
					}
					printf(" -> %lld status = %x  error @ %d\n", ansll, status,status & 0xff);
					
				}
			}
		 } else { // INT_32
				 
			status = evaluate_l_expression("1+abs(-2-1)",&ansl);
			printf(" -> %d  status=%d\n", ansl,status );
			for (;;) {
				printf("eval- l> ");
				fflush(stdout);
				if (!fgets(buf, sizeof buf, stdin) || (buf[0] == 'q' && buf[1] == '\n')) {
					break;
				}
				status = evaluate_l_expression(buf,&ansl);
				if ( status == 0 ) {
					printf(" -> %d   %08x    %011o    status=%d ok\n", ansl,ansl,ansl, status);
				} else {
					if (status >= 0x1000) { 
						if       ( status >= 0x4000 ) {
							reason = "Divide by zero";
						} else if ( status >= 0x3000 ) {
							reason = "invalid function";
						} else if ( status >= 0x2000 ) {
							reason = "invalid character";
						
						} else {
							reason = "grammar error";
						
						}
						printf("      %*s  ^^^^ %x = %s\n", status & 0xff, "", status,reason);
						continue;
					} else if (status == ERROR_PAREN_NOT_BALANCED) {
						printf("Unbalanced parens\n");
						continue;
					} else if (status == ERROR_RECURSION) {
						printf("exceeded recursion depth %d\n",RECURSION_MAX);
						continue;
					}
					printf(" -> %d status = %x  error @ %d\n", ansl, status,status & 0xff);
					
				}
			}
		}       
    }
    return 0;
}
#endif

#ifdef Main2

#include <stdio.h>
#include <stdbool.h>
#include <math.h> // For pow function

// Function to evaluate an arithmetic expression
double evaluate_expression(char* expr) {
	int status;
	double ansd;
	status = evaluate_d_expression(expr, &ansd);
    if (status !=0) printf(" status = %x",status);
    return ansd;
};


// Function to compare floating point numbers with tolerance
bool compare_double(double a, double b, double epsilon) {
    return fabs(a - b) < epsilon;
}

// Function to run test cases
void run_test(int test_number, const char* expr, double expected_result, double epsilon) {
    double result = evaluate_expression(expr);
    if (compare_double(result, expected_result, epsilon)) {
        printf("Test %d passed for expression: %s\n", test_number, expr);
    } else {
        printf("Test %d failed for expression: %s\n", test_number, expr);
        printf("Expected: %.10lf, Actual: %.10lf\n", expected_result, result);
    }
}

int main() {
    // Define test cases
    const char* expressions[] = {
        "-(3 + 4)", "-5 * -2",
        "1000000 * 1000000", "999999 / 999",
        "0.1 + 0.2", "0.3 - 0.1",
        "2 * 3 + 4 / 2", "3 + 4 * 2 - 6 / 2",
        "((2 + 3) * (4 - 1)) / (5 - 2)", "(((1 + 2) * (3 - 1)) + 4) / 2",
        "(-2) ** 3", "(-4) ** 0.5",
        "2 ** 0.5", "4 ** (1 / 3.0)",
        "2 * (3 + 4) - (5 / 2) + 6 ** 2", "((3 + 2) * (7 - 4)) / 2 - 1",
        "-(+2 * -3)", "-(-4) * (-(5 - 2))"
    };

    double expected_results[] = {
         -7, 10,
        1e12, 1000,
        0.3, 0.2,
        11, 9,
        15, 4,
        -8, -2,
        sqrt(2), 1.587401051,
        50, 6,
        -6, 12
    };

    // Define tolerance for floating point comparison
    double epsilon = 0.0000001; // Adjust as needed

    // Run test cases
    int num_tests = sizeof(expressions) / sizeof(expressions[0]);
    for (int i = 0; i < num_tests; ++i) {
        run_test(i + 1, expressions[i], expected_results[i], epsilon);
    }

    return 0;
}
#endif

//#define Main3

#ifdef Main3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure for stack
typedef struct {
    double* array;
    int top;
    int capacity;
} Stack;

// Function to create a stack
Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (double*)malloc(stack->capacity * sizeof(double));
    return stack;
}

// Function to check if the stack is full
int isFull(Stack* stack) {
    return stack->top == stack->capacity - 1;
}

// Function to check if the stack is empty
int isEmpty(Stack* stack) {
    return stack->top == -1;
}

// Function to push an element onto the stack
void push(Stack* stack, double item) {
    if (isFull(stack)) {
        printf("Stack Overflow\n");
        exit(EXIT_FAILURE);
    }
    stack->array[++stack->top] = item;
}

// Function to pop an element from the stack
double pop(Stack* stack) {
    if (isEmpty(stack)) {
        printf("Stack Underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack->array[stack->top--];
}

// Function to evaluate an arithmetic expression
double evaluateExpression(const char* expr) {
    Stack* stack = createStack(strlen(expr));
    double num = 0.0;
    double result = 0.0;
    int sign = 1;
    int i = 0;

    while (expr[i] != '\0') {
        char c = expr[i];
        if (isdigit(c)) {
            while (isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            push(stack, sign * num);
            num = 0.0;
            sign = 1;
        } else if (c == '+') {
            sign = 1;
            i++;
        } else if (c == '-') {
            sign = -1;
            i++;
        } else if (c == '*') {
            double operand1 = pop(stack);
            double operand2 = 0.0;
            i++;
            while (expr[i] == ' ') {
                i++;
            }
            if (expr[i] == '-') {
                sign = -1;
                i++;
            }
            while (isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            operand2 = sign * num;
            num = 0.0;
            push(stack, operand1 * operand2);
            sign = 1;
        } else if (c == '/') {
            double operand1 = pop(stack);
            double operand2 = 0.0;
            i++;
            while (expr[i] == ' ') {
                i++;
            }
            if (expr[i] == '-') {
                sign = -1;
                i++;
            }
            while (isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            operand2 = sign * num;
            num = 0.0;
            if (operand2 == 0.0) {
                printf("Division by zero error\n");
                exit(EXIT_FAILURE);
            }
            push(stack, operand1 / operand2);
            sign = 1;
        } else if (c == '(') {
            push(stack, result);
            push(stack, sign);
            result = 0.0;
            sign = 1;
            i++;
        } else if (c == ')') {
            double signFactor = pop(stack);
            double prevResult = pop(stack);
            result = prevResult + signFactor * result;
            i++;
        } else if (c == ' ') {
            i++;
        }
    }
    
    while (!isEmpty(stack)) {
        result += pop(stack);
    }

    free(stack->array);
    free(stack);
    
    return result;
}

int main() {
    // Example usage
    const char* expression = "(2 + 3) * (4 - 1)";
    double result = evaluateExpression(expression);
    printf("Result of expression '%s' = %.2f\n", expression, result);
    return 0;
}


#endif
