///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2002-2003 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://wwhiz.com/LuaPlus/.
//
// The code presented in this file may be freely used and modified for all
// non-commercial and commercial purposes.
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LuaObject.h"

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

namespace LCD
{
	struct MiniFunctor;
}

/**
	A lightweight lua_State wrapper.
**/
class LUA_API LuaState
{
public:
	enum ErrorCodes
	{
		/* error codes for `lua_load' and `lua_pcall' */
		ERRRUN = 1,
		ERRFILE = 2,
		ERRSYNTAX = 3,
		ERRMEM = 4,
		ERRERR = 5,
	};

	enum Types
	{
		TNONE			= (-1),
		TNIL			= 0,
		TBOOLEAN		= 1,
		TLIGHTUSERDATA	= 2,
		TNUMBER			= 3,
		TSTRING			= 4,
		TTABLE			= 5,
		TFUNCTION		= 6,
		TUSERDATA		= 7,
		TTHREAD			= 8,
		TWSTRING		= 9,
		NTYPES			= 11,
	};

	enum DumpObjectTypes
	{
		DUMP_ALPHABETICAL		= 0x00000001,
		DUMP_WRITEALL			= 0x00000002,
		DUMP_WRITETABLEPOINTERS = 0x00000004,
	};


	///////////////////////////////////////////////////////////////////////////
	static LuaState* Create(bool initStandardLibrary = true, bool multithreaded = false);
	static LuaState* CreateThread(LuaState* parentState);
	static LuaState* CastState(lua_State* state);
	static void Destroy(LuaState* state);

	lua_CFunction AtPanic(lua_CFunction panicf);

	// LuaObject
	LuaObject GetGlobals() throw();
	LuaObject GetGlobal(const char *name);
	LuaObject GetRegistry();

	int Equal(const LuaObject& o1, const LuaObject& o2);
	int LessThan(const LuaObject& o1, const LuaObject& o2);

	// Stack functions.
	LuaStackObject Stack(int index)				{  return LuaStackObject(this, index);  }
	int GetTop();
	void SetTop(int index);
	void PushValue(int index);
	void PushValue(LuaStackObject object);
	void Remove(int index);
	void Insert(int index);
	void Replace(int index);
	int CheckStack(int size);
	void XMove(LuaState* to, int n);
	int Equal(int index1, int index2);
	int RawEqual(int index1, int index2);
	int LessThan(int index1, int index2);

	// push functions (C -> stack)
	LuaStackObject PushNil();
	LuaStackObject PushNumber(lua_Number n);
	LuaStackObject PushLString(const char *s, size_t len);
	LuaStackObject PushLWString(const wchar_t* s, size_t len);
	LuaStackObject PushString(const char *s);
	LuaStackObject PushWString(const wchar_t* s);
	const char *PushVFString(const char *fmt, va_list argp);
	LuaStackObject PushCClosure(lua_CFunction fn, int n);

	LuaStackObject PushCClosure(LuaStateCFunction function, int n)
	{
		return PushCClosure(LuaFunctor(function), n);
	}

	LuaStackObject PushCClosureEx(int (*f)(LuaState*, LCD::MiniFunctor&));

	template <class Callee>
	LuaStackObject PushCClosure(const Callee& callee, int (Callee::*f)(LuaState*), int n)
	{
		return PushCClosure(LuaFunctor(callee, f), n);
	}
	
	LuaStackObject PushCClosure(const LuaFunctor& functor, int n);
	LuaStackObject PushCFunction(lua_CFunction f);
	LuaStackObject PushBoolean(bool value);
	LuaStackObject PushLightUserData(void* p);

	// get functions (Lua -> stack)
	void GetTable(int index);
	void RawGet(int index);
	void RawGetI(int index, int n);
	LuaStackObject NewTable(int size = 0);
	void* NewUserData(size_t size);
	LuaStackObject GetMetaTable(int objindex);
	LuaStackObject GetDefaultMetaTable(int type);
	LuaStackObject GetGlobals_Stack();					// Backward compatible.
	LuaStackObject GetGlobal_Stack(const char *name);
	LuaStackObject GetRegistry_Stack();
	LuaStackObject GetRef(int ref);


