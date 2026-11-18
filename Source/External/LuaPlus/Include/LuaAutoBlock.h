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
	A helper class for automatically setting the stack state back to point of
	LuaAutoBlock creation.
**/
class LuaAutoBlock
{
public:
	LuaAutoBlock(LuaState* state) :
		m_state(state),
		m_stackTop(m_state->GetTop())
	{
	}

	LuaAutoBlock(LuaStackObject& object) :
		m_state(object),
		m_stackTop(m_state->GetTop())
	{
	}

	~LuaAutoBlock()
	{
		m_state->SetTop(m_stackTop);
	}

private:
	LuaAutoBlock(const LuaAutoBlock& src);					// Not implemented
	const LuaAutoBlock& operator=(const LuaAutoBlock& src);	// Not implemented

	LuaState* m_state;
	int m_stackTop;
};


} // namespace LuaPlus


