///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2002 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://wwhiz.com/LuaPlus/.
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdarg.h>
#include <wchar.h>
#include <assert.h>

#pragma warning(disable: 4505)
#pragma warning(disable: 4127)

#include "LuaLink.h"

#include "LuaFunctor.h"

struct lua_State;
struct lua_TObject;
struct lua_Debug;
typedef int (*lua_CFunction) (lua_State *L);
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);
typedef int (*lua_Chunkwriter) (lua_State *L, const void* p, size_t sz, void* ud);

extern "C" {
#include "include/Lua.h"
#include "include/lauxlib.h"
#include "LuaPlusAddons.h"
} // extern "C"

#define LUAPLUS_ENABLE_INLINES

#ifdef LUAPLUS_ENABLE_INLINES
#define LUAPLUS_INLINE inline
#else !LUAPLUS_ENABLE_INLINES
#define LUAPLUS_INLINE
#endif LUAPLUS_ENABLE_INLINES

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

class LuaStateOutFile;
class LuaState;
class LuaStackObject;

typedef int (*LuaStateCFunction)(LuaState* state);


struct LuaFunctorList
{
	LuaFunctorList() :
		name(NULL)
	{
	}

	LuaFunctorList(const char* _name, int (*f)(LuaState*)) :
		name(_name),
		luaFunctor(LuaFunctor(f))
	{
	}

    template <typename Callee>
	LuaFunctorList(const char* _name, const Callee& c, int (Callee::*f)(LuaState*)) :
		name(_name),
		luaFunctor(LuaFunctor(c, f))
	{
	}

	LuaFunctorList(const char* _name, const LuaFunctor& _functor) :
		name(_name),
		luaFunctor(_functor)
	{
	}

	const char *name;
	LuaFunctor luaFunctor;
};


} // namespace LuaPlus

#include "LuaStackObject.h"
#include "LuaObject.h"
#include "LuaState.h"
#include "LuaTableIterator.h"
#include "LuaObject.inl"
#include "LuaStateOutFile.h"
#include "LuaHelper.h"
#include "LuaAutoBlock.h"
#include "LuaStackTableIterator.h"