	// set functions(stack -> Lua)
	void SetTable(int index);
	void RawSet(int index);
	void RawSetI(int index, int n);
	void SetMetaTable(int index);
	void SetDefaultMetaTable(int type);
	void SetGlobals(int index);
	void SetGlobal(const char *name);
	int Ref(int lock);
	void Unref(int ref);

	// `load' and `do' functions (load and run Lua code)
	void Call(int nargs, int nresults);
	int PCall(int nargs, int nresults, int errf);
	int CPCall(lua_CFunction func, void* ud);

	int Dump(lua_Chunkwriter writer, void* data);

	int LoadFile(const char* filename);
	int DoFile(const char *filename);
	int DoString(const char *str);
	int LoadBuffer(const char* buff, size_t size, const char* name);
	int DoBuffer(const char *buff, size_t size, const char *name);

	// Coroutine functions
	int CoYield(int nresults);
	int CoResume(int narg);

	// Garbage-collection functions
	int GetGCThreshold();
	int GetGCCount();
	void SetGCThreshold(int newthreshold);

	// Miscellaneous functions
	int Error();

	int Next(int index);
	int GetN(int index);

	void Concat(int n);

	LuaObject NewUserDataBox(void* u);
	LuaStackObject NewUserDataBox_Stack(void* u);

	// Helper functions
	void Pop();
	void Pop(int amount);

	void CollectGarbage();

	bool DumpObject(const char* filename, const char* name, LuaObject& value, unsigned int flags = DUMP_ALPHABETICAL,
					int indentLevel = 0, unsigned int maxIndentLevel = 0xffffffff);
	bool DumpObject(const char* filename, LuaObject& key, LuaObject& value, unsigned int flags = DUMP_ALPHABETICAL,
					int indentLevel = 0, unsigned int maxIndentLevel = 0xffffffff);

	bool DumpObject(LuaStateOutFile& file, const char* name, LuaObject& value, unsigned int flags = DUMP_ALPHABETICAL,
					int indentLevel = 0, unsigned int maxIndentLevel = 0xffffffff);
	bool DumpObject(LuaStateOutFile& file, LuaObject& key, LuaObject& value, unsigned int flags = DUMP_ALPHABETICAL,
					int indentLevel = 0, unsigned int maxIndentLevel = 0xffffffff);

	bool DumpGlobals(const char* filename, unsigned int flags = DUMP_ALPHABETICAL, unsigned int maxIndentLevel = 0xFFFFFFFF);
	bool DumpGlobals(LuaStateOutFile& file, unsigned int flags = DUMP_ALPHABETICAL, unsigned int maxIndentLevel = 0xFFFFFFFF);

	operator lua_State*()						{  return m_state;  }
	lua_State* GetCState()						{  return m_state;  }

	// Arg functions.
	int TypeError(int narg, const char* tname);
	int ArgError(int narg, const char* extramsg);
	const char* CheckLString(int numArg, size_t* len);
	const char* OptLString(int numArg, const char *def, size_t* len);
	lua_Number CheckNumber(int numArg);
	lua_Number OptNumber(int nArg, lua_Number def);
	void ArgCheck(bool condition, int numarg, const char* extramsg);
	const char* CheckString(int numArg);
	const char* OptString(int numArg, const char* def);
	int CheckInt(int numArg);
	long CheckLong(int numArg);
	int OptInt(int numArg, int def);
	long OptLong(int numArg, int def);
	void CheckStack(int sz, const char* msg);
	void CheckType(int narg, int t);
	void CheckAny(int narg);
	void* CheckUData(int ud, const char* tname);

	// Debug functions.
	int GetStack(int level, lua_Debug* ar);
	int GetInfo(const char* what, lua_Debug* ar);
	const char* GetLocal(const lua_Debug* ar, int n);
	const char* SetLocal(const lua_Debug* ar, int n);

	int SetHook(lua_Hook func, int mask, int count);
	lua_Hook GetHook();
	unsigned long GetHookMask();

	// Extra
	LuaStackObject BoxPointer(void* u);
	void* UnBoxPointer(int stackIndex);

	lua_State* m_state;
	bool m_ownState;

