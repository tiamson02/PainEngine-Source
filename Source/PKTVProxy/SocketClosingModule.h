
#ifndef __SOCKETCLOSINGMODULE_H__
#define __SOCKETCLOSINGMODULE_H__

#include "DynArray.h"
#include <winsock2.h>

class SocketClosingModule
{
	DynamicArray<SOCKET> mSocketsToClose;
public:
	SocketClosingModule();
	~SocketClosingModule();

	void AddSocketToClose( SOCKET socket );
	void Tick( void );
};

#endif	// __SOCKETCLOSINGMODULE_H__
