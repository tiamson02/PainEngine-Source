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

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

class LuaCall;
struct LuaArgNil {};

struct LuaRun
{
	LuaRun(int numResults = -1) throw()
	{
		m_numResults = numResults;
	}

	int m_numResults;
};

/**
	Representation of a Lua object.
**/
class LUA_API LuaObject
{
public:
	LuaObject() throw() :
		m_state(NULL),
		m_next(NULL),
		m_prev(NULL)
	{
		// Bad to have this barebones constructor... but useful for containers.
	}

	LuaObject(LuaState* state) throw();
	LuaObject(LuaState* state, int stackIndex) throw();
	LuaObject(LuaState* state, const lua_TObject* obj) throw();
	LuaObject(const LuaObject& src) throw();
	LuaObject(const LuaStackObject& src) throw();
	LuaObject& operator=(const LuaObject& src) throw();
	LuaObject& operator=(const LuaStackObject& src) throw();

	template <typename T>
	LuaObject& operator=(const T& value)
	{
		assert(m_state);
		LCD::Assign(*this, value);
		return *this;
	}

	~LuaObject();

	void Reset();

	/**
		Retrieves the LuaState object associated with this LuaObject.
	**/
	LuaState* GetState() const;
	lua_State* GetCState() const;
	operator lua_State*() const;
	operator LuaState*() const;

	bool operator==(const LuaObject& right) const;
	bool operator<(const LuaObject& right) const;

	const char* TypeName() const;
	int Type() const;

	bool IsNil() const;
	bool IsTable() const;
	bool IsUserData() const;
	bool IsCFunction() const;
	bool IsNumber() const;
	bool IsString() const;
	bool IsWString() const;
	bool IsConvertibleToNumber() const;
	bool IsConvertibleToString() const;
	bool IsConvertibleToWString() const;
	bool IsFunction() const;
	bool IsNone() const;
	bool IsLightUserData() const;
	bool IsBoolean() const;

	lua_Number ToNumber();
	const char* ToString();
	const wchar_t* ToWString();
	size_t ToStrLen();

	int GetInteger() const;
	float GetFloat() const;
	double GetDouble() const;
	lua_Number GetNumber() const;
	const char* GetString() const;
	const wchar_t* GetWString() const;
	int StrLen();
	lua_CFunction GetCFunction() const;
	void* GetUserData();
	const void* GetLuaPointer();
	void* GetLightUserData() const;
	bool GetBoolean() const;

	LuaStackObject PushStack();

	LuaStackObject __cdecl PCall(const char* argTypes, ...);
	LuaStackObject __cdecl PCall(int nresults, const char* argTypes, ...);

	LuaObject GetMetaTable();
	void SetMetaTable(LuaObject& valueObj);

	int GetN();
	void SetN(int n);

	void Insert(LuaObject& obj);
	void Insert(int index, LuaObject& obj);
	void Remove(int index = -1);
	void Sort();

	int GetCount();
	int GetTableCount();

	LuaObject Clone();

	LuaObject CreateTable(const char* key, int narray = 0, int lnhash = 0);
	LuaObject CreateTable(int key, int narray = 0, int lnhash = 0);
	LuaObject CreateTable(LuaObject& key, int narray = 0, int lnhash = 0);

	LuaObject& SetNil(const char* key);
	LuaObject& SetNil(int key);
	LuaObject& SetNil(LuaObject& key);
	LuaObject& SetBoolean(const char* key, bool value);
	LuaObject& SetBoolean(int key, bool value);
	LuaObject& SetBoolean(LuaObject& key, bool value);
	LuaObject& SetNumber(const char* key, lua_Number value);
	LuaObject& SetNumber(int key, lua_Number value);
	LuaObject& SetNumber(LuaObject& key, lua_Number value);
	LuaObject& SetString(const char* key, const char* value);
	LuaObject& SetString(int key, const char* value);
	LuaObject& SetString(LuaObject& key, const char* value);
	LuaObject& SetWString(const char* key, const wchar_t* value);
	LuaObject& SetWString(int key, const wchar_t* value);
	LuaObject& SetWString(LuaObject& key, const wchar_t* value);
	LuaObject& SetUserData(const char* key, void* value);
	LuaObject& SetUserData(int key, void* value);
	LuaObject& SetUserData(LuaObject& key, void* value);
	LuaObject& SetLightUserData(const char* key, void* value);
	LuaObject& SetLightUserData(int key, void* value);
	LuaObject& SetLightUserData(LuaObject& key, void* value);
	LuaObject& SetObject(const char* key, LuaObject& value);
	LuaObject& SetObject(int key, LuaObject& value);
	LuaObject& SetObject(LuaObject& key, LuaObject& value);

