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

/**
	Constructor.
**/
LUAPLUS_INLINE LuaStackObject::LuaStackObject( LuaState* state, int stackIndex ) :
	m_state( state ),
	m_stackIndex( stackIndex )
{
}


/**
	Constructor.
**/
LUAPLUS_INLINE LuaStackObject::LuaStackObject( LuaState& state, int stackIndex ) :
	m_state( &state ),
	m_stackIndex( stackIndex )
{
}

	
/**
**/
LUAPLUS_INLINE bool LuaStackObject::operator==(const LuaStackObject& right) const
{
	return m_state->Equal((int)*this, (int)right) != 0;
}

	
/**
**/
LUAPLUS_INLINE LuaState* LuaStackObject::GetState() const
{
	return m_state;
}


LUAPLUS_INLINE lua_State* LuaStackObject::GetCState() const			{  return m_state->m_state;  }
LUAPLUS_INLINE LuaStackObject::operator lua_State*() const			{  return m_state->m_state;  }
LUAPLUS_INLINE LuaStackObject::operator LuaState*() const			{  return m_state;  }

/**
	Retrieves the type name.  Corresponds to the information presented in section
	4.5 of the Lua manual.
**/
LUAPLUS_INLINE const char* LuaStackObject::GetTypeName() const
{
	return lua_typename( m_state->m_state, m_stackIndex );
}


/**
	Returns one of the LUA_T* constants, based on the type.  Corresponds to section
	4.5 of the Lua manual.

	\return Returns one of the following constants: LUA_TNIL, LUA_TNUMBER,
		LUA_TSTRING, LUA_TBOOLEAN, LUA_TTABLE, LUA_TFUNCTION, LUA_TUSERDATA,
		LUA_TLIGHTUSERDATA, LUA_TWSTRING
**/
LUAPLUS_INLINE int LuaStackObject::GetType() const
{
	return lua_type( m_state->m_state, m_stackIndex );
}


/**
	\return Returns true if the object is nil.
**/
LUAPLUS_INLINE bool LuaStackObject::IsNil() const					{  return lua_isnil( m_state->m_state, m_stackIndex );  }


/**
	\return Returns true if the object is a table.
**/
LUAPLUS_INLINE bool LuaStackObject::IsTable() const					{  return lua_istable( m_state->m_state, m_stackIndex );  }


/**
	\return Returns true if the object is user data, including light user data.
**/
LUAPLUS_INLINE bool LuaStackObject::IsUserData() const				{  return lua_isuserdata( m_state->m_state, m_stackIndex ) != 0;  }


/**
	\return Returns true if the object is a C function callback.
**/
LUAPLUS_INLINE bool LuaStackObject::IsCFunction() const				{  return lua_iscfunction( m_state->m_state, m_stackIndex ) != 0;  }


/**
	\return Returns true if the object is a number.
**/
LUAPLUS_INLINE bool LuaStackObject::IsNumber() const					{  return lua_type( m_state->m_state, m_stackIndex ) == LUA_TNUMBER;  }


/**
	\return Returns true if the object is a string.
**/
LUAPLUS_INLINE bool LuaStackObject::IsString() const					{  return lua_isstring( m_state->m_state, m_stackIndex ) != 0;  }


/**
	\return Returns true if the object is a wide character string.
**/
LUAPLUS_INLINE bool LuaStackObject::IsWString() const				{  return lua_iswstring( m_state->m_state, m_stackIndex ) != 0;  }


/**
	\return Returns true if the object is a Lua function.
**/
LUAPLUS_INLINE bool LuaStackObject::IsFunction() const				{  return lua_isfunction( m_state->m_state, m_stackIndex );  }


/**
	\return Returns true if the object is none.
**/
LUAPLUS_INLINE bool LuaStackObject::IsNone() const					{  return !m_state  ||  lua_isnone( m_state->m_state, m_stackIndex );  }


/**
	\return Returns true if the object is light user data (that is, the object
		is just a pointer.
**/
LUAPLUS_INLINE bool LuaStackObject::IsLightUserData() const			{  return lua_islightuserdata( m_state->m_state, m_stackIndex ) != 0;  }


/**
	\return Returns true if the object is a boolean.
**/
LUAPLUS_INLINE bool LuaStackObject::IsBoolean() const				{  return lua_isboolean( m_state->m_state, m_stackIndex );  }


