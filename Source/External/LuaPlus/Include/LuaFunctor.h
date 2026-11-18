#ifndef LUAFUNCTOR_H
#define LUAFUNCTOR_H

#include <memory.h>

namespace LuaPlus {

class LuaState;
class LuaStackObject;

#include <pshpack1.h>

class LuaFunctor
{
public:
	LuaFunctor() :
		m_callee(NULL)
	{
		memset(m_func, 0, sizeof(m_func));
	}

	LuaFunctor(int (*f)(LuaState*)) :
		m_thunk((Thunk)&FunctionTranslator<int (*)(LuaState*)>::thunk),
		m_callee(NULL)
	{
		memset(m_func, 0, MEM_FUNC_SIZE);
		memcpy(m_func, &f, sizeof(f));
	}

    template <typename Callee>
	LuaFunctor(const Callee& c, int (Callee::*f)(LuaState*)) :
		m_thunk((Thunk)&MemberTranslator<Callee, int (Callee::*)(LuaState*)>::thunk),
		m_callee((void*)&c)
	{
		memset(m_func, 0, MEM_FUNC_SIZE);
		memcpy(m_func, &f, sizeof(f));
	}

	operator bool()const				{  return m_callee || *(void (*)())&m_func;  }

	int operator()(LuaState* state) const
	{  return m_thunk(*this, state);  }

private:
	template <class Func>
	class FunctionTranslator
	{
	public:
		static int thunk(const LuaFunctor& functor, LuaState* state)
		{
			Func& func = *(Func*)functor.m_func;
			return func(state);
		}
	};

	template <class Callee, class MemFunc>
	class MemberTranslator
	{
	public:
		static int thunk(const LuaFunctor& functor, LuaState* state)
		{
			Callee& callee = *(Callee*)functor.m_callee;
			MemFunc& func = *(MemFunc*)functor.m_func;
			return (callee.*func)(state);
		}
	};

	typedef int (*Thunk)(const LuaFunctor&, LuaState* state);
	Thunk m_thunk;

public:
	// MEM_FUNC_SIZE must be equal to the largest member function size the system
	// can create.  For virtual inheritance on MSVC, it is 16 bytes.
	void *m_callee;
	enum { MEM_FUNC_SIZE = 16 };
	char m_func[MEM_FUNC_SIZE];
};

#include <poppack.h>

} // namespace LuaPlus

#endif LUAFUNCTOR_H
