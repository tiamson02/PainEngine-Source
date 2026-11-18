// HeapDumpCompare.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <windows.h>
#include <shellapi.h>

//----------------------------------------------------------------------------------------------------------------------------------

void CompareFiles(char *File1, char *File2);

//----------------------------------------------------------------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 3)
	{
		CompareFiles(argv[1], argv[2]);
	}
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------

class TEXTFILE
{
public:

	void BuildLineList(void)
	{
		char	*Ln;
		int		 LnCount = 0;
		char	*p;

		Ln = Buffer;
		LnCount = 0;

		while(Ln <(Buffer+Size))
		{
			p = strchr(Ln, 13);
			if (p)
			{
				LnCount++;
				if (p[1]==10)
					p+=2;
				else
					p+=1;
				Ln = p;
			}
			else
				break;
		}

		Ln = Buffer;
		Lines = 0;
		Line = (char**)malloc(sizeof(char*)*LnCount);

		while(Ln <(Buffer+Size))
		{
			p = strchr(Ln, 13);
			if (p)
			{
				p[0]=0;
				if (p[1]==10)
					p+=2;
				else
					p+=1;
				Line[Lines++] = Ln;
				Ln = p;
			}
			else
				break;
		}
	}

	void Compact(void)
	{
		for (int i=0; i<Lines; i++)
		{
			if (Line[i] == NULL)
			{
				memmove(&Line[i], &Line[i+1], sizeof(char*)*(Lines-i));
				i--;
				Lines--;
			}
		}
	}

	TEXTFILE(const char *Filename)
	{
		FILE	*fp;

		fp = fopen(Filename, "rb");
		if (fp)
		{
			printf("Loading %s\n", Filename);

			fseek(fp, 0, SEEK_END);
			Size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			Buffer = (char*)malloc(Size+1);
			memset(Buffer, 0, (Size+1));
			fread(Buffer, 1, Size, fp);

			fclose(fp);

			BuildLineList();
		}
	}

	~TEXTFILE()
	{
		free(Buffer);
		free(Line);
	}

private:

	char	*Buffer;
	int		Size;

public:

	char	**Line;
	int		Lines;

};

//----------------------------------------------------------------------------------------------------------------------------------

void OutputTable(FILE *fp, char *Line, long &Total)
{
	char	Data[4][512];
	char	*p;
	char	comma = ',';

	memset(Data[0], 0, sizeof(Data[0]));
	memset(Data[1], 0, sizeof(Data[0]));
	memset(Data[2], 0, sizeof(Data[0]));
	memset(Data[3], 0, sizeof(Data[0]));

	if (atoi(Line)==0)
	{
		p = strchr(Line, ',');
		memcpy(Data[0], Line, p-Line);
		Line = p+1;
	}
	else
	{
		p = strchr(Line, ',');
		if (p)
		{
			p = strchr(p+1, ',');
			Line = strchr(p+1, ',')+1;
			p = strchr(Line, ',');
			memcpy(Data[0], Line, p-Line);
			Line = p+1;
		}
		else
		{
			comma = ' ';
			p = strchr(Line, ' ');
			p = strchr(p+1, ' ');
			while(p[0]==' ') p++;
			Line = strchr(p+1, ' ')+1;
			p = strchr(Line, ' ');
			memcpy(Data[0], Line, p-Line);
			while(p[0]==' ') p++;
			Line = p+1;
		}
	}

	p = strchr(Line, comma);
	memcpy(Data[1], Line, p-Line);
	Line = p+1;

	p = strrchr(Line, '(');
	memcpy(Data[2], Line, p-Line);
	Line = p+1;

	memcpy(Data[3], Line, strlen(Line));
	*strrchr(Data[3], ')')=0;

	if (strchr(Data[2],comma))
	{
		char	*end;

		p = strchr(Data[2],comma);
		end = p;
		while(	end[0]==comma || end[0]=='Y' || end[0]=='N')
			end++;

		memmove(p, end, strlen(p));
		p[0]=' ';
	}
	

	if (Data[0][0]==0 || (Data[0][0]==32 && Data[0][1]==0))											strcpy(Data[0], "&nbsp;");
	if (Data[1][0]==0 || (Data[1][0]==32 && Data[1][1]==0))											strcpy(Data[1], "&nbsp;");
	if (Data[2][0]==0 || (Data[2][0]==32 && Data[2][1]==0))											strcpy(Data[2], "&nbsp;");
	if (Data[3][0]==0 || (Data[3][0]==32 && Data[3][1]==0) || (Data[3][0]==')' && Data[3][1]==0))	strcpy(Data[3], "&nbsp;");

	if (atoi(Data[1]))
		Total += atoi(Data[1]);

	fprintf(fp, "<tr>\r\n");
	fprintf(fp, "<td><p align=\"right\"><font face=\"Verdana\" size=\"2\">%s</font></p></td>\r\n", Data[0]);
	fprintf(fp, "<td><p align=\"center\"><font face=\"Verdana\" size=\"2\">%s</font></p></td>\r\n", Data[1]);
	fprintf(fp, "<td><p align=\"left\"><font face=\"Verdana\" size=\"2\">%s</font></p></td>\r\n", Data[2]);
	fprintf(fp, "<td><p align=\"left\"><font face=\"Verdana\" size=\"2\">%s</font></p></td>\r\n", Data[3]);
	fprintf(fp, "</tr>\r\n");
}