/**
**/
LUAPLUS_INLINE int LuaStackObject::GetInteger() const				{  return (int)lua_tonumber( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE float LuaStackObject::GetFloat() const				{  return (float)lua_tonumber( m_state->m_state, m_stackIndex);  }
LUAPLUS_INLINE double LuaStackObject::GetDouble() const				{  return (double)lua_tonumber( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE lua_Number LuaStackObject::GetNumber() const				{  return lua_tonumber( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE const char* LuaStackObject::GetString() const
{
	const char* str = lua_tostring( m_state->m_state, m_stackIndex );
	assert(str);
	return str;
}
LUAPLUS_INLINE const wchar_t* LuaStackObject::GetWString() const
{
	const wchar_t* str = (const wchar_t*)lua_towstring( m_state->m_state, m_stackIndex );
	assert(str);
	return str;
}
LUAPLUS_INLINE int LuaStackObject::StrLen() const					{  return (int)lua_strlen( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE lua_CFunction LuaStackObject::GetCFunction() const	{  assert( lua_iscfunction( m_state->m_state, m_stackIndex ) );  return lua_tocfunction( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE void* LuaStackObject::GetUserData() const				{  assert( lua_isuserdata( m_state->m_state, m_stackIndex ) );  return lua_touserdata( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE const void* LuaStackObject::GetLuaPointer() const		{  return lua_topointer( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE void* LuaStackObject::GetLightUserData() const		{  assert( lua_islightuserdata( m_state->m_state, m_stackIndex ));  return (void*)lua_touserdata( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE bool LuaStackObject::GetBoolean() const				{  assert( lua_isboolean( m_state->m_state, m_stackIndex )  ||  lua_isnil( m_state->m_state, m_stackIndex ) );  return (int)lua_toboolean( m_state->m_state, m_stackIndex ) != 0;  }

LUAPLUS_INLINE void LuaStackObject::Push()							{  lua_pushvalue( m_state->m_state, m_stackIndex );  }
// This is a dangerous function, as it can affect the stack placement of other LuaObjects.
LUAPLUS_INLINE void LuaStackObject::Pop()							{  lua_remove( m_state->m_state, m_stackIndex );  }

LUAPLUS_INLINE int LuaStackObject::Ref( int lock )
{
	Push();
	return m_state->Ref( lock );
}

LUAPLUS_INLINE LuaStackObject LuaStackObject::GetMetaTable()
{
	lua_getmetatable( m_state->m_state, m_stackIndex );
	return LuaStackObject( GetState(),
		lua_gettop( m_state->m_state ) );
}

LUAPLUS_INLINE void LuaStackObject::SetMetaTable()					{  lua_setmetatable( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE void LuaStackObject::SetMetaTable( LuaStackObject value )
{
	value.Push();
	lua_setmetatable( m_state->m_state, m_stackIndex );
}

LUAPLUS_INLINE void LuaStackObject::SetTable()						{  lua_settable( m_state->m_state, m_stackIndex );  }
LUAPLUS_INLINE int LuaStackObject::GetCount()						{  return lua_getn( m_state->m_state, m_stackIndex );  }


/**
	Creates a table called [name] within the current LuaStackObject.

	@param name The name of the table to create.
	@param size The size of the table.
	@return Returns the object representing the newly created table.
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::CreateTable( const char* name, int narray, int lnhash )
{
	narray, lnhash;

//jj		lua_newtablesize(m_state, narray, lnhash);			// T
	lua_newtable( m_state->m_state );								// T
	lua_pushstring( m_state->m_state, name );						// T name
	lua_pushvalue( m_state->m_state, lua_gettop( m_state->m_state ) - 1 );	// T name T
	lua_settable( m_state->m_state, m_stackIndex );

	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
	Creates a table called [key] within the current LuaStackObject.

	@param index The index of the table to create.
	@param size The size of the table.
	@return Returns the object representing the newly created table.
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::CreateTable(int index, int narray, int lnhash )
{
	narray, lnhash;

//jj		lua_newtablesize(m_state, narray, lnhash);			// T
	lua_newtable( m_state->m_state );								// T
	lua_pushnumber( m_state->m_state, index );						// T name
	lua_pushvalue( m_state->m_state, lua_gettop( m_state->m_state ) - 1 );	// T name T
	lua_settable( m_state->m_state, m_stackIndex );

	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
	Assigns the table key [name] to nil.

	@param name The name of the object to make nil.
**/
LUAPLUS_INLINE void LuaStackObject::SetNil( const char* name )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushnil( m_state->m_state );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Assigns the table key [index] to nil.

	@param key The index of the object to make nil.
**/
LUAPLUS_INLINE void LuaStackObject::SetNil( int index )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushnil( m_state->m_state );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetBoolean( const char* name, bool value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushboolean( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetBoolean( int index, bool value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushboolean( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetNumber( const char* name, lua_Number value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushnumber( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetNumber( int index, lua_Number value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushnumber( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetString( const char* name, const char* value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushstring( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetString( int index, const char* value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushstring( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetWString( const char* name, const wchar_t* value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushwstring( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetWString( int index, const wchar_t* value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushwstring( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetUserData( const char* name, void* value )
{
	lua_pushstring( m_state->m_state, name );
	lua_newuserdatabox( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetUserData( int index, void* value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_newuserdatabox( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetLightUserData( const char* name, void* value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushlightuserdata( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [name] to [value].

	@param name The name of the object to assign the value to.
	@param value The value to assign to [name].
**/
LUAPLUS_INLINE void LuaStackObject::SetObject( const char* name, LuaStackObject value )
{
	lua_pushstring( m_state->m_state, name );
	lua_pushvalue( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Creates (or reassigns) the object called [index] to [value].

	@param index The index of the object to assign the value to.
	@param value The value to assign to [index].
**/
LUAPLUS_INLINE void LuaStackObject::SetObject( int index, LuaStackObject value )
{
	lua_pushnumber( m_state->m_state, index );
	lua_pushvalue( m_state->m_state, value );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Assuming the current object is a table, retrieves the table entry
	called [name].

	@param name The name of the entry from the current table to retrieve.
	@return Returns an LuaStackObject representing the retrieved entry.
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::GetByName( const char* name )
{
	lua_pushstring( m_state->m_state, name );
	lua_gettable( m_state->m_state, m_stackIndex );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
	Assuming the current object is a table, retrieves the table entry
	at [index].

	@param index The numeric name of a table entry.
	@return Returns an LuaStackObject representing the retrieved entry.
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::GetByIndex( int index )
{
	lua_rawgeti( m_state->m_state, m_stackIndex, index );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
	Assuming the current object is a table, retrieves the table entry
	called [name].

	@param name The name of the entry from the current table to retrieve.
	@return Returns an LuaStackObject representing the retrieved entry.
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::GetByObject( LuaStackObject obj )
{
	lua_pushvalue( m_state->m_state, obj );
	lua_rawget( m_state->m_state, m_stackIndex );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::operator[]( const char* name ) const
{
	lua_pushstring( m_state->m_state, name );
	lua_rawget( m_state->m_state, m_stackIndex );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::operator[]( int index ) const
{
	lua_rawgeti( m_state->m_state, m_stackIndex, index );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
**/
LUAPLUS_INLINE LuaStackObject LuaStackObject::operator[]( LuaStackObject obj ) const
{
	lua_pushvalue( m_state->m_state, obj );
	lua_rawget( m_state->m_state, m_stackIndex );
	return LuaStackObject( GetState(), lua_gettop( m_state->m_state ) );
}

/**
	Assuming the current object is a table, registers a C function called 
	[funcName] within the table.

	@param funcName The name of the function to register.
	@param function A pointer to the C function to register.
**/
LUAPLUS_INLINE void LuaStackObject::Register( const char* funcName, lua_CFunction function )
{
	lua_pushstring( m_state->m_state, funcName );
	lua_pushcclosure( m_state->m_state, (lua_CFunction)function, 0 );
	lua_settable( m_state->m_state, m_stackIndex );
}

LUA_API void lua_pushcfunctorclosure (lua_State *L, const LuaFunctor& fn, int n);

/**
	Assuming the current object is a table, registers a C function called 
	[funcName] within the table.

	@param funcName The name of the function to register.
	@param function A pointer to the C function to register.
**/
LUAPLUS_INLINE void LuaStackObject::Register( const char* funcName, const LuaFunctor& functor )
{
	lua_pushstring( m_state->m_state, funcName );
	lua_pushcfunctorclosure( m_state->m_state, functor, 0 );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Assuming the current object is a table, registers all of the functions
	in [functionList] with the table.

	@param functionList A NULL terminated list of functions to register in
		the current table object.
**/
LUAPLUS_INLINE void LuaStackObject::Register( const LuaFunctorList* functionList )
{
	int i = 0;
	while ( true )
	{
		if ( !functionList[ i ].name )
			break;
		Register( functionList[ i ].name, functionList[ i ].luaFunctor );
		i++;
	}
}

/**
	Assuming the current object is a table, unregisters the function called
	[funcName].

	@param funcName The name of the function to unregister.
**/
LUAPLUS_INLINE void LuaStackObject::Unregister( const char* funcName )
{
	lua_pushstring( m_state->m_state, funcName );
	lua_pushnil( m_state->m_state );
	lua_settable( m_state->m_state, m_stackIndex );
}

/**
	Assuming the current object is a table, unregisters the list of
	functions described in [functionList].

	@param functionList The list of functions to unregister.
**/
LUAPLUS_INLINE void LuaStackObject::Unregister( const LuaFunctorList* functionList )
{
	int i = 0;
	while ( true )
	{
		if (!functionList[ i ].name)
			break;
		Unregister( functionList[i].name );
		i++;
	}
}

LUAPLUS_INLINE LuaRefObject::LuaRefObject()
{
	m_stackIndex = -40000;				// Invalid
}


LUAPLUS_INLINE LuaRefObject::LuaRefObject(LuaStackObject& srcObj)
{
	*this = srcObj;
}


LUAPLUS_INLINE LuaRefObject& LuaRefObject::operator=(LuaStackObject& srcObj)
{
	m_state = srcObj.m_state;
	srcObj.Push();
	m_stackIndex = -40000 - m_state->Ref(1);
	return *this;
}


LUAPLUS_INLINE LuaRefObject::~LuaRefObject()
{
	if (m_stackIndex != -40000)
		m_state->Unref(-m_stackIndex - 40000);
}


LUAPLUS_INLINE const LuaAutoObject& LuaAutoObject::operator=( const LuaStackObject& src )
{
	lua_remove( m_state->m_state, m_stackIndex );

	m_state = src.m_state;
	m_stackIndex = src.m_stackIndex;
	return *this;
}

LUAPLUS_INLINE LuaAutoObject::~LuaAutoObject()
{
	lua_remove( m_state->m_state, m_stackIndex );
}


} // namespace LuaPlus
