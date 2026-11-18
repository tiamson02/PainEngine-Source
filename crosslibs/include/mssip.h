/* basic headers required by stubs in crypt32.dll
 * this should eventually be replaced with a de-MS'd real mssip.h header.
 * FIXME: not for general consumption
 */


typedef struct SIP_ADD_NEWPROVIDER_ {
  DWORD cbStruct;
  GUID* pgSubject;
  WCHAR* pwszDLLFileName;
  WCHAR* pwszMagicNumber;
  WCHAR* pwszIsFunctionName;
  WCHAR* pwszGetFuncName;
  WCHAR* pwszPutFuncName;
  WCHAR* pwszCreateFuncName;
  WCHAR* pwszVerifyFuncName;
  WCHAR* pwszRemoveFuncName;
  WCHAR* pwszIsFunctionNameFmt2;
} SIP_ADD_NEWPROVIDER, *PSIP_ADD_NEWPROVIDER;

