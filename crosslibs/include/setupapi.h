/* Initial setupapi.h.

   FIXME: Missing A LOT of definitions / declarations!
*/

#ifndef __SETUPAPI__
#define __SETUPAPI__

#include "commctrl.h"

/* Define type for handle to a loaded inf file */
typedef PVOID HINF;

/* Define type for handle to a device information set */
typedef PVOID HDEVINFO;

/* Define type for setup file queue */
typedef PVOID HSPFILEQ;

/* inf structure. */
typedef struct _INFCONTEXT
{
   PVOID Inf;
   PVOID CurrentInf;
   UINT  Section;
   UINT  Line;
} INFCONTEXT, *PINFCONTEXT;

/* Device Information structure (references a device instance that is a member
   of a device information set) */
typedef struct _SP_DEVINFO_DATA
{
   DWORD cbSize;
   GUID  ClassGuid;
   DWORD DevInst;   /* DEVINST handle */
   DWORD Reserved;
} SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;


/*  Flags used by SetupDiGetClassDevs */
#define DIGCF_DEFAULT           0x00000001
#define DIGCF_PRESENT           0x00000002
#define DIGCF_ALLCLASSES        0x00000004
#define DIGCF_PROFILE           0x00000008
#define DIGCF_DEVICEINTERFACE   0x00000010

#define DIGCF_INTERFACEDEVICE DIGCF_DEVICEINTERFACE /* Don't use */


typedef struct _SP_DEVICE_INTERFACE_DATA {
  DWORD cbSize;
  GUID  InterfaceClassGuid;
  DWORD Flags;
  DWORD Reserved;
} SP_DEVICE_INTERFACE_DATA, *PSP_DEVICE_INTERFACE_DATA;
                                                                                                                                                        
#define SPINT_ACTIVE  0x00000001
#define SPINT_DEFAULT 0x00000002
#define SPINT_REMOVED 0x00000004


typedef UINT (CALLBACK *PSP_FILE_CALLBACK_A)( PVOID Context, UINT Notification,
                                              UINT Param1, UINT Param2 );
typedef UINT (CALLBACK *PSP_FILE_CALLBACK_W)( PVOID Context, UINT Notification,
                                              UINT Param1, UINT Param2 );
#define PSP_FILE_CALLBACK WINELIB_NAME_AW(PSP_FILE_CALLBACK_)


BOOL WINAPI SetupDiGetDeviceRegistryPropertyA( HDEVINFO DeviceInfoSet,
  PSP_DEVINFO_DATA DeviceInfoData,
  DWORD Property,
  PDWORD PropertyRegDataType,
  PBYTE PropertyBuffer,
  DWORD PropertyBufferSize,
  PDWORD RequiredSize );

BOOL WINAPI SetupDiGetDeviceRegistryPropertyW( HDEVINFO DeviceInfoSet,
  PSP_DEVINFO_DATA DeviceInfoData,
  DWORD Property,
  PDWORD PropertyRegDataType,
  PBYTE PropertyBuffer,
  DWORD PropertyBufferSize,
  PDWORD RequiredSize );
#define SetupDiGetDeviceRegistryProperty WINELIB_NAME_AW(SetupDiGetDeviceRegistryProperty)

BOOL WINAPI SetupDiEnumDeviceInfo( IN HDEVINFO  DeviceInfoSet, IN DWORD  MemberIndex, OUT PSP_DEVINFO_DATA  DeviceInfoData );

BOOL WINAPI SetupDiDestroyDeviceInfoList( HDEVINFO DeviceInfoSet );

HDEVINFO WINAPI SetupDiGetClassDevsA( const GUID* ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags );
HDEVINFO WINAPI SetupDiGetClassDevsW( const GUID* ClassGuid, PCWSTR Enumerator, HWND hwndParent, DWORD Flags );
#define SetupDiGetClassDevs WINELIB_NAME_AW(SetupDiGetClassDevs);

#endif /* __SETUPAPI__ */
