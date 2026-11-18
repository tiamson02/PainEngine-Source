/*
GameSpy PT SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 2000 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
Tel: 949.798.4200
Fax: 949.798.4299
*/

#include <stdio.h>
#include "pt.h"
#include "../ghttp/ghttp.h"
#include "../stringutil.h"
#include "../available.h"

/************
** DEFINES **
************/
#define PTA_VERCHECK_URL    "http://motd.gamespy.com/motd/vercheck.asp"
#define PTA_MOTD_URL        "http://motd.gamespy.com/motd/motd.asp"
#define PTA_FILEPLANET_URL  "http://www.fileplanet.com/dlfileraw.asp"
#define MAX_MIRRORS         32

/**********
** TYPES **
**********/
typedef struct ptaPatchData
{
	ptPatchCallback callback;
	void * param;
} ptaPatchData;

typedef struct ptaFilePlanetInfoData
{
	int fileID;
	ptFilePlanetInfoCallback callback;
	void * param;
} ptaFilePlanetInfoData;

/************
** GLOBALS **
************/
static char URL[256];

/**************
** FUNCTIONS **
**************/
static const char * ptaGetKeyValue
(
	const char * buffer,
	const char * key
)
{
	static char value[256];
	const char * str;
	int len;

	str = strstr(buffer, key);
	if(!str)
		return NULL;
	str += strlen(key);
	len = strcspn(str, "\\");
	len = min(len, sizeof(value) - 1);
	memcpy(value, str, len);
	value[len] = '\0';

	return value;
}

static char Line[256];
static int ptaFillLine
(
	const char * buffer
)
{
	char * str = Line;
	int i;

	// Skip white space.
	////////////////////
	for(i = 0 ; isspace(*buffer) ; i++)
		buffer++;

	// Check for EOF.
	/////////////////
	if(*buffer == '\0')
		return EOF;

	// Copy off the line.
	/////////////////////
	for( ; *buffer && ((*buffer != 0x0A) && (*buffer != 0x0D)) ; i++)
	{
		if(i == sizeof(Line) - 1)
		{
			// This line is too big for our buffer.
			///////////////////////////////////////
			assert(0);
			return EOF;
		}
		*str++ = *buffer++;
	}
	*str = '\0';

	return i;
}

static void ptaCallPatchCallback
(
	ptaPatchData * data,
	PTBool available,
	PTBool mandatory,
	const char * versionName,
	int fileID,
	const char * downloadURL
)
{
	if(data->callback)
	{
#ifndef GSI_UNICODE
		data->callback(available, mandatory, versionName, fileID, downloadURL, data->param);
#else
		unsigned short versionName_W[255];
		unsigned short downloadURL_W[1024];
		UTF8ToUCS2String(versionName, versionName_W);
		AsciiToUCS2String(downloadURL, downloadURL_W);
		data->callback(available, mandatory, versionName_W, fileID, downloadURL_W, data->param);
#endif
	}

	gsifree(data);
}

// already returns ghttptrue
static GHTTPBool ptaPatchFailed
(
	ptaPatchData * data
)
{
	ptaCallPatchCallback(data, PTFalse, PTFalse, "", 0, "");

	return GHTTPTrue;
}

static void ptaCallFilePlanetInfoCallback
(
	ptaFilePlanetInfoData * data,
	PTBool found,
	const char * description,
	const char * size,
	int numMirrors,
	char ** mirrorNames,
	char ** mirrorURLs
)
{
	int i;

	if(data->callback)
	{
		if (!found)
			data->callback(data->fileID, PTFalse, NULL, NULL, 0, NULL, NULL, data->param);
		else
		{
#ifndef GSI_UNICODE
			data->callback(data->fileID, found, description, size, numMirrors, (const char**)mirrorNames, (const char**)mirrorURLs, data->param);
#else
			unsigned short description_W[255];
			unsigned short size_W[1024];
			unsigned short** mirrorNames_W;
			unsigned short** mirrorURLs_W;
			int i;

			UTF8ToUCS2String(description, description_W);
			AsciiToUCS2String(size, size_W);
			mirrorNames_W = UTF8ToUCS2StringArrayAlloc((const unsigned char**)mirrorNames, numMirrors);
			mirrorURLs_W = UTF8ToUCS2StringArrayAlloc((const unsigned char**)mirrorURLs, numMirrors);
			data->callback(data->fileID, found, description_W, size_W, numMirrors, (const unsigned short**)mirrorNames_W, (const unsigned short**)mirrorURLs_W, data->param);

			for (i=0; i < numMirrors; i++)
			{
				gsifree(mirrorNames[i]);
				gsifree(mirrorURLs[i]);
			}
			gsifree(mirrorNames);
			gsifree(mirrorURLs);
#endif
		}
	}

	for(i = 0 ; i < numMirrors ; i++)
	{
		gsifree(mirrorNames[i]);
		gsifree(mirrorURLs[i]);
	}

	gsifree(data);
}

