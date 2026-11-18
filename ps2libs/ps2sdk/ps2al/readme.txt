Introduction
------------

copy from website

Installation
----------------

1) Put the Multistream distribution in a directory called "MultiStream" next to the "ps2al" one.
2) run make
3) Add ps2al to your include and lib paths and add libopenal.a to your libs. You can now use AL/al.h and AL/alut.h in your programs
4) Use "stdio_multistream.h" instead of <stdio.h> in your programs.
5) If you find that the commands you want to use are missing (very likely since I only did the ones I needed for my application) then feel free to add them and I'll give you cvs access.

Initialization
--------------

alutInit works very similar to ps2gl's glutInit in which a search is done on the argv parameters for 'iop_module_path=' to get the location of the IRX files. Since Multistream is needed (stream.irx), this is even more important since the default ('host0:/usr/local/sce/iop/modules') is unlikely to contain it. ps2al also uses this path information to set up the device prefix to add to all fopen operations (cd,host,atwinmon).

If you use both ps2gl and ps2al, then you need put glutInit before alutInit. Unfortunately glutInit removes 'iop_module_path=' when it finds it which is needed by alutInit. A way around this is to do the following:

char **copyarguments(int argc,char **argv)
{
	int i;
	char **origargv=(char**)malloc(argc*sizeof(char *));
	for (i=0;i<argc;i++)
	{
		if (strnicmp(argv[i],"atfile:",7)==0) // atwinmon
		{
			// cut off elf name
			char *strptr = argv[i];
			static char newargv[256];
			while (*strptr != ',')
			{
				strptr++;
			}
			strptr++; // keep comma
			*strptr = 0;
			strcpy(newargv,"iop_module_path=");
			strcat(newargv,argv[i]);
			strcat(newargv,"IRX");
			argv[i]=newargv; // replace original with atwinmon prefix			
		}

		origargv[i]=(char*)malloc(strlen(argv[i])+1);
		strcpy(origargv[i],argv[i]);
	}
	return(origargv);
}

void freearguments(int argc,char **argv)
{
	int i;
	for (i=0;i<argc;i++)
		free (argv[i]);
	free(argv);
}

#include "GL/glut.h"
#include "AL/alut.h"

int main(int argc, char **argv)

{
	int origargc=argc;
	char **origargv=copyarguments(argc,argv);	
	glutInit(&argc, argv);
	alutInit(&origargc, origargv);

...

}

I've also included a way to make running over AtWinMon automatically tie into glutInit/alutInit although as you can see it assumes the IRX's are sitting in a subdirectory called 'IRX'.

Loading sounds
--------------

The alutLoadWAV* functions actually replace the .wav with a .vag and loads that

Contact
-------

Ben Crossman

bencrossman@mail.com
