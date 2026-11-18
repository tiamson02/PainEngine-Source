// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/06 Bartek Sokolowski
//----------------------------------------------------------------------------
#ifndef SCRIPTING_H_
#define SCRIPTING_H_

#include "trace.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
	#include "lobject.h"

    LUA_API void  lua_setconstant (lua_State *L) ;
	LUA_API int lua_getn (lua_State *L, int index);
}

#ifdef LUA_VERSION_NUM // Lua 5.1
#define TObject TValue
#define lua_TObject lua_TValue
#define lua_dostring(L,s) (luaL_loadstring(L,s) || lua_pcall(L, 0, 0, 0))
#define lua_dobuffer(L,b,s,n) (luaL_loadbuffer(L,b,s,n) || lua_pcall(L, 0, 0, 0))
#endif

//----------------------------------------------------------------------------
class ScriptObject
{
protected:
	lua_State *mLua;
	TObject mObject;
	
	TObject *mParent;
	char *mSKey;
	int mIKey;
private:
	void UpdateValue();
public:
	ScriptObject(lua_State *pLua,const TObject *obj = NULL);
	~ScriptObject();

	// variables TYPE
	int         GetType() const;
	bool        IsString()	const;
	bool        IsNumber()	const;
	bool        IsBool()	const;
	bool        IsFunction() const;
	bool        IsTable() const;
	bool        IsLightUD() const;
	bool        IsNil()  const;

	// variables GET
	float       GetFloat() const;
	int         GetInt() const;
	const char* GetString() const;
	bool        GetBool() const;
	void*       GetLightUD() const;

	// variables ASSIGN
	void        SetNil();
	void        SetInteger(int val);
	void        SetFloat(float val);
	void        SetBool(bool val);
	void        SetTObject(lua_TObject* val);
	void        SetString(const char* val);

	// table
	ScriptObject &  NewKey(const char * key);
	ScriptObject &  NewKey(int key);
	int             GetCount() const;
	ScriptObject    GetMetatable() const;

	// table element access
	ScriptObject operator[] (const char* key);
	ScriptObject operator[] (int key);

	friend class ScriptTableIterator;
	friend bool ScriptTable_next(lua_State* state, ScriptObject* table, ScriptObject* key, ScriptObject* value);
};

//----------------------------------------------------------------------------
class Script
{
protected:
	lua_State *mLua;
	bool mToKill;
	int mIndex;

public:
	Script();
	Script(lua_State *pLua,int index=0);
    
	~Script();

	// initialization / shutdown
	bool        Init();
	void        Release();
	bool        Initialized() {return mLua?true:false;};

	// load/execute lua scrips
	bool        DoFile(const char *file,bool must_exist = true);
	void        DoString(const char *str, ...);
	void        DoBuffer(const char *str, const char *bufname);
    void        DoStrings(int count, const char **strings);
    void        DumpError(int error, const char *file, const char *func) const;

	// sctipt message callback
	void        QueueMessage(const char* msg_name,const char* params_format,...);

	// register function/library
	void        RegisterFunction(const char *name,lua_CFunction function);
	void        RegisterLibrary(const char *name,const luaL_reg *l,bool constant = true);

	// variables TYPE
	int	        GetType(int idx=-1) {return lua_type(mLua,idx);}
	bool        IsString()	  {return lua_isstring(mLua, mIndex)==1?true:false;}
	bool        IsNumber()	  {return lua_isnumber(mLua, mIndex)==1?true:false;}
	bool        IsFunction() {return lua_isfunction(mLua, mIndex);}
	bool        IsTable()	  {return lua_istable(mLua, mIndex);}
	bool        IsNil(int idx=-1)      {return lua_isnoneornil(mLua,idx);}

	// variables GET
	const char *GetString(int idx=-1,const char* optional=NULL);
	int         GetInt(int idx=-1,int optional = 0);
	float       GetFloat(int idx=-1,float optional=0.0f);
	bool        GetBool(int idx=-1,bool optional=false);
	void *      GetLightUD(int idx=-1,void*optional=NULL);
	Script      GetTable(int idx=-1);
	int         GetMetatable(int idx=-1);

