/*
** $Id: lmem.c,v 1.70 2005/12/26 13:35:47 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lmem_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



// XXX - Sokol
//#define LUA_MEMORY_TEST
//#define LUA_DLMEMORY
#ifdef TRANSGAMING // =======================================================
# if defined(DEBUG) && defined(MEMORY_TEST) && !defined(MEMORY_NOLUA)

#  define LUA_USE_MEMORY_TRACKER

/*extern const	unsigned int	m_alloc_realloc;
extern const	unsigned int	m_alloc_free;
//extern void	*m_reallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
//					const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress);
extern void *m_scriptReallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, 
									const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress);
extern void	m_deallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
							const unsigned int deallocationType, const void *reportedAddress);
//extern void mmgr_setTag(const char *tag);

#  define l_realloc(ptr,os,sz)                          m_scriptReallocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_realloc, sz, ptr)  //m_reallocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_realloc,sz,ptr)        //(mmgr_setTag("Script"), m_reallocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_realloc,sz,ptr))
#  define l_free(ptr,os)			                    m_deallocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_free,ptr)
#  define l_reallocDBG(file, line, func, ptr, os, sz)   m_scriptReallocator(file, line, func, m_alloc_realloc, sz, ptr)
#  define l_freeDBG(file, line, func, ptr, os)          m_deallocator(file,line,func,m_alloc_free,ptr)//*/

# else // defined(DEBUG) || defined(MEMORY_TEST)

/*#  define l_realloc(ptr,os,sz) realloc(ptr , sz )
#  define l_free(ptr,os) free(ptr)//*/

# endif // defined(DEBUG) || defined(MEMORY_TEST)

#else // TRANSGAMING ============================
// TRANSGAMING - WE SHOULDN'T EVER BE USING THIS!!!!!!!!!!!!!!
# error WE SHOULD NOT BE USING THIS ON PS2 or PSP!!
# define USE_DL_PREFIX
# include "../LMalloc.h"
# define l_realloc(b,os,s)	dlrealloc(b,s)
# define l_free(b,os)		dlfree(b)
#endif // TRANSGAMING ===============================================
/// XXX - Sokol



/*
** About the realloc function:
** void * frealloc (void *ud, void *ptr, size_t osize, size_t nsize);
** (`osize' is the old size, `nsize' is the new size)
**
** Lua ensures that (ptr == NULL) iff (osize == 0).
**
** * frealloc(ud, NULL, 0, x) creates a new block of size `x'
**
** * frealloc(ud, p, x, 0) frees the block `p'
** (in this specific case, frealloc must return NULL).
** particularly, frealloc(ud, NULL, 0, 0) does nothing
** (which is equivalent to free(NULL) in ANSI C)
**
** frealloc returns NULL if it cannot create or reallocate the area
** (any reallocation to an equal or smaller size cannot fail!)
*/



#define MINSIZEARRAY	4

#ifdef LUA_USE_MEMORY_TRACKER
void *luaM_growaux_ (lua_State *L, void *block, int *size, size_t size_elems,
                     int limit, const char *errormsg,
                     const char *file, int line, const char *func) {
#else
void *luaM_growaux_ (lua_State *L, void *block, int *size, size_t size_elems,
                     int limit, const char *errormsg) {
#endif
  void *newblock;
  int newsize;
  if (*size >= limit/2) {  /* cannot double it? */
    if (*size >= limit)  /* cannot grow even a little? */
      luaG_runerror(L, errormsg);
    newsize = limit;  /* still have at least one free place */
  }
  else {
    newsize = (*size)*2;
    if (newsize < MINSIZEARRAY)
      newsize = MINSIZEARRAY;  /* minimum size */
  }
#ifdef LUA_USE_MEMORY_TRACKER
  newblock = luaM_reallocv(L, block, *size, newsize, size_elems,
                           file, line, func);
#else
  newblock = luaM_reallocv(L, block, *size, newsize, size_elems);
#endif
  *size = newsize;  /* update only when everything else is OK */
  return newblock;
}


void *luaM_toobig (lua_State *L) {
  luaG_runerror(L, "memory allocation error: block too big");
  return NULL;  /* to avoid warnings */
}



/*
** generic allocation routine.
*/
#ifdef LUA_USE_MEMORY_TRACKER
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize,
                     const char *file, int line, const char *func) {
#else
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize) {
#endif
  global_State *g = G(L);
  lua_assert((osize == 0) == (block == NULL));
#ifdef LUA_USE_MEMORY_TRACKER
  block = (*g->frealloc)(g->ud, block, osize, nsize, file, line, func);
#else
  block = (*g->frealloc)(g->ud, block, osize, nsize);
#endif
  if (block == NULL && nsize > 0)
    luaD_throw(L, LUA_ERRMEM);
  lua_assert((nsize == 0) == (block == NULL));
  g->totalbytes = (g->totalbytes - osize) + nsize;
  return block;
}

