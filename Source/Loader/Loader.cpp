// Loader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Loader.h"

#include <shellapi.h>

#define PK_WINDOW_LABEL "Painkiller Safedisc CD"

bool Check( void );

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Set working dir to module dir
	char dir[1024];
	GetModuleFileName(hInstance,dir,sizeof(dir));
	if( ::strlen(dir) )
	{
		char* s;
		s = strrchr( dir, '\\' );
		if( s )
		{
			*(s+1)=0;
			SetCurrentDirectory(dir);
		}
		else
		{
			s = strrchr( dir, '/' );
			if( s )
			{
				*(s+1)=0;
				SetCurrentDirectory(dir);
			}
		}
	}

	char lang[10];
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, lang, 10 );

	char text[100] = "";

	if( ::strcmp( lang, "Polish" ) == 0 )
		::strcpy( text, "Proszê w³o¿yæ odpowiedni CD do napêdu" );
	else if( ::strncmp( lang, "Spanish", 7 ) == 0 )
		::strcpy( text, "Por favor, introduzca el CD-ROM correcto" );
	else if( ::strncmp( lang, "German", 6 ) == 0 )
		::strcpy( text, "Legen Sie die korrekte CD-ROM ein" );
	else if( ::strncmp( lang, "Italian", 7 ) == 0 )
		::strcpy( text, "Inserire il CD-ROM corretto" );
	else if( ::strncmp( lang, "French", 6 ) == 0 )
		::strcpy( text, "Veuillez insérer le CD-ROM approprié" );
	else
		::strcpy( text, "Please insert the correct CD-ROM" );


	do
	{
		if( Check() )
		{
			ShellExecute( NULL, "open", "PainGame.exe", lpCmdLine, NULL, SW_SHOWNORMAL );
			return 0;
		}
	}
	while( ::MessageBox( NULL, text, PK_WINDOW_LABEL, MB_RETRYCANCEL ) != IDCANCEL );
	return 0;
}

bool Check( void )
{
	//printf("[loader.cpp]::Check(): checking CD label\n");
	char dbits[111];
	GetLogicalDriveStrings( 111, dbits );
	DWORD mf, sf, sno;

	for( int i = 0; *(dbits+i); i+= 4 )
	{
		char vol[256];
		vol[0] = '\0';
		char drive[4];
		strncpy(drive,dbits+i,4);
		if( GetDriveType( drive ) == DRIVE_CDROM )
		{
			GetVolumeInformation( drive, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '3' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '3' ) && ( vol[7] == 0 ) ) ) )
				return true;
		}
	}
	return false;
}