// already returns ghttptrue
static GHTTPBool ptaFilePlanetInfoFailed
(
	ptaFilePlanetInfoData * data
)
{
	ptaCallFilePlanetInfoCallback(data, PTFalse, NULL, NULL, 0, NULL, NULL);

	return GHTTPTrue;
}

static GHTTPBool ptaPatchCompletedCallback
(
	GHTTPRequest request,
	GHTTPResult result,
	char * buffer,
	GHTTPByteCount bufferLen,
	void * param
)
{
	ptaPatchData * data = (ptaPatchData *)param;
	const char * value;
	PTBool mandatory;
	int fileID;
	char versionName[256];
	char downloadURL[101];

	GSI_UNUSED(bufferLen);
	GSI_UNUSED(request);

	// Check for success.
	/////////////////////
	if(result != GHTTPSuccess)
		return ptaPatchFailed(data);

	// Check for a patch.
	/////////////////////
	value = ptaGetKeyValue(buffer, "\\newver\\");
	if(!value || !atoi(value))
		return ptaPatchFailed(data);

	// Check the mandatory flag.
	////////////////////////////
	value = ptaGetKeyValue(buffer, "\\lockout\\");
	mandatory = (value && atoi(value));

	// Get the file id.
	///////////////////
	value = ptaGetKeyValue(buffer, "\\fpfileid\\");
	if(value)
		fileID = atoi(value);
	else
		fileID = 0;

	// Get the name.
	////////////////
	value = ptaGetKeyValue(buffer, "\\newvername\\");
	if(value)
	{
		strncpy(versionName, value, sizeof(versionName));
		versionName[sizeof(versionName) - 1] = '\0';
	}
	else
		versionName[0] = '\0';

	// Get the URL.
	///////////////
	value = ptaGetKeyValue(buffer, "\\dlurl\\");
	if(value)
	{
		strncpy(downloadURL, value, sizeof(downloadURL));
		downloadURL[sizeof(downloadURL) - 1] = '\0';
	}
	else
		downloadURL[0] = '\0';

	// Call the callback.
	/////////////////////
	ptaCallPatchCallback(data, PTTrue, mandatory, versionName, fileID, downloadURL);

	return GHTTPTrue;
}

PTBool ptCheckForPatchA
(
	int productID,
	const char * versionUniqueID,
	int distributionID,
	ptPatchCallback callback,
	PTBool blocking,
	void * param
)
{
	ptaPatchData * data;

	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return PTFalse;

	// Check the arguments.
	///////////////////////
	assert(versionUniqueID);
	if(!versionUniqueID)
		return PTFalse;
	assert(callback);
	if(!callback)
		return PTFalse;

	// Store some data.
	///////////////////
	data = (ptaPatchData *)gsimalloc(sizeof(ptaPatchData));
	if(!data)
		return PTFalse;
	memset(data, 0, sizeof(ptaPatchData));
	data->callback = callback;
	data->param = param;

	// Build the URL.
	/////////////////
	sprintf(URL, PTA_VERCHECK_URL "?productid=%d&versionuniqueid=%s&distid=%d&gamename=%s",
		productID,
		versionUniqueID,
		distributionID,
		__GSIACGamename);

	// Send the request.
	////////////////////
	if((ghttpGetFileA(URL, blocking, ptaPatchCompletedCallback, data) == -1) && !blocking)
		return PTFalse;

	return PTTrue;
}
#ifdef GSI_UNICODE
PTBool ptCheckForPatchW
(
	int productID,
	const unsigned short* versionUniqueID,
	int distributionID,
	ptPatchCallback callback,
	PTBool blocking,
	void * param
)
{
	char versionUniqueID_A[255];
	UCS2ToUTF8String(versionUniqueID, versionUniqueID_A);
	return ptCheckForPatchA(productID, versionUniqueID_A, distributionID, callback, blocking, param);
}
#endif

PTBool ptTrackUsageA
(
	int userID,
	int productID,
	const char * versionUniqueID,
	int distributionID,
	PTBool blocking
)
{
	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return PTFalse;

	// Check the arguments.
	///////////////////////
	assert(versionUniqueID);
	if(!versionUniqueID)
		return PTFalse;

	// Build the URL.
	/////////////////
	sprintf(URL, PTA_MOTD_URL "?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s",
		userID,
		productID,
		versionUniqueID,
		distributionID,
		GOAGetUniqueID(),
		__GSIACGamename);

	// Send the info.
	/////////////////
	if(ghttpGetFileA(URL, blocking, NULL, NULL) == -1)
		return PTFalse;

	return PTTrue;
}
#ifdef GSI_UNICODE
PTBool ptTrackUsageW
(
	int userID,
	int productID,
	const unsigned short* versionUniqueID,
	int distributionID,
	PTBool blocking
)
{
	char versionUniqueID_A[255];
	UCS2ToUTF8String(versionUniqueID, versionUniqueID_A);
	return ptTrackUsageA(userID, productID, versionUniqueID_A, distributionID, blocking);
}
#endif