	// We want the storage space, but we can't have the constructor run.
	LuaObject* GetHeadObject()			{  return (LuaObject*)&m_headObject;  }
	LuaObject* GetTailObject()			{  return (LuaObject*)&m_tailObject;  }

protected:
	LuaState(bool initStandardLibrary = true, bool multithreaded = false);
	LuaState(LuaState* script, bool initStandardLibrary = true);
	LuaState(lua_State* state);
	~LuaState();
	LuaState& operator=(LuaState& src);		// Not implemented.

	void Init(bool initStandardLibrary);

	bool CallFormatting(LuaObject& tableObj, LuaStateOutFile& file, int indentLevel,
			bool writeAll, bool alphabetical, bool writeTablePointers,
			unsigned int maxIndentLevel);

	void SetupStateEx();

	struct MiniLuaObject
	{
		LuaObject* m_next;		   // only valid when in free list
		LuaObject* m_prev;		   // only valid when in used list
	};

	MiniLuaObject m_headObject;
	MiniLuaObject m_tailObject;
	LuaObject m_threadObj;
};


class LuaStateAuto
{
public:
    operator LuaState*()							{  return m_state;  }
    operator const LuaState*() const				{  return m_state;  }
    operator LuaState*() const						{  return m_state;  }
    LuaState& operator*()							{  return *m_state; }
    const LuaState& operator*() const				{  return *m_state; }
    LuaState* operator->()							{  return m_state;  }
    const LuaState* operator->() const				{  return m_state;  }
	LuaState* Get() const							{  return m_state;  }

	LuaStateAuto() : m_state(NULL) {}
    LuaStateAuto(LuaState* newState) : m_state(newState) {}
	LuaStateAuto& operator=(LuaState* newState)
	{
		Assign(newState);
		return *this;
	}

	~LuaStateAuto()
	{
		Assign(NULL);
	}

    LuaState* m_state;

protected:
    LuaStateAuto(const LuaStateAuto&);					// Not implemented.
    LuaStateAuto& operator=(const LuaStateAuto&);		// Not implemented.

	void Assign(LuaState* state)
	{
		if (m_state)
			LuaState::Destroy(m_state);
		m_state = state;
	}
};


class LuaStateOwner : public LuaStateAuto
{
public:
    LuaStateOwner(bool initStandardLibrary = true)
	{
		m_state = LuaState::Create(initStandardLibrary);
	}

    LuaStateOwner(LuaState* newState) : LuaStateAuto(newState) {}
	LuaStateOwner& operator=(LuaState* newState)
	{
		Assign(newState);
		return *this;
	}

	~LuaStateOwner()
	{
		Assign(NULL);
	}

private:
    LuaStateOwner(const LuaStateOwner&);				// Not implemented.
    LuaStateOwner& operator=(const LuaStateOwner&);		// Not implemented.
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class LuaStack
{
public:
	LuaStack(LuaState* state) : m_state(state) {}
	~LuaStack() {}

	LuaStackObject operator[](int index)
	{
		return LuaStackObject(m_state, index);
	}
 
protected:
	LuaState* m_state;
};
 

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct LuaLightUserData
{
	LuaLightUserData(const void* value) :
		m_value(value)
	{
	}

	const void* m_value;
};


struct LuaUserData
{
	LuaUserData(const void* value) :
		m_value(value)
	{
	}

	const void* m_value;
};



// Lua Call Dispatcher
namespace LCD
{
	inline void Push(LuaState* state, bool value)				{  lua_pushboolean(*state, value);  }
	inline void Push(LuaState* state, char value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, unsigned char value)		{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, short value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, unsigned short value)		{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, int value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, unsigned int value)		{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, long value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, unsigned long value)		{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, float value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, double value)				{  lua_pushnumber(*state, value);  }
	inline void Push(LuaState* state, const char* value)		{  lua_pushstring(*state, value);  }
	inline void Push(LuaState* state, const wchar_t* value)		{  lua_pushwstring(*state, value);  }
	inline void Push(LuaState* state, const LuaArgNil&)			{  lua_pushnil(*state);  }
	inline void Push(LuaState* state, lua_CFunction value)		{  lua_pushcclosure(*state, value, 0);  }
	inline void Push(LuaState* state, LuaStateCFunction value)	{  state->PushCClosure(value, 0);  }
	inline void Push(LuaState* state, LuaStackObject& value)	{  state; value.Push();  }
	inline void Push(LuaState* state, LuaObject& value)		{  state; value.PushStack();  }
	inline void Push(LuaState* state, const void* value)		{  lua_pushlightuserdata(*state, (void*)value);  }
	inline void Push(LuaState* state, const LuaLightUserData& value)	{  lua_pushlightuserdata(*state, (void*)value.m_value);  }
	inline void Push(LuaState* state, const LuaUserData& value){  state->NewUserDataBox((void*)value.m_value);  }

