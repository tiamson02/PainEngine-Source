//-----------------------------------------------------------------------------
//                       AsciiToJis - String converter
//
// String converter, required by Memory Card manager
//
// $Workfile: AsciiToJis.c $
// $Author: Hbodasin $
// $Date: 26/06/03 14:16 $
// $Revision: 3 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Legacy code by Tom Garrard, adapted for PS2 Shell project compliance by
// Hemal Bodasing
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include <shellCore.h>


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

//ASCII code to Shift-JIS code transfer function
const unsigned short ASCII_TABLE[3][2] = 
{
	{0x824f, 0x30}, /* 0-9  */
	{0x8260, 0x41}, /* A-Z  */
	{0x8281, 0x61}, /* a-z  */
};

//ASCII code to Shift-JIS code transfer table (kigou)
const unsigned short ASCII_K_TABLE[] = 
{
		0x8140,         /*   */
        0x8149,         /* ! */
        0x8168,         /* " */
        0x8194,         /* # */
        0x8190,         /* $ */
        0x8193,         /* % */
        0x8195,         /* & */
        0x8166,         /* ' */
        0x8169,         /* ( */
        0x816a,         /* ) */
        0x8196,         /* * */
        0x817b,         /* + */
        0x8143,         /* , */
        0x817c,         /* - */
        0x8144,         /* . */
        0x815e,         /* / */
        0x8146,         /* : */
        0x8147,         /* ; */
        0x8171,         /* < */
        0x8181,         /* = */
        0x8172,         /* > */
        0x8148,         /* ? */
        0x8197,         /* @ */
        0x816d,         /* [ */
        0x818f,         /* \ */
        0x816e,         /* ] */
        0x814f,         /* ^ */
        0x8151,         /* _ */
        0x8165,         /* ` */
        0x816f,         /* { */
        0x8162,         /* | */
        0x8170,         /* } */
        0x8150,         /* ~ */
};


const unsigned short ASCII_SPECIAL_TABLE[] = 
{
		' ',
        '!',
		'"',
		'#',
		'$',
		'%',
		'&',
		'\'',
		'(',
		')',
		'*',
		'+',
		',',
		'-',
		'.',
		'/',
		':',
		';',
		'<',
		'=',
		'>',
		'?',
		'@',
		'[',
		'\\',
		']',
		'^',
		'_',
		'`',
		'{',
		'|',
		'}',
		'~'
};


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef struct
{
	unsigned char byte1;
	unsigned char byte2;
} DoubleChar;

typedef union
{
	unsigned short 	sjisChar;
	DoubleChar 		doubleChar;
} SJIS_STRUCT;


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AsciiToJis()
// Description	: This function converts ascii strings to shift-JIS strings
// Parameters	: dest		: Pointer to destination shift-JIS string
//				  string	: Input ascii string
// Returns		: Nothing
// Notes		: -
//-----------------------------------------------------------------------------
void shellCoreAsciiToJis(unsigned char* dest, const char* string)
{	
	int     i;
	
	int sjis_code;
	int ascii_code;
	unsigned char stmp = '0';
	unsigned char stmp2 = '0';
	
	int ctr=0;
	
	unsigned char *dest2;
	
	dest2 = dest;
	
	// clear destination to padded spaces
	for( i=0; i<32; i++ )
	{
		*dest2++ = 0x0;
		*dest2++ = 0x0;
	}
	
	while(*string) {		// What the hell kind of indenting is this then?? :)
		stmp2 = 0;
		ascii_code = *string++;
		if ((ascii_code >= 0x20) && (ascii_code <= 0x2f))
			stmp2 = 1;
		else
			if ((ascii_code >= 0x30) && (ascii_code <= 0x39))
				stmp = 0;
			else
                if ((ascii_code >= 0x3a) && (ascii_code <= 0x40))
					stmp2 = 11;
                else
					if ((ascii_code >= 0x41) && (ascii_code <= 0x5a))
                        stmp = 1;
					else
						if ((ascii_code >= 0x5b) && (ascii_code <= 0x60))
							stmp2 = 37;
						else
							if ((ascii_code >= 0x61) && (ascii_code <= 0x7a))
								stmp = 2;
							else
								if ((ascii_code >= 0x7b) && (ascii_code <= 0x7e))
									stmp2 = 63;
								else {
									PRINT("Bad ASCII code 0x%x\n", ascii_code);
									//exit(1);
									ASSERT(0, "");
								}
								
								if(stmp2)
									sjis_code = ASCII_K_TABLE[ascii_code - 0x20 - (stmp2 - 1)];
								else
									sjis_code = ASCII_TABLE[stmp][0] + ascii_code - ASCII_TABLE[stmp][1];
								
								/* Write sjis */
								
								ctr++;
								*dest++ = (unsigned char) ((sjis_code&0xff00)>>8);
								*dest++ = (unsigned char) (sjis_code&0xff);
	}
}



//-----------------------------------------------------------------------------
// JisToAscii()
// Description	: This function converts shift-JIS strings to ascii strings
// Parameters	: dest		: Pointer to destination ascii string
//				  string	: Input shift-JIS string
// Returns		: 0 on success; -1 on failure
// Notes		: -
//-----------------------------------------------------------------------------
int shellCoreJisToAscii(char* destination, const unsigned char* string)
{
	int i;
	int set;
	
	//unsigned char *src;
	char *dest;
	SJIS_STRUCT test;
	
	const unsigned char* src = string;
	dest= destination;
	
	do
	{
		test.doubleChar.byte2 = *src;
		src++;
		test.doubleChar.byte1 = *src;
		src++;
		
		// work out what ascii character we have here, and copy it into the destination string
		if((test.sjisChar >= 0x8260) && (test.sjisChar <= 0x8279))
		{
			// A(65) - Z(90)
			*dest++ = (char) (65 + (test.sjisChar - 0x8260));
		}
		else
			if ((test.sjisChar >= 0x8281) && (test.sjisChar <= 0x829a))
			{
				// a(97) - z(122)
				*dest++ = (char) (97 + (test.sjisChar - 0x8281));
			}
			else
				if ((test.sjisChar >= 0x824f) && (test.sjisChar <= 0x8258))
				{
					// 0(48) - 9(57)
					*dest++ = (char) (48 + (test.sjisChar - 0x824f));
				}
				else
				{
					// now look for other characters
					set = 0;
					for( i=0 ; i<33 ; i++)
					{
						if(ASCII_K_TABLE[i] == test.sjisChar)
						{
							*dest++ = (char) ASCII_SPECIAL_TABLE[i];
							set = 1;
						}
					}
					
					PRINT("Unknown SJIS code, possibly NULL %x,%x,%x\n",test.sjisChar,test.doubleChar.byte1,test.doubleChar.byte2);
					
					if( (set==0) && (test.sjisChar != 0) )
					{
						PRINT("BAD CODE\n");
						return -1;
					}
				}
				
	} while ((test.doubleChar.byte1 != 0) ||  (test.doubleChar.byte2 != 0));
	
	return 0;
}