	void AssignNil(LuaState* state);
	void AssignBoolean(LuaState* state, bool value);
	void AssignNumber(LuaState* state, lua_Number value);
	void AssignString(LuaState* state, const char* value);
	void AssignWString(LuaState* state, const wchar_t* value);
	void AssignUserData(LuaState* state, void* value);
	void AssignLightUserData(LuaState* state, void* value);
	void AssignObject(LuaObject& value);		// Should this function be removed??
	void AssignNewTable(LuaState* state, int narray = 0, int numhash = 0);
	void AssignTObject(LuaState* state, lua_TObject* value);

	LuaObject GetByName(const char* name);
	LuaObject GetByIndex(int index);
	LuaObject GetByObject(const LuaStackObject& obj);
	LuaObject GetByObject(const LuaObject& obj);
	LuaObject operator[](const char* name);
	LuaObject operator[](int index);
	LuaObject operator[](const LuaStackObject& obj);
	LuaObject operator[](const LuaObject& obj);

	LuaObject Lookup(const char* key);

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

	template <typename F>
	void RegisterEx(const char* name, F f)
	{
		PushStack();
		m_state->PushString(name);
		m_state->PushCClosureEx(LCD::FunctionDispatcher<F>);
		LCD::MiniFunctor* mlc = (LCD::MiniFunctor*)lua_getcclosure_userdata(m_state->m_state, -1);
		memcpy(&mlc->func, &f, sizeof(F));
		m_state->SetTable(-3);
		m_state->Pop();
	}


	template <typename Callee, typename F>
	void RegisterEx(const char* name, Callee& obj, F f)
	{
		PushStack();
		m_state->PushString(name);
		m_state->PushCClosureEx(LCD::MemberDispatcher<Callee, F>);
		LCD::MiniFunctor* mlc = (LCD::MiniFunctor*)lua_getcclosure_userdata(m_state->m_state, -1);
		mlc->callee = &obj;
		memcpy(&mlc->func, &f, sizeof(F));
		m_state->SetTable(-3);
		m_state->Pop();
	}

	void Unregister(const char* funcName);
	void Unregister(const LuaFunctorList* functionList);

	LuaCall operator()();
	LuaStackObject PCall(int nresults, const char* argTypes, va_list args);

	// Private functions and data
	void AddToUsedList(LuaState* state);
	void AddToUsedList(LuaState* state, const lua_TObject& obj);
	void RemoveFromUsedList();

	LuaObject* m_next;		   // only valid when in free list
	LuaObject* m_prev;		   // only valid when in used list
	LuaState* m_state;
#ifdef BUILDING_LUAPLUS
	lua_TObject m_object;
#else BUILDING_LUAPLUS
	unsigned char m_object[12];
#endif BUILDING_LUAPLUS

protected:
	void Register(const char* funcName, const LuaFunctor& functor);
	LuaObject& SetTableHelper(const char* key, lua_TObject& valueObj);
	LuaObject& SetTableHelper(int key, lua_TObject& valueObj);
	LuaObject& SetTableHelper(LuaObject& key, lua_TObject& valueObj);
};


class LuaCall
{
public:
	LuaCall(LuaObject& functionObj);

	LuaCall& operator<<(const LuaArgNil& /*value*/);
	LuaCall& operator<<(float value);
	LuaCall& operator<<(double value);
	LuaCall& operator<<(int value);
	LuaCall& operator<<(const char* value);
	LuaCall& operator<<(const wchar_t* value);
	LuaCall& operator<<(lua_CFunction value);
	LuaCall& operator<<(LuaStateCFunction value);
	LuaCall& operator<<(bool value);
	LuaCall& operator<<(void* value);
	LuaCall& operator<<(LuaStackObject& obj);
	LuaCall& operator<<(LuaObject& obj);
	LuaStackObject operator<<(const LuaRun& /*run*/);
	LuaCall& operator=(const LuaCall& src);

protected:
	LuaState* m_state;
	LuaObject m_functionObj;
	int m_numArgs;
	int m_startResults;
};


} // namespace LuaPlus

#include "LuaState.h"