	template<class T> struct TypeWrapper {};

	inline bool	Match(TypeWrapper<bool>, const LuaStackObject& obj)
		{  return obj.IsBoolean();  }
	inline bool	Match(TypeWrapper<char>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<unsigned char>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<short>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<unsigned short>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<int>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<unsigned int>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<long>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<unsigned long>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<float>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<double>, const LuaStackObject& obj)
		{  return obj.IsNumber();  }
	inline bool	Match(TypeWrapper<const char*>, const LuaStackObject& obj)
		{  return obj.IsString();  }
	inline bool	Match(TypeWrapper<const wchar_t*>, const LuaStackObject& obj)
		{  return obj.IsWString();  }
	inline bool	Match(TypeWrapper<LuaStackObject>, const LuaStackObject&)
		{  return true;  }
	inline bool	Match(TypeWrapper<LuaObject>, const LuaStackObject&)
		{  return true;  }

	inline void				Get(TypeWrapper<void>, const LuaStackObject&)
		{  }
	inline bool				Get(TypeWrapper<bool>, const LuaStackObject& obj)
		{  return obj.GetBoolean();  }
	inline char				Get(TypeWrapper<char>, const LuaStackObject& obj)
		{  return static_cast<char>(obj.GetNumber());  }
	inline unsigned char	Get(TypeWrapper<unsigned char>, const LuaStackObject& obj)
		{  return static_cast<unsigned char>(obj.GetNumber());  }
	inline short			Get(TypeWrapper<short>, const LuaStackObject& obj)
		{  return static_cast<short>(obj.GetNumber());  }
	inline unsigned short	Get(TypeWrapper<unsigned short>, const LuaStackObject& obj)
		{  return static_cast<unsigned short>(obj.GetNumber());  }
	inline int				Get(TypeWrapper<int>, const LuaStackObject& obj)
		{  return static_cast<int>(obj.GetNumber());  }
	inline unsigned int		Get(TypeWrapper<unsigned int>, const LuaStackObject& obj)
		{  return static_cast<unsigned int>(obj.GetNumber());  }
	inline long				Get(TypeWrapper<long>, const LuaStackObject& obj)
		{  return static_cast<long>(obj.GetNumber());  }
	inline unsigned long	Get(TypeWrapper<unsigned long>, const LuaStackObject& obj)
		{  return static_cast<unsigned long>(obj.GetNumber());  }
	inline float			Get(TypeWrapper<float>, const LuaStackObject& obj)
		{  return static_cast<float>(obj.GetNumber());  }
	inline double			Get(TypeWrapper<double>, const LuaStackObject& obj)
		{  return static_cast<double>(obj.GetNumber());  }
	inline const char*		Get(TypeWrapper<const char*>, const LuaStackObject& obj)
		{  return static_cast<const char*>(obj.GetString());  }
	inline const wchar_t*	Get(TypeWrapper<const wchar_t*>, const LuaStackObject& obj)
		{  return static_cast<const wchar_t*>(obj.GetWString());  }
	inline LuaArgNil		Get(TypeWrapper<LuaArgNil>, const LuaStackObject& obj)
		{  obj;  return LuaArgNil();  }
	inline lua_CFunction	Get(TypeWrapper<lua_CFunction>, const LuaStackObject& obj)
		{  return static_cast<lua_CFunction>(obj.GetCFunction());  }
	inline LuaStackObject	Get(TypeWrapper<LuaStackObject>, const LuaStackObject& obj)
		{  return obj;  }
	inline LuaObject		Get(TypeWrapper<LuaObject>, const LuaStackObject& obj)
		{  return obj;  }