PTBool ptCheckForPatchAndTrackUsageA
(
	int userID,
	int productID,
	const char * versionUniqueID,
	int distributionID,
	ptPatchCallback callback,
	PTBool blocking,
	void * param
)
{
	ptaPatchData * data;

	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return PTFalse;

	// Check the arguments.
	///////////////////////
	assert(versionUniqueID);
	if(!versionUniqueID)
		return PTFalse;
	assert(callback);
	if(!callback)
		return PTFalse;

	// Store some data.
	///////////////////
	data = (ptaPatchData *)gsimalloc(sizeof(ptaPatchData));
	if(!data)
		return PTFalse;
	memset(data, 0, sizeof(ptaPatchData));
	data->callback = callback;
	data->param = param;

	// Build the URL.
	/////////////////
	sprintf(URL, PTA_VERCHECK_URL "?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s",
		userID,
		productID,
		versionUniqueID,
		distributionID,
		GOAGetUniqueID(),
		__GSIACGamename);

	// Send the request.
	////////////////////
	if((ghttpGetFileA(URL, blocking, ptaPatchCompletedCallback, data) == -1) && !blocking)
		return PTFalse;

	return PTTrue;
}
#ifdef GSI_UNICODE
PTBool ptCheckForPatchAndTrackUsageW
(
	int userID,
	int productID,
	const unsigned short * versionUniqueID,
	int distributionID,
	ptPatchCallback callback,
	PTBool blocking,
	void * param
)
{
	char versionUniqueID_A[255];
	UCS2ToUTF8String(versionUniqueID, versionUniqueID_A);
	return ptCheckForPatchAndTrackUsageA(userID, productID, versionUniqueID_A, distributionID, callback, blocking, param);
}
#endif

static GHTTPBool ptaFilePlanetCompletedCallback
(
	GHTTPRequest request,
	GHTTPResult result,
	char * buffer,
	GHTTPByteCount bufferLen,
	void * param
)
{
	ptaFilePlanetInfoData * data = (ptaFilePlanetInfoData *)param;
	int len;
	char description[256];
	char size[64];
	char * mirrorNames[MAX_MIRRORS];
	char * mirrorURLs[MAX_MIRRORS];
	int i;
	char * str;

	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return PTFalse;

	GSI_UNUSED(request);
	GSI_UNUSED(bufferLen);

	// Check for success.
	/////////////////////
	if(result != GHTTPSuccess)
		return ptaFilePlanetInfoFailed(data);

	// Get the description.
	///////////////////////
	len = ptaFillLine(buffer);
	if(len == EOF)
		return ptaFilePlanetInfoFailed(data);
	buffer += len;
	strncpy(description, Line, sizeof(description));
	description[sizeof(description) - 1] = '\0';

	// Get the size.
	////////////////
	len = ptaFillLine(buffer);
	if(len == EOF)
		return ptaFilePlanetInfoFailed(data);
	buffer += len;
	strncpy(size, Line, sizeof(size));
	size[sizeof(size) - 1] = '\0';

	// Get the mirrors.
	///////////////////
	for(i = 0 ; (i < MAX_MIRRORS) && ((len = ptaFillLine(buffer)) != EOF) ; )
	{
		// Adjust the buffer.
		/////////////////////
		buffer += len;

		// Find the tab.
		////////////////
		str = strchr(Line, '\t');
		if(!str)
			continue;

		// Copy off the name.
		/////////////////////
		len = (str - Line);
		mirrorNames[i] = (char *)gsimalloc(len + 1);
		if(!mirrorNames[i])
			break;
		memcpy(mirrorNames[i], Line, len);
		mirrorNames[i][len] = '\0';

		// Copy off the URL.
		////////////////////
		str++;
		len = strlen(str);
		mirrorURLs[i] = (char *)gsimalloc(len + 1);
		if(!mirrorURLs[i])
		{
			gsifree(mirrorNames[i]);
			break;
		}
		strcpy(mirrorURLs[i], str);

		// One more mirror.
		///////////////////
		i++;
	}

	// Call the callback.
	/////////////////////
	ptaCallFilePlanetInfoCallback(data, PTTrue, description, size, i, mirrorNames, mirrorURLs);

	return GHTTPTrue;
}

PTBool ptLookupFilePlanetInfo
(
	int fileID,
	ptFilePlanetInfoCallback callback,
	PTBool blocking,
	void * param
)
{
	ptaFilePlanetInfoData * data;

	// Check the arguments.
	///////////////////////
	assert(callback);
	if(!callback)
		return PTFalse;

	// Store some data.
	///////////////////
	data = (ptaFilePlanetInfoData *)gsimalloc(sizeof(ptaFilePlanetInfoData));
	if(!data)
		return PTFalse;
	memset(data, 0, sizeof(ptaFilePlanetInfoData));
	data->callback = callback;
	data->param = param;
	data->fileID = fileID;

	// Build the URL.
	/////////////////
	sprintf(URL, PTA_FILEPLANET_URL "?file=%d&gamename=%s",
		fileID,
		__GSIACGamename);

	// Send the request.
	////////////////////
	if((ghttpGetFileA(URL, blocking, ptaFilePlanetCompletedCallback, data) == -1) && !blocking)
		return PTFalse;

	return PTTrue;
}