	// variables PUSH
	void        PushString(const char *s);
	void        PushFloat(float number);
	void        PushInt(int number);
	void        PushBool(bool val) {lua_pushboolean(mLua,val);}
	void        PushNil() {lua_pushnil(mLua);}
	void        PushLightUD(void *p) {lua_pushlightuserdata(mLua,p);}

	void        SetHook(lua_Hook func, unsigned long mask);
	void        GetInfo(const char *args, lua_Debug *ar);

	// function call
	void        Call(int nargs, int nresults);
	void        Call(const char* func_name,int nresults,const char* params_format,...);
    void        Call(Script &obj, const char *func_name, int nresults, const char *fmt, ...);

	// stack
	int         GetTop();
	void        SetTop(int index);

    int         GetIndex() const{ return mIndex; }

	// garbage
#ifdef LUA_VERSION_NUM
	// Lua 5.1
	int         GetGCCount()		              {return lua_gc(mLua, LUA_GCCOUNT, 0);}
	int         GetGCThreshold()			   	  {return 0;}
	void        SetGCThreshold(int newthreshold) {}
#else
	// Lua 5.0
	int         GetGCCount()		              {return lua_getgccount(mLua);}
	int         GetGCThreshold()			   	  {return lua_getgcthreshold(mLua);}
	void        SetGCThreshold(int newthreshold) {lua_setgcthreshold(mLua,newthreshold);}
#endif

	// global table
	ScriptObject Globals();

	// table
	int         NewTable() ;
	void        SetTable(int idx);
	int         GetCount(int idx=-1);

	// table element access
	Script      operator[] (const char* key)
	{ 
		lua_pushstring(mLua, key);
		lua_rawget(mLua, mIndex);
		return Script(mLua, GetTop());
	}
	Script      operator[] (int idx)
	{ 
		lua_rawgeti(mLua, mIndex,idx);
		return Script(mLua, GetTop());
	}
};

//----------------------------------------------------------------------------
// table iterator
bool ScriptTable_next(lua_State* state, ScriptObject* table, ScriptObject* key, ScriptObject* value);

class ScriptTableIterator{	
public:
	ScriptTableIterator( ScriptObject& tableObj, bool doReset = true ) :
	      m_isDone(false),
		  m_tableObj(tableObj),
		  m_keyObj(tableObj.mLua),
		  m_valueObj(tableObj.mLua)
	  {
		  Assert(tableObj.IsTable());
		  if ( doReset )
			  Reset();
	  }

	  /**
	  Start iteration at the beginning of the table.
	  **/
	  void Reset()
	  {
		  m_keyObj.SetNil();

		  if ( !ScriptTable_next(m_keyObj.mLua, &m_tableObj, &m_keyObj, &m_valueObj) )
			  m_isDone = true;
	  }

	  /**
	  Invalidates the iterator.  Call this function if you early abort from
	  an iteration loop (such as before it hits the end).
	  **/
	  void Invalidate()
	  {
		  m_keyObj.SetNil();
		  m_valueObj.SetNil();
	  }

	  bool Next()
	  {
		  // This function is only active if Reset() has been called head.
		  Assert( IsValid() );
		  if ( !ScriptTable_next(m_tableObj.mLua, &m_tableObj, &m_keyObj, &m_valueObj) )
		  {
			  m_isDone = true;
			  return false;
		  }
		  return true;
	  }

	  bool IsValid() const
	  {
		  return !m_isDone;
	  }

	  ScriptTableIterator& operator++()
	  {
		  Next();
		  return *this;
	  }

	  inline operator bool() const
	  {
		  return IsValid();
	  }

	  inline ScriptObject& GetKey()
	  {
		  Assert( IsValid() );
		  return m_keyObj;
	  }

	  inline ScriptObject& GetValue()
	  {
		  Assert( IsValid() );
		  return m_valueObj;
	  }

private:
	ScriptTableIterator& operator=( const ScriptTableIterator& iter );
	ScriptTableIterator( const ScriptTableIterator& iter );

	ScriptObject& m_tableObj;
	ScriptObject m_keyObj;
	ScriptObject m_valueObj;
	bool m_isDone;
};

//----------------------------------------------------------------------------
#endif //SCRIPTING_H_