	//////////////////////////////////////////////////////////////////////////
	
	#define luaL_argassert(arg) if (!Match(TypeWrapper<P##arg>(), LuaStackObject(state, arg))) \
				luaL_argerror(*state, arg, "bad argument")

	template<class RT>
	struct ReturnSpecialization
	{
		static int Call(RT (*f)(), LuaState* state)
		{
			RT ret = f();
			Push(state, ret);
			return 1;
		}


		template <typename P1>
		static int Call(RT (*f)(P1), LuaState* state)
		{
			luaL_argassert(1);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2>
		static int Call(RT (*f)(P1, P2), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(RT (*f)(P1, P2, P3), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2))
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(RT (*f)(P1, P2, P3, P4), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5>
		static int Call(RT (*f)(P1, P2, P3, P4, P5), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6>
		static int Call(RT (*f)(P1, P2, P3, P4, P5, P6), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6))
			);
			Push(state, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6, typename P7>
		static int Call(RT (*f)(P1, P2, P3, P4, P5, P6, P7), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);
			luaL_argassert(7);

			RT ret = f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6)),
				Get(TypeWrapper<P7>(), LuaStackObject(state, 7))
			);
			Push(state, ret);
			return 1;
		}


		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, RT (Callee::*func)(), LuaState* state)
		{
			RT ret = (callee.*func)();
			Push(state, ret);
			return 1;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, RT (Callee::*func)(P1), LuaState* state)
		{
			luaL_argassert(1);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1))
			);
			Push(state, ret);
			return 1;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2))
			);
			Push(state, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3))
			);
			Push(state, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4))
			);
			Push(state, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5))
			);
			Push(state, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6))
			);
			Push(state, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);
			luaL_argassert(7);

			RT ret = (callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6)),
				Get(TypeWrapper<P7>(), LuaStackObject(state, 7))
			);
			Push(state, ret);
			return 1;
		}
	};


	template<>
	struct ReturnSpecialization<void>
	{
		static int Call(void (*f)(), LuaState* state)
		{
			state;
			f();
			return 0;
		}


		template <typename P1>
		static int Call(void (*f)(P1), LuaState* state)
		{
			luaL_argassert(1);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1))
			);
			return 0;
		}


		template <typename P1, typename P2>
		static int Call(void (*f)(P1, P2), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2))
			);
			return 0;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(void (*f)(P1, P2, P3), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(void (*f)(P1, P2, P3, P4), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(void (*f)(P1, P2, P3, P4, P5), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(void (*f)(P1, P2, P3, P4, P5, P6), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(void (*f)(P1, P2, P3, P4, P5, P6, P7), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);
			luaL_argassert(7);

			f(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6)),
				Get(TypeWrapper<P7>(), LuaStackObject(state, 7))
			);
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, void (Callee::*func)(), LuaState* state)
		{
			(callee.*func)();
			return 0;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, void (Callee::*func)(P1), LuaState* state)
		{
			luaL_argassert(1);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1))
			);
			return 0;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6))
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7), LuaState* state)
		{
			luaL_argassert(1);
			luaL_argassert(2);
			luaL_argassert(3);
			luaL_argassert(4);
			luaL_argassert(5);
			luaL_argassert(6);
			luaL_argassert(7);

			(callee.*func)(
				Get(TypeWrapper<P1>(), LuaStackObject(state, 1)),
				Get(TypeWrapper<P2>(), LuaStackObject(state, 2)),
				Get(TypeWrapper<P3>(), LuaStackObject(state, 3)),
				Get(TypeWrapper<P4>(), LuaStackObject(state, 4)),
				Get(TypeWrapper<P5>(), LuaStackObject(state, 5)),
				Get(TypeWrapper<P6>(), LuaStackObject(state, 6)),
				Get(TypeWrapper<P7>(), LuaStackObject(state, 7))
			);
			return 0;
		}
	};


	struct MiniFunctor
	{
		void *callee;
		enum { MEM_FUNC_SIZE = 16 };
		char func[MEM_FUNC_SIZE];
	};


	template <typename RT>
	int Call(RT (*f)(), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(f, state);
	}


	template <typename RT, typename P1>
	int Call(RT (*f)(P1), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(f, state);
	}


	template <typename RT, typename P1, typename P2>
	int Call(RT (*f)(P1, P2), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(f, state);
	}


	template <typename RT, typename P1, typename P2, typename P3>
	int Call(RT (*f)(P1, P2, P3), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(f, state);
	}


	template <typename Callee, typename RT>
	int Call(Callee& callee, RT (Callee::*f)(), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1>
	int Call(Callee& callee, RT (Callee::*f)(P1), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2, P3), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2, P3, P4), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2, P3, P4, P5), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2, P3, P4, P5, P6), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6, typename P7>
	int Call(Callee& callee, RT (Callee::*f)(P1, P2, P3, P4, P5, P6, P7), LuaState* state)
	{
		return ReturnSpecialization<RT>::Call(callee, f, state);
	}


	template <typename F>
	inline int FunctionDispatcher(LuaState* state, LCD::MiniFunctor& miniFunctor)
	{
		return Call(*(F*)(&miniFunctor.func), state);
	}


	template <typename Callee, typename F>
	inline int MemberDispatcher(LuaState* state, LCD::MiniFunctor& miniFunctor)
	{
		return Call(*(Callee*)miniFunctor.callee, *(F*)(&miniFunctor.func), state);
	}
} // namespace LCD

