
#include "Engine.h"
#include "IpAddress.h"
#include <winsock2.h>

// ---------------------------- GENERIC NET FUNCTIONS ---------------------------------------------------

bool Resolve( const char* hostName, IpAddress& ipAddr )
{
	char buf[256];
	ipAddr.Port = 0;

	if( !hostName )
		return false;

	// Strip surroundings
	if( ( hostName[0] == '"' ) || ( hostName[0] == '\'' ) )
	{
		::strcpy( buf, hostName+1 );
		buf[::strlen(buf)-1] = 0;
	}
	else
		::strcpy( buf, hostName );

	// Get port if present
	for( char* colon = buf; *colon ; ++colon )
	{
		if( *colon == ':' )
		{
			*colon = 0;
			ipAddr.Port = (short)atoi( colon + 1 );
			break;
		}
	}

	// Check for a.b.c.d form
	if( buf[0] >= '0' && buf[0] <= '9' )
		ipAddr.IP = inet_addr( buf );
	else
	{
		hostent* host = gethostbyname( hostName ); 
		if( !host || PF_INET != host->h_addrtype )
			return false;
		ipAddr.IP = *(DWORD*)(*host->h_addr_list);
	}

	return true;
}

String GetSocketErrorString( void )
{
	int error = WSAGetLastError();
	switch( error )
	{
	case WSAEINTR:				return String("WSAEINTR");
	case WSAEBADF:				return String("WSAEBADF");
	case WSAEACCES:				return String("WSAEACCES");
	case WSAEDISCON:			return String("WSAEDISCON");
	case WSAEFAULT:				return String("WSAEFAULT");
	case WSAEINVAL:				return String("WSAEINVAL");
	case WSAEMFILE:				return String("WSAEMFILE");
	case WSAEWOULDBLOCK:		return String("WSAEWOULDBLOCK");
	case WSAEINPROGRESS:		return String("WSAEINPROGRESS");
	case WSAEALREADY:			return String("WSAEALREADY");
	case WSAENOTSOCK:			return String("WSAENOTSOCK");
	case WSAEDESTADDRREQ:		return String("WSAEDESTADDRREQ");
	case WSAEMSGSIZE:			return String("WSAEMSGSIZE");
	case WSAEPROTOTYPE:			return String("WSAEPROTOTYPE");
	case WSAENOPROTOOPT:		return String("WSAENOPROTOOPT");
	case WSAEPROTONOSUPPORT:	return String("WSAEPROTONOSUPPORT");
	case WSAESOCKTNOSUPPORT:	return String("WSAESOCKTNOSUPPORT");
	case WSAEOPNOTSUPP:			return String("WSAEOPNOTSUPP");
	case WSAEPFNOSUPPORT:		return String("WSAEPFNOSUPPORT");
	case WSAEAFNOSUPPORT:		return String("WSAEAFNOSUPPORT");
	case WSAEADDRINUSE:			return String("WSAEADDRINUSE");
	case WSAEADDRNOTAVAIL:		return String("WSAEADDRNOTAVAIL");
	case WSAENETDOWN:			return String("WSAENETDOWN");
	case WSAENETUNREACH:		return String("WSAENETUNREACH");
	case WSAENETRESET:			return String("WSAENETRESET");
	case WSAECONNABORTED:		return String("WSWSAECONNABORTEDAEINTR");
	case WSAECONNRESET:			return String("WSAECONNRESET");
	case WSAENOBUFS:			return String("WSAENOBUFS");
	case WSAEISCONN:			return String("WSAEISCONN");
	case WSAENOTCONN:			return String("WSAENOTCONN");
	case WSAESHUTDOWN:			return String("WSAESHUTDOWN");
	case WSAETOOMANYREFS:		return String("WSAETOOMANYREFS");
	case WSAETIMEDOUT:			return String("WSAETIMEDOUT");
	case WSAECONNREFUSED:		return String("WSAECONNREFUSED");
	case WSAELOOP:				return String("WSAELOOP");
	case WSAENAMETOOLONG:		return String("WSAENAMETOOLONG");
	case WSAEHOSTDOWN:			return String("WSAEHOSTDOWN");
	case WSASYSNOTREADY:		return String("WSASYSNOTREADY");
	case WSAVERNOTSUPPORTED:	return String("WSAVERNOTSUPPORTED");
	case WSANOTINITIALISED:		return String("WSANOTINITIALISED");
	case WSAHOST_NOT_FOUND:		return String("WSAHOST_NOT_FOUND");
	case WSATRY_AGAIN:			return String("WSATRY_AGAIN");
	case WSANO_RECOVERY:		return String("WSANO_RECOVERY");
	case WSANO_DATA:			return String("WSANO_DATA");
	default:					return String("UNKNOWN ERROR");
	} 
}
