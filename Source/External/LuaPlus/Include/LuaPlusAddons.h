#pragma once

#define _countof(array) (sizeof(array)/sizeof(array[0]))
//#define lua_lock(L)		if (L->l_G  &&  L->l_G->lockFunc) (*L->l_G->lockFunc)(L->l_G->lockData)
//#define lua_unlock(L)	if (L->l_G  &&  L->l_G->unlockFunc) (*L->l_G->unlockFunc)(L->l_G->lockData)
#define lua_lock(L)
#define lua_unlock(L)

#define LUA_ALLOC_TEMP 1
typedef void* (*lua_ReallocFunction)(void* ptr, int oldsize, int size, void* data, const char* allocName, unsigned int flags);
typedef void (*lua_FreeFunction)(void* ptr, int oldsize, void* data);
LUA_API void lua_getdefaultmemoryfunctions(lua_ReallocFunction* reallocFunc, lua_FreeFunction* freeFunc, void** data);
LUA_API void lua_setdefaultmemoryfunctions(lua_ReallocFunction reallocFunc, lua_FreeFunction freeFunc, void* data);

struct GCState;
LUA_API void lua_setusergcfunction(lua_State *L, void (*userGCFunction)(struct GCState*));

LUA_API void* lua_getglobaluserdata(lua_State *L);
LUA_API void lua_setglobaluserdata(lua_State *L, void* globalUserData);

LUA_API void* lua_getstateuserdata(lua_State *L);
LUA_API void lua_setstateuserdata(lua_State *L, void* stateUserData);

LUA_API void lua_setlockfunctions(lua_State *L, void (*lockFunc)(void *), void (*unlockFunc)(void *), void *lockData);
LUA_API void lua_setfatalerrorfunction(lua_State *L, void (*fatalErrorFunc)());
LUA_API void lua_setminimumstringtablesize(int numstrings);
LUA_API void lua_setdefaulttagtablesize(int numtags);
LUA_API void lua_setminimumglobaltablesize(int numentries);
LUA_API void lua_setminimumauxspace(int size);

LUA_API int             lua_iswstring (lua_State *L, int index);
LUA_API const wchar_t  *lua_towstring (lua_State *L, int index);
LUA_API void  lua_pushlwstring (lua_State *L, const wchar_t *s, size_t len);
LUA_API void  lua_pushwstring (lua_State *L, const wchar_t *s);

#define lua_pushwliteral(L, s)	\
	lua_pushlwstring(L, L"" s, (sizeof(s)/sizeof(wchar_t))-1)

LUA_API void  lua_getdefaultmetatable (lua_State *L, int objindex);
LUA_API void  lua_setdefaultmetatable(lua_State *L, int type);

LUA_API void lua_newuserdatabox (lua_State *L, void *ptr);

LUA_API int lua_getn (lua_State *L, int index);

/* formats for Lua numbers */
#ifndef LUA_NUMBER_WSCAN
#define LUA_NUMBER_WSCAN	L"%lf"
#endif

#ifndef LUA_NUMBER_WFMT
#define LUA_NUMBER_WFMT		L"%.16g"
#endif

#define LUA_WSTRLIBNAME	"wstring"
LUALIB_API int luaopen_wstring (lua_State *L);

LUA_API unsigned char* lua_getcclosure_userdata(lua_State *L, int funcindex);
LUA_API void lua_setcclosure_userdata(lua_State *L, int funcindex, void* data, unsigned int size);