#define LUAFUNCTION_PRECALL() \
		LuaState* state = m_functionObj; \
		LuaAutoBlock autoBlock(state); \
		m_functionObj.PushStack();

#define LUAFUNCTION_POSTCALL(numArgs) \
		if (state->PCall(numArgs, 1, 0)) \
		{ \
			const char* errorString = lua_tostring(*state, -1); \
			assert(0); \
			throw errorString; \
		} \
		return LCD::Get(LCD::TypeWrapper<RT>(), LuaStackObject(state, -1));


/**
**/
template <typename RT = void>
class LuaFunction
{
public:
	LuaFunction(LuaObject& functionObj) :
		m_functionObj(functionObj)
	{
		assert(m_functionObj.IsFunction());
	}

	LuaFunction(LuaState* state, const char* functionName)
	{
		m_functionObj = state->GetGlobals()[functionName];
		assert(m_functionObj.IsFunction());
	}

	RT operator()()
	{
		LUAFUNCTION_PRECALL();
		LUAFUNCTION_POSTCALL(0);
	}

	template <typename P1>
	RT operator()(P1 p1)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LUAFUNCTION_POSTCALL(1);
	}

	template <typename P1, typename P2>
	RT operator()(P1 p1, P2 p2)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LUAFUNCTION_POSTCALL(2);
	}

	template <typename P1, typename P2, typename P3>
	RT operator()(P1 p1, P2 p2, P3 p3)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LCD::Push(state, p3);
		LUAFUNCTION_POSTCALL(3);
	}

	template <typename P1, typename P2, typename P3, typename P4>
	RT operator()(P1 p1, P2 p2, P3 p3, P4 p4)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LCD::Push(state, p3);
		LCD::Push(state, p4);
		LUAFUNCTION_POSTCALL(4);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5>
	RT operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LCD::Push(state, p3);
		LCD::Push(state, p4);
		LCD::Push(state, p5);
		LUAFUNCTION_POSTCALL(5);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	RT operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LCD::Push(state, p3);
		LCD::Push(state, p4);
		LCD::Push(state, p5);
		LCD::Push(state, p6);
		LUAFUNCTION_POSTCALL(6);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	RT operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		LUAFUNCTION_PRECALL();
		LCD::Push(state, p1);
		LCD::Push(state, p2);
		LCD::Push(state, p3);
		LCD::Push(state, p4);
		LCD::Push(state, p5);
		LCD::Push(state, p6);
		LCD::Push(state, p7);
		LUAFUNCTION_POSTCALL(7);
	}

protected:
	LuaObject m_functionObj;
};

} // namespace LuaPlus

#ifdef LUAPLUS_ENABLE_INLINES
#include "LuaState.inl"
#endif LUAPLUS_ENABLE_INLINES

