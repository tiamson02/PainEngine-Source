#ifndef __WINE__
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif
#endif

#ifndef __WINE_OCIDL_H
#define __WINE_OCIDL_H

#if defined(__WINE__) && !defined(INITGUID) && !defined(__WINE_INCLUDE_OCIDL)
#error DO NOT INCLUDE DIRECTLY
#endif

#include "objbase.h"
#define __WINE_INCLUDE_OLEIDL
#include "oleidl.h"
#undef __WINE_INCLUDE_OLEIDL

#define __WINE_INCLUDE_OAIDL
#include "oaidl.h"
#undef __WINE_INCLUDE_OAIDL

#include "wine/obj_olefont.h"
#include "wine/obj_picture.h"

#include "wine/obj_control.h"
#include "wine/obj_connection.h"
#include "wine/obj_property.h"
#include "wine/obj_oleundo.h"

#endif /* __WINE_OCIDL_H */


