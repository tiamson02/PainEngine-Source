#ifndef LUAPLUS_LIB
	#ifdef LUAPLUS_BUILDDLL
		#define LUA_API __declspec(dllexport)
	#else
		#define LUA_API __declspec(dllimport)
		#ifdef _DEBUG
			#pragma comment(lib, "LuaPlusD.lib")
		#else _DEBUG
			#pragma comment(lib, "LuaPlus.lib")
		#endif _DEBUG
	#endif
#else //!LUASTATEDLL
	#define LUA_API
#endif

#define LUALIB_API LUA_API


