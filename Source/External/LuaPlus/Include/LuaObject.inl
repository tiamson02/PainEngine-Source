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
**/
inline bool LuaObject::operator==(const LuaObject& right) const
{
	assert(m_state);
	return m_state->Equal(*this, right) != 0;
}

	
/**
**/
inline bool LuaObject::operator<(const LuaObject& right) const
{
	assert(m_state);
	return m_state->LessThan(*this, right) != 0;
}

	
/**
**/
inline LuaState* LuaObject::GetState() const
{
	return m_state;
}


inline lua_State* LuaObject::GetCState() const
{
	return m_state->m_state;
}


inline LuaObject::operator lua_State*() const
{
	return m_state->m_state;
}


inline LuaObject::operator LuaState*() const
{
	return m_state;
}


inline LuaCall LuaObject::operator()()
{
	m_state->GetGlobal_Stack("_ALERT");
	return LuaCall(*this);
}


inline LuaCall::LuaCall(LuaObject& functionObj) :
	m_functionObj(functionObj),
	m_numArgs(0)
{
	assert(m_functionObj.IsFunction());
	m_state = m_functionObj,
	m_startResults = m_state->GetTop() + 1;
	m_functionObj.PushStack();
}

inline LuaCall& LuaCall::operator<<(const LuaArgNil& /*value*/)
{
	m_state->PushNil();
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(float value)
{
	m_state->PushNumber(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(double value)
{
	m_state->PushNumber(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(int value)
{
	m_state->PushNumber(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(const char* value)
{
	m_state->PushString(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(const wchar_t* value)
{
	m_state->PushWString(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(lua_CFunction value)
{
	m_state->PushCClosure(value, 0);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(LuaStateCFunction value)
{
	m_state->PushCClosure(value, 0);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(bool value)
{
	m_state->PushBoolean(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(void* value)
{
	m_state->PushLightUserData(value);
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(LuaStackObject& obj)
{
	obj.Push();
	m_numArgs++;
	return *this;
}

inline LuaCall& LuaCall::operator<<(LuaObject& obj)
{
	obj.PushStack();
	m_numArgs++;
	return *this;
}

inline LuaStackObject LuaCall::operator<<(const LuaRun& run)
{
	int alertStackPos = m_state->GetTop() - m_numArgs - 1;
	int err = m_state->PCall(m_numArgs, run.m_numResults, alertStackPos);	err;
	if (err != 0)
	{
		LuaStackObject errObj(m_state, -1);
		assert(err == 0);
	}
	else
	{
		m_state->Remove(alertStackPos);		// Pop _ALERT.
	}
	return LuaStackObject(m_state, m_startResults - 1);
}

inline LuaCall& LuaCall::operator=(const LuaCall& src)
{
	m_state = src.m_state;
	m_functionObj = src.m_functionObj;
	m_numArgs = src.m_numArgs;
	m_startResults = src.m_startResults;

	return *this;
}


} // namespace LuaPlus