//----------------------------------------------------------------------------------------------------------------------------------

void BlankAddress(char *dst, const char *ln)
{
	char	*p;
	
	p = strchr((char*)ln, ',');
	if (p)
	{
		p = strchr((char*)p+1, ',');
		p = strchr((char*)p+1, ',');
	}
	else
	{
		p = strchr((char*)ln,  ' ');
		while(p[0]==' ') p++;
		p = strchr((char*)p+1, ' ');
		while(p[0]==' ') p++;
		p = strchr((char*)p+1, ' ');
		while(p[0]==' ') p++;
	}

	strcpy(dst, p+1);

	p = dst;	//strstr(dst, ",0x");
	p[ 2] = '0';
	p[ 3] = '0';
	p[ 4] = '0';
	p[ 5] = '0';
	p[ 6] = '0';
	p[ 7] = '0';
	p[ 8] = '0';
	p[ 9] = '0';

	p = strchr(dst, ')');
	if (p)	p[0]=0;
}

bool SimularMatch(const char *Line, TEXTFILE &TF)
{
	char	L1[512];
	char	L2[512];

	BlankAddress(L1, Line);

	for (int i=0; i<TF.Lines; i++)
	{
		BlankAddress(L2, TF.Line[i]);
		if (_stricmp(L1,L2)==0)
		{
			TF.Line[i] = NULL;
			TF.Compact();
			return(true);
		}
	}
	return(false);
}

//----------------------------------------------------------------------------------------------------------------------------------

unsigned int HexFromAscii(char *Line)
{
	unsigned int	Value = 0;
	char			temp[]="00000000";

	memcpy(temp, Line, 8);
	_strupr(temp);
	Line = temp;

	for (int i=0; i<4; i++)
	{
		Value <<= 8;
		if (Line[0]>='0' && Line[0]<='9')	Value |= (Line[0]-'0')<<4;
		else								Value |= (Line[0]-'A'+10)<<4;

		if (Line[1]>='0' && Line[1]<='9')	Value |= (Line[1]-'0');
		else								Value |= (Line[1]-'A'+10);

		Line+=2;
	}

	return(Value);
}

//----------------------------------------------------------------------------------------------------------------------------------

#define MAXMEM ((128*1024)/32)

