// Loader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BlackEditionLoader.h"

#include <shellapi.h>

#define PK_WINDOW_LABEL "Painkiller Black Edition Installer Loader"

void PrepareMessageBoxText( char* text );

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Test if Painkiller is running, and if it is, exit quietly
	CreateMutex( NULL, TRUE, "PCFPainEditor" );
	if( ERROR_ALREADY_EXISTS == GetLastError() )
		return 0;

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

	// Test if Painkiller is installed
	bool installed = false;
	{
		HKEY dirKey;
		if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Software\\PeopleCanFly\\Painkiller", 0, STANDARD_RIGHTS_READ, &dirKey ) == ERROR_SUCCESS )
		{
			installed = true;
			RegCloseKey( dirKey );
		}
	}

	// , and if it is, ask to install.
	if( installed )
	{
		char text[100] = "";
		PrepareMessageBoxText( (char*)text );
		if( ::MessageBox( NULL, text, PK_WINDOW_LABEL, MB_YESNO ) != IDYES )
			return 0;
	}

	ShellExecute( NULL, "open", "Content\\PKBE.exe", lpCmdLine, NULL, SW_SHOWNORMAL );
	return 0;
}

void PrepareMessageBoxText( char* text )
{
	char lang[10];
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, lang, 10 );

	if( ::strcmp( lang, "Polish" ) == 0 )
		::strcpy( text, "Painkiller jest, lub by³, zainstalowany na tym komputerze. Czy chcesz kontynuowaæ instalacjê?" );
	else if( ::strncmp( lang, "Spanish", 7 ) == 0 )
		::strcpy( text, "El instalador ha detectado que Painkiller ya está instalado. ?Desea continuar con la instalación?" );
	else if( ::strncmp( lang, "German", 6 ) == 0 )
		::strcpy( text, "Das Installationsprogramm hat festgestellt, dass Painkiller bereits installiert ist. Möchten Sie die Installation trotzdem fortsetzen?" );
	else if( ::strncmp( lang, "Italian", 7 ) == 0 )
		::strcpy( text, "Il programma di installazione ha individuato una installazione precedente di Painkiller. Continuare con l'installazione?" );
	else if( ::strncmp( lang, "French", 6 ) == 0 )
		::strcpy( text, "Le programme d'installation a détecté que Painkiller est deja installé. Souhaitez-vous poursuivre l'installation ?" );
	else
		::strcpy( text, "Painkiller is, or was at some time, installed here. Do you want to install?" );
}