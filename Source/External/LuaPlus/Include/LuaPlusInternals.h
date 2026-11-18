#pragma once

namespace LuaPlus {

struct CClosureEx
{
	lu_byte f1[sizeof(LuaFunctor)]; // LSFunctor f1;
};

} // namespace LuaPlus

