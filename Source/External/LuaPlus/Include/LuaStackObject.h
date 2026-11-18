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

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/**
	Representation of a Lua object residing on the Lua stack.
**/
class LUA_API LuaStackObject
{
public:
	/**
	**/
	LuaStackObject() :
		m_state(NULL)
	{
		// Bad to have this... but useful for containers.
	}

	/**
		Various constructors accepting different parameters.
	**/
	LuaStackObject(LuaState* state, int stackIndex);

	/**
		Various constructors accepting different parameters.
	**/
	LuaStackObject(LuaState& state, int stackIndex);

	/**
		Copy constructor.
	**/
	LuaStackObject(const LuaStackObject& src) :
		m_state(src.m_state),
		m_stackIndex(src.m_stackIndex)
	{
	}

	/**
		Assignment operator.
	**/
	const LuaStackObject& operator=(const LuaStackObject& src)
	{
		m_state = src.m_state;
		m_stackIndex = src.m_stackIndex;
		return *this;
	}

	/**
		Retrieves the LuaState object associated with this LuaStackObject.
	**/
	LuaState* GetState() const;
	lua_State* GetCState() const;
	operator lua_State*() const;
	operator LuaState*() const;
	operator int() const				{  return m_stackIndex;  }

	bool operator==(const LuaStackObject& right) const;

	const char* GetTypeName() const;
	int GetType() const;

	bool IsNil() const;
	bool IsTable() const;
	bool IsUserData() const;
	bool IsCFunction() const;
	bool IsNumber() const;
	bool IsString() const;
	bool IsWString() const;
	bool IsFunction() const;
	bool IsNone() const;
	bool IsLightUserData() const;
	bool IsBoolean() const;

	int GetInteger() const;
	float GetFloat() const;
	double GetDouble() const;
	lua_Number GetNumber() const;
	const char* GetString() const;
	const wchar_t* GetWString() const;
	int StrLen() const;
	lua_CFunction GetCFunction() const;
	void* GetUserData() const;
	const void* GetLuaPointer() const;
	void* GetLightUserData() const;
	bool GetBoolean() const;

	void Push();
	void Pop();

	LuaStackObject __cdecl PCall(const char* argTypes, ...);
	LuaStackObject __cdecl PCall(int nresults, const char* argTypes, ...);

	int Ref(int lock = 1);

	LuaStackObject GetMetaTable();
	void SetMetaTable();
	void SetMetaTable(LuaStackObject value);

	void SetTable();
	int GetCount();

	LuaStackObject CreateTable(const char* name, int narray = 0, int lnhash = 0);
	LuaStackObject CreateTable(int index, int narray = 0, int lnhash = 0);

	void SetNil(const char* name);
	void SetNil(int index);
	void SetBoolean(const char* name, bool value);
	void SetBoolean(int index, bool value);
	void SetNumber(const char* name, lua_Number value);
	void SetNumber(int index, lua_Number value);
	void SetString(const char* name, const char* value);
	void SetString(int index, const char* value);
	void SetWString(const char* name, const wchar_t* value);
	void SetWString(int index, const wchar_t* value);
	void SetUserData(const char* name, void* value);
	void SetUserData(int index, void* value);
	void SetLightUserData(const char* name, void* value);
	void SetObject(const char* name, LuaStackObject value);
	void SetObject(int index, LuaStackObject value);

	LuaStackObject GetByName(const char* name);
	LuaStackObject GetByIndex(int index);
	LuaStackObject GetByObject(LuaStackObject obj);

	LuaStackObject operator[](const char* name) const;
	LuaStackObject operator[](int index) const;
	LuaStackObject operator[](LuaStackObject obj) const;

	void Register(const char* funcName, lua_CFunction function);

	void Register(const char* funcName, LuaStateCFunction function)
	{
		Register(funcName, LuaFunctor(function));
	}

	template <class Callee>
	void Register(const char* funcName, const Callee& callee, int (Callee::*f)(LuaState*))
	{
		Register(funcName, LuaFunctor(callee, f));
	}
	
	void Register(const LuaFunctorList* functionList);
	void Unregister(const char* funcName);
	void Unregister(const LuaFunctorList* functionList);

//protected:
	friend class LuaState;

	void Register(const char* funcName, const LuaFunctor& functor);
	LuaStackObject PCall(int nresults, const char* argTypes, va_list args);

	LuaState* m_state;		//!< The parent state of this object.
	int m_stackIndex;		//!< The stack index representing this object.
};


/**
	Representation of a Lua object residing on the Lua stack.
**/
class LUA_API LuaAutoObject : public LuaStackObject
{
public:
	/**
		Various constructors accepting different parameters.
	**/
//	LuaAutoObject(lua_State* state, int stackIndex) : LuaStackObject(state, stackIndex) {}

	/**
		Various constructors accepting different parameters.
	**/
	LuaAutoObject(LuaState* state, int stackIndex) : LuaStackObject(state, stackIndex) {}

	/**
		Various constructors accepting different parameters.
	**/
	LuaAutoObject(LuaState& state, int stackIndex) : LuaStackObject(state, stackIndex) {}

	/**
		Copy constructor.
	**/
	LuaAutoObject(const LuaStackObject& src) : LuaStackObject(src)
	{
		// No destruction necessary.
	}

	/**
		Assignment operator.
	**/
	const LuaAutoObject& operator=(const LuaStackObject& src);

	/**
		Assignment operator.
	**/
	const LuaAutoObject& operator=(const LuaAutoObject& src)
	{
		m_state = src.m_state;
		m_stackIndex = src.m_stackIndex;
		return *this;
	}

	~LuaAutoObject();
}; // LuaAutoObject


/**
**/
class LuaRefObject : public LuaStackObject
{
public:
	LuaRefObject();
	LuaRefObject(LuaStackObject& srcObj);
	LuaRefObject& operator=(LuaStackObject& srcObj);
	~LuaRefObject();
};


} // namespace LuaPlus


#ifdef LUAPLUS_ENABLE_INLINES
#include "LuaState.h"
#include "LuaStackObject.inl"
#endif LUAPLUS_ENABLE_INLINES