void CompareFiles(char *File1, char *File2)
{
	TEXTFILE		TF1(File1);
	TEXTFILE		TF2(File2);
	int				i,j;
	char			Used[MAXMEM];
	unsigned int	Address;
	long			Total;
	char			temp[512];
	char			Outname[_MAX_PATH];
	char			*p;

	for (i=0; i<MAXMEM; i++)
		Used[i]=0;

	for (i=0; i<TF1.Lines; i++)
		if (TF1.Line[i][0]==' ' || TF1.Line[i][0]=='|' || TF1.Line[i][0]==0)
			TF1.Line[i] = NULL;

	for (i=0; i<TF2.Lines; i++)
		if (TF2.Line[i][0]==' ' || TF2.Line[i][0]=='|' || TF2.Line[i][0]==0)
			TF2.Line[i] = NULL;


	for (i=0; i<TF1.Lines; i++)
	{
		if ((i & 255)==0)
			printf(".");
		for (j=0; j<TF2.Lines; j++)
		{
			if (TF1.Line[i] && TF2.Line[j] && _stricmp(TF1.Line[i],TF2.Line[j])==0)
			{
				TF1.Line[i] = NULL;
				TF2.Line[j] = NULL;
				break;
			}
		}
	}

	printf("<>");
	TF1.Compact();
	printf("<>");
	TF2.Compact();

	//create output file....
	strcpy(temp, File1);
	p = strrchr(temp, '.');
	if (p)	p[0] = 0;
	strcpy(Outname, temp);

	strcpy(temp, File2);
	p = strrchr(temp, '.');
	if (p)	p[0] = 0;
	strcat(Outname, temp);
	strcat(Outname, ".html");

	FILE	*fp = fopen(Outname, "wt");
	if (fp)
	{
		fprintf(fp, "<html>\r\n");
		fprintf(fp, "<head>\r\n");
		fprintf(fp, "<meta http-equiv=\"Content-Type\"\r\n");
		fprintf(fp, "content=\"text/html; charset=iso-8859-1\">\r\n");
		fprintf(fp, "<title>Heap Dump Analysys</title>\r\n");
		fprintf(fp, "</head>\r\n");
		fprintf(fp, "<body>\r\n");

		fprintf(fp, "<p align=\"center\"><font size=\"4\" face=\"Verdana\"><strong><u>Heap Dump Analysys</u></strong></font></p>\r\n");

		//----- moved ----

		fprintf(fp, "<br><br><br>");

		fprintf(fp, "<p align=\"center\"><font size=\"3\" face=\"Verdana\"><strong>Moved Allocations</strong></font></p><div align=\"center\"><center>\r\n");
		fprintf(fp, "<table border=\"1\" cellpadding=\"3\" cellspacing=\"1\">\r\n");

		OutputTable(fp, "<b>Address</b>,<b>Size</b>,<b>Function</b> (<b>Tag</b>)", Total);

		Total = 0;
		for (i=0; i<TF1.Lines; i++)
		{
			if (SimularMatch(TF1.Line[i], TF2))
			{
				if ((i&255)==0) 
					printf(".");

				OutputTable(fp, TF1.Line[i], Total);

				Address	= HexFromAscii(strstr(TF1.Line[i], "0x")+2)/(MAXMEM*32);
				assert(Address<MAXMEM);
				Used[Address]=1;

				TF1.Line[i] = NULL;
			}
		}
		OutputTable(fp, "-----,-----,----- (-----)", Total);
		sprintf(temp, "<b>Total</b>,%d, ()", Total);
		OutputTable(fp, temp, Total);

		TF1.Compact();

		fprintf(fp, "</table>\r\n");
		fprintf(fp, "<br><hr><br>\r\n");

		//----- deleted ----

		if (TF1.Lines)
		{
			fprintf(fp, "<p align=\"center\"><font size=\"3\" face=\"Verdana\"><strong>Deleted Allocations</strong></font></p><div align=\"center\"><center>\r\n");
			fprintf(fp, "<table border=\"1\" cellpadding=\"3\" cellspacing=\"1\">\r\n");

		OutputTable(fp, "<b>Address</b>,<b>Size</b>,<b>Function</b> (<b>Tag</b>)", Total);

			Total = 0;
			for (i=0; i<TF1.Lines; i++)
			{
				if ((i&255)==0) 
					printf(".");

				if (!SimularMatch(TF1.Line[i], TF2))
				{
					OutputTable(fp, TF1.Line[i], Total);

					Address	= HexFromAscii(strstr(TF1.Line[i], "0x")+2)/(MAXMEM*32);
					assert(Address<MAXMEM);
					Used[Address]=3;
				}
			}
			OutputTable(fp, "-----,-----,----- (-----)", Total);
		sprintf(temp, "<b>Total</b>,%d, ()", Total);
			OutputTable(fp, temp, Total);

			fprintf(fp, "</table>\r\n");
			fprintf(fp, "<br><hr><br>\r\n");
		}

		//---------- new --------------

		fprintf(fp, "<p align=\"center\"><font size=\"3\" face=\"Verdana\"><strong>New Allocations</strong></font></p><div align=\"center\"><center>\r\n");
		fprintf(fp, "<table border=\"1\" cellpadding=\"3\" cellspacing=\"1\">\r\n");

		OutputTable(fp, "<b>Address</b>,<b>Size</b>,<b>Function</b> (<b>Tag</b>)", Total);

		TF2.Compact();
		Total = 0;
		for (i=0; i<TF2.Lines; i++)
		{
				if ((i&255)==0) 
					printf(".");

			OutputTable(fp, TF2.Line[i], Total);

			Address	= HexFromAscii(strstr(TF2.Line[i], "0x")+2)/(MAXMEM*32);
			assert(Address<MAXMEM);
			//ticken line as this is the most serious info
			Used[Address-1]=2;
			Used[Address+0]=2;
			Used[Address+1]=2;
		}
		OutputTable(fp, "-----,-----,----- (-----)", Total);
		sprintf(temp, "<b>Total</b>,%d, ()", Total);
		OutputTable(fp, temp, Total);

		fprintf(fp, "</table>\r\n");
		fprintf(fp, "<br><hr><br>\r\n");

		//----- done ----

		fprintf(fp, "</center></div>\r\n");

		//--- memory bar ----
		typedef struct {
			int	top,left,width,height,colour;
		}MBDATA;
		MBDATA	mbdata[MAXMEM];
		int		mbcount = MAXMEM;

		for (i=0; i<MAXMEM; i++)
		{
			mbdata[i].top = 60;
			mbdata[i].left = i;
			mbdata[i].width = 1;
			mbdata[i].height = 20;

			switch (Used[i])
			{
				case 0:	mbdata[i].colour = i>1024?0xbfbfbf:0x9f9f9f; break;		//nothing
				case 1:	mbdata[i].colour = 0x0000ff; break;		//moved
				case 2:	mbdata[i].colour = 0xff0000; break;		//new
				case 3:	mbdata[i].colour = 0xff00ff; break;		//deleted
			}
		}

		//optimize mbdata;
		for (i=0; i<mbcount-1; i++)
		{
				if ((i&255)==0) 
					printf(".");

			if (mbdata[i].colour == mbdata[i+1].colour)
			{
				mbdata[i+1].width += mbdata[i].width;
				mbdata[i+1].left   = mbdata[i].left;

				memmove(&mbdata[i], &mbdata[i+1], sizeof(MBDATA)*(mbcount-i));
				mbcount--;
				i--;
			}
		}

		//draw mbdata
		for (i=0; i<mbcount; i++)
			fprintf(fp, "<DIV style=\"position:absolute;top:%dpx;left:%dpx;width:%dpx;height:%dpx;background-color:#%6.6x\"></DIV>\r\n", mbdata[i].top,mbdata[i].left+10,mbdata[i].width,mbdata[i].height,mbdata[i].colour);

		for (i=0; i<=MAXMEM; i+=32)
		{
			fprintf(fp, "<DIV style=\"position:absolute;top:80px;left:%dpx;width:1px;height:10px;background-color:#000000\"></DIV>\r\n", 10+i);
			if (i<320)
				fprintf(fp, "<DIV style=\"position: absolute; top:90px; left:%dpx;\"><font size=\"1\" face=\"Verdana\">%d</font></DIV>\r\n", 7+i, i/32);
			else
				fprintf(fp, "<DIV style=\"position: absolute; top:90px; left:%dpx;\"><font size=\"1\" face=\"Verdana\">%d</font></DIV>\r\n", 4+i, i/32);
		}

		fprintf(fp, "</body>\r\n");
		fprintf(fp, "</html>\r\n");

		fclose(fp);
	}

	ShellExecute(NULL, "open", Outname, NULL, NULL, SW_SHOWNORMAL);
}

//----------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------------------------------------
