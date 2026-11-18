// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/06 Bartek Sokolowski
//----------------------------------------------------------------------------
#include "Scripting.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entity.h"
#include "engineToLua.h"

extern "C"
{
    #include "lstate.h"
    #include "ltable.h"
    #include "lobject.h"
    void luaA_pushobject (lua_State *L, const TObject *o);
    Table* luaT_getmetatable(lua_State *L, const TObject *o);
    int luaH_index (lua_State *L, Table *t, StkId key);
    TString *luaS_newlstr (lua_State *L, const char *str, size_t l);
    void luaV_settable (lua_State *L, const TObject *t, TObject *key, StkId val);
    LUA_API void  lua_pushnumber (lua_State *L, lua_Number n);
#ifdef LUA_VERSION_NUM
#define lua_setobj(L,o1,o2) setobj(L,o1,o2)
#define lua_setsvalue(L,o,x) setsvalue(L,o,x)
#define lua_setsvalue2n(L,o,x) setsvalue2n(L,o,x)
#define lua_gkey(n) key2tval(n)
#define lua_gval(n) gval(n)
#define lu_hash unsigned int
#define gcotots(o) rawgco2ts(o)
#else
#define lua_setobj(L,o1,o2) setobj(o1,o2)
#define lua_setsvalue(L,o,x) setsvalue(o,x)
#define lua_setsvalue2n(L,o,x) setsvalue2n(o,x)
#define lua_gkey(n) gkey(n)
#define lua_gval(n) gval(n)
#endif
}





//----------------------------------------------------------------------------
Script::Script()
:mLua(NULL), mIndex(0), mToKill(false)
{

}
//----------------------------------------------------------------------------
Script::Script(lua_State *state,int index)
:mLua(state), mIndex(index), mToKill(false)
{
    //mTop = lua_gettop(state);
}
//----------------------------------------------------------------------------
Script::~Script()
{
    Release();
    //if (mLua && mTop>-1)
    //  lua_settop(mLua,mTop);
}
//----------------------------------------------------------------------------
static int script_panic (lua_State *L) {
    lua_Debug db;
    memset(&db, 0, sizeof(db));
    TRACE("!!!! ******************************************************************************************* !!!!\n");

    if (lua_getstack (L,0,&db) && lua_getinfo(L, "Sln", &db))
    {       
        int         i = 1;

        TRACE("!!!! script_panic(): FATAL-> script runtime error!\n");

        do{ 
            TRACE("  %s (%d) in function %s (%d) (%s)\n", db.source, db.currentline, db.name ? db.name : "?", db.linedefined, db.namewhat);
            i++;
        }
        while (lua_getstack (L,i,&db) && lua_getinfo(L, "Sln", &db));

    }
    else{
        TRACE("!!!! script_panic(): FATAL-> unknown script error!\n");
    }

    TRACE("!!!! ******************************************************************************************* !!!!\n\n");
    return 0;
} 

void lua_mywhere (lua_State *L, int level) {
    lua_Debug ar;
    if (lua_getstack(L, level, &ar)) {  /* check function at level */
        lua_getinfo(L, "Snl", &ar);  /* get info about it */
        if (ar.currentline > 0) {  /* is there info? */
            lua_pushfstring(L, "%s:%d - %s", ar.short_src, ar.currentline, ar.name?ar.name:"?");
            return;
        }
    }
    lua_pushliteral(L, "");  /* else, no information available... */
}

static int luaGetCallStackInfo(lua_State *state) 
{
    Script L(state);
    lua_mywhere (state, L.GetInt(1)) ;
    return 1;
}
//----------------------------------------------------------------------------
bool Script::Init(){
    mLua = lua_open();
    if (!mLua) return false;
    //lua_setgcthreshold (mLua,500);

#ifdef LUA_VERSION_NUM

    // adaptation of linit.c

    static const luaL_Reg lualibs[] = {
        {"", luaopen_base},
        //{LUA_LOADLIBNAME, luaopen_package},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_IOLIBNAME, luaopen_io},
        {LUA_OSLIBNAME, luaopen_os},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
# ifndef FINAL_BUILD
        {LUA_DBLIBNAME, luaopen_debug},
# endif
        {NULL, NULL}
    };

    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++) {
        lua_pushcfunction(mLua, lib->func);
        lua_pushstring(mLua, lib->name);
        lua_call(mLua, 1, 0);
    }

#else

    //"coroutine"
    lua_baselibopen (mLua);
    //"table"
    lua_tablibopen (mLua);  
    //"io" "os"
    lua_iolibopen (mLua);
    //"string"
    lua_strlibopen (mLua);
    //"math"
    lua_mathlibopen (mLua);
    //"debug"
# ifndef FINAL_BUILD
    lua_dblibopen (mLua); 
# endif

#endif

    RegisterFunction("GetCallStackInfo", luaGetCallStackInfo);       
    lua_atpanic (mLua, script_panic); 
    //mLua->l_G->panic = &script_panic;

    engine2Lua(*this);
    
    mToKill = true;
    return true;
}
//----------------------------------------------------------------------------
void Script::Release()
{
    if (!mToKill) return;

    if (mLua)
        lua_close(mLua);
    mLua = NULL;
}
//----------------------------------------------------------------------------

void Script::DumpError(int error, const char *file, const char *func) const{
    if (error == 0)
        return;

    TRACE("\n!!!! ******************************************************************************************* !!!!\n");


    switch (error){
        case LUA_ERRRUN:
            TRACE("!!!! %s: ERROR-> error running the script '%s': '%s'\n", func, file, lua_tostring(mLua, -1));

            break;

        case LUA_ERRSYNTAX:
            TRACE("!!!! %s: ERROR-> syntax error in the file '%s': '%s'\n", func, file, lua_tostring(mLua, -1));
            
            break;

        case LUA_ERRMEM:
            TRACE("!!!! %s: ERROR-> could not allocate memory while compiling '%s': '%s'\n", func, file, lua_tostring(mLua, -1));
            
            break;

        case LUA_ERRERR:
            TRACE("!!!! %s: ERROR-> error handler error while compiling '%s': '%s'\n", func, file, lua_tostring(mLua, -1));
            
            break;

        case LUA_ERRFILE:
            TRACE("!!!! %s: ERROR-> could not open the file '%s': '%s'\n", func, file, lua_tostring(mLua, -1));
            
            break;

        case 0:
            return;
        default:
            TRACE("!!!! %s: ERROR-> unknown lua error compiling '%s' (%d): '%s'\n", func, file, error, lua_tostring(mLua, -1));
            
            break;
    }


    //attempt to dump the script call stack
    lua_Debug db;
    memset(&db, 0, sizeof(db));

    if (lua_getstack (mLua,0,&db) && lua_getinfo(mLua, "Sln", &db))
    {       
        int         i = 1;


        TRACE("!!!! %s(): attempted stack trace:\n", func);
    
        do{ 
            TRACE("!!!!    %s (%d) in function %s (%d) (%s)\n", db.source, db.currentline, db.name ? db.name : "?", db.linedefined, db.namewhat);
            i++;
        }
        while (lua_getstack (mLua,i,&db) && lua_getinfo(mLua, "Sln", &db));

    }
    else
        TRACE("!!!! %s(): ERROR-> unknown script location!\n", func);

    TRACE("!!!! ******************************************************************************************* !!!!\n\n");
}


void Script::DoString(const char *str,...)
{
    if (!mLua) return;

    const int BUFSIZE = 512;
    char gBuf[BUFSIZE];

    va_list args;
    va_start(args, str);
    
    if (_vsnprintf(gBuf, BUFSIZE, str, args) < 0){
        TRACE("!!!! Script::DoString(): ERROR-> buffer overflow!  Script command '%s' was truncated\n", str);
        va_end(args);
    
        return;
    }

    va_end(args);



    int t = GetTop();
    int error = lua_dostring(mLua, gBuf);

    if (error != 0)
        DumpError(error, gBuf, "Script::DoString()");

    SetTop(t);
}


char getLastNonWhitespaceCharacter(const char *str){
    size_t len = strlen(str);


    for (size_t i = len - 1; i >= 0; i--){
        if (str[i] != ' ' && str[i] != '\t')
            return str[i];
    }

    return str[0];
}

void Script::DoStrings(int count, const char **strings){
    static const int    BUFSIZE = 1024;
    static char         buf[BUFSIZE];
    int                 pos;
    int                 error;
    int                 i = 0;
    const char *        toExec;


    //continue to fill the buffer until all commands have been executed
    while (i < count){
        pos = 0;
     

        //this command is larger than the buffer => just execute it immediately
        if (strlen(strings[i]) > BUFSIZE){
            toExec = strings[i];
            i++; 
        }

        //the first command will fit in the buffer => queue up as many as possible into the same buffer and execute it
        else{

            for (; i < count; i++){

                //ignore null or empty commands
                if (strings[i] == NULL || strings[i][0] == 0)
                    continue;

                //Lua can't handle two semicolons in a row (it considers it an error) => check the
                //end of the string to see if it already contains one, then append a new one only
                //if necessary.

                //the end of this command doesn't contain a semicolon => add one to seperate the commands
                if (getLastNonWhitespaceCharacter(strings[i]) != ';')
                    error = snprintf(&buf[pos], BUFSIZE - pos, "%s; ", strings[i]);

                //the end of this command already contains a semicolon => just append the next command to it
                else
                    error = snprintf(&buf[pos], BUFSIZE - pos, "%s ", strings[i]);

                if (error < 0){
                    //TRACE("Script::DoStrings(): ERROR-> buffer overflow on command '%s'\n", strings[i]);
                    buf[pos] = 0;

                    break;
                }

                pos += error;
            }

            toExec = buf;
        }


        //execute the string
        int t = GetTop();
        error = lua_dostring(mLua, toExec);

        if (error != 0)
            DumpError(error, toExec, "Script::DoStrings()");

        SetTop(t);
    }
}


//----------------------------------------------------------------------------
bool Script::DoFile(const char *file,bool must_exist){
    if (mLua == NULL)
        return false;

    FILE *  f = fopen(file, "rb");


    if (!f){
        if (must_exist){
            TRACE("Script::DoFile(): ERROR-> file '%s' not found!\n", file);

            return false;
        }
        else
            return false;
    }
    else 
    {
        const int   BUFSIZE = 150000;
        char        buf[BUFSIZE + 1];
        size_t      l = fread(buf, 1, BUFSIZE, f);


        if (l == BUFSIZE || l <= 0)
            TRACE("[]::Script::DoFile(): ERROR-> buffer overflow when loading file '%s'!\n", file);

        fclose(f);
        buf[l] = 0;
        buf[BUFSIZE] = 0;


        char filename[_MAX_PATH];


        //copy this string since this function can recurse and the <file> parameter may 
        //be the same buffer each time
        strncpy(filename, file, _MAX_PATH);

        TRACE("Script::DoFile(): loading lua script '%s'\n", filename);
        int error = lua_dobuffer(mLua, buf, l, filename);
        TRACE("Script::DoFile(): done loading lua script '%s' (error = %d)\n", filename, error);

        if (error != 0)
            DumpError(error, filename, "Script::DoFile()");
    }
    return true;
}

void Script::DoBuffer(const char *buf, const char *bufname){
    if (mLua == NULL)
        return;

    int error = lua_dobuffer(mLua, buf, strlen(buf), bufname);

    if (error != 0)
        DumpError(error, bufname, "Script::DoBuffer()");
}
//----------------------------------------------------------------------------
void Script::RegisterFunction(const char *name, lua_CFunction function)
{
    if (!mLua) return;
    lua_register(mLua, name, function); 
}
//----------------------------------------------------------------------------
void Script::RegisterLibrary(const char *name,const luaL_reg *l,bool constant)
{
    if (!mLua) return;
    luaL_openlib(mLua, name, l, 0); 
    if (constant)
        lua_setconstant(mLua);
}
//----------------------------------------------------------------------------
const char * Script::GetString(int idx,const char* optional)
{
    return luaL_optlstring(mLua,idx,optional,NULL);
}
//----------------------------------------------------------------------------
int Script::GetInt(int idx, int optional)
{
    return (int) luaL_optnumber(mLua,idx, (lua_Number)optional);
}
//----------------------------------------------------------------------------
bool Script::GetBool(int idx,bool optional)
{
  if (lua_isnoneornil(mLua, idx))
      return optional;
  else 
      return lua_toboolean(mLua, idx)?true:false;
}
//----------------------------------------------------------------------------
float Script::GetFloat(int idx,float optional)
{
    return (float) luaL_optnumber(mLua,idx,optional);;
}
//----------------------------------------------------------------------------
void *Script::GetLightUD(int idx,void*optional)
{
    if (lua_isnoneornil(mLua, idx))
        return optional;
    else 
        return lua_touserdata(mLua,idx);
}
//----------------------------------------------------------------------------
Script Script::GetTable(int idx) 
{
    lua_topointer(mLua,idx);
    return Script(mLua, GetTop());
};
//----------------------------------------------------------------------------
int Script::GetMetatable(int idx)      
{
    return lua_getmetatable(mLua,idx);
};
//----------------------------------------------------------------------------
void Script::Call(int nargs, int nresults)          
{  
    //lua_rawget(mLua,idx);
    lua_call(mLua, nargs, nresults);  
} 
//----------------------------------------------------------------------------    
void Script::Call(const char* func_name,int nresults,const char* params_format,...){
    int count = 0;
    int top = lua_gettop(mLua);


    //this script object references a stack index => grab the function pointer from the table identified by that index
    if (mIndex){
        //make sure there's a table on top of the stack
        Assert(lua_istable(mLua, mIndex));

        //push the function name onto the stack
        lua_pushstring(mLua, func_name);
        Assert(lua_isstring(mLua, -1));

        //access the context table to grab the function pointer
        lua_gettable(mLua, mIndex);

        //make sure the stack has the function on top of the stack
        Assert(lua_isfunction(mLua, -1));

        //push the context table on after the function
        lua_pushvalue(mLua, mIndex);
        Assert(lua_istable(mLua, -1));
        
        //update the argument count to include the context table
        count++;
    }

    //this script object doesn't reference anything => grab the function pointer from the global table
    else{
        lua_pushstring(mLua, func_name);
        lua_gettable(mLua, LUA_GLOBALSINDEX);

        Assert(lua_isfunction(mLua, -1));
    }


    // eric-> make sure a params list was passed in before attempting to parse non-existent args.
    //   Yet another potential error that wasn't checked for.
    if (params_format){
        va_list args;
        va_start(args,params_format);

        // eric-> change this unfriendliness here... allow the parser to seperate tokens by spaces as
        //   well as commas.  This won't affect any other behaviour since the parser doesn't handle 
        //   non-printf() style formatting tokens at all (ie: the string tokens 'edible', 'idiot', and
        //   'addition' will each parse as an arg expecting an int)
        //const char * type = strchr(pbuf, ", \t");
        bool done = false;


        for (int i = 0; params_format[i] && !done; i++){
            if (params_format[i] == '%'){
                switch (params_format[i + 1]){
                    case 's':
                        PushString(va_arg( args, char*));
                        break;

                    case 'f':
                        PushFloat((float) va_arg( args, double)); // double from MSDN
                        break;

                    case 'c':
                    case 'd':
                        PushInt(va_arg( args, int));
                        break;

                    case 'b':
                        PushBool(va_arg( args, int) != 0);
                        break;

                    case 'e':{
                            Entity * entity = (Entity *) va_arg( args, void*);
                            PushInt(entity?entity->GIndex:0);
                        }
                        break;
                    case 'p':{
                            PushLightUD(va_arg( args, void*));
                        }
                        break;

                    default:
                        TRACE("Script::Call(): ERROR-> invalid parameter format character '%c'\n", params_format[i + 1]);
                        done = true;
                        count--;
                        break;
                }

                count++;
                i++;
            }
        }       
        
        va_end(args);
    }


#ifdef _DEBUG
    int err = lua_pcall(mLua, count, nresults, 0);

    if (err != 0){
        TRACE("Script::Call(): ERROR-> runtime error! (msg = '%s')\n", GetString(-1, "unknown"));

        Assert(0);
    }
#else
    lua_call(mLua, count, nresults);
#endif

    if (nresults == 0) lua_settop(mLua, top);
}
//----------------------------------------------------------------------------    
void Script::Call(Script &obj, const char *func_name, int nresults, const char *params_format, ...){
    int count = 0;
    int top = lua_gettop(mLua);


    //the script object <obj> references a stack index => grab the function pointer from the table identified by that index
    if (obj.mIndex){
        //make sure there's a table on top of the stack
        Assert(lua_istable(mLua, obj.mIndex));

        //push the function name onto the stack
        lua_pushstring(mLua, func_name);
        Assert(lua_isstring(mLua, -1));

        //access the context table to grab the function pointer
        lua_gettable(mLua, obj.mIndex);

        //make sure the stack has the function on top of the stack
        Assert(lua_isfunction(mLua, -1));

        //push the context table on after the function
        lua_pushvalue(mLua, obj.mIndex);
        Assert(lua_istable(mLua, -1));

        //update the argument count to include the context table
        count++;
    }

    //the script object <obj> doesn't reference anything => grab the function pointer from the global table
    else{
        lua_pushstring(mLua, func_name);
        lua_gettable(mLua, LUA_GLOBALSINDEX);

        Assert(lua_isfunction(mLua, -1));
    }


    // eric-> make sure a params list was passed in before attempting to parse non-existent args.
    //   Yet another potential error that wasn't checked for.
    if (params_format){
        va_list args;
        va_start(args,params_format);

        // eric-> change this unfriendliness here... allow the parser to seperate tokens by spaces as
        //   well as commas.  This won't affect any other behaviour since the parser doesn't handle 
        //   non-printf() style formatting tokens at all (ie: the string tokens 'edible', 'idiot', and
        //   'addition' will each parse as an arg expecting an int)
        //const char * type = strchr(pbuf, ", \t");
        bool done = false;


        for (int i = 0; params_format[i] && !done; i++){
            if (params_format[i] == '%'){
                switch (params_format[i + 1]){
                    case 's':
                        PushString(va_arg( args, char*));
                        break;

                    case 'f':
                        PushFloat((float) va_arg( args, double)); // double from MSDN
                        break;

                    case 'c':
                    case 'd':
                        PushInt(va_arg( args, int));
                        break;

                    case 'b':
                        PushBool(va_arg( args, int) != 0);
                        break;

                    case 'e':{
                            Entity * entity = (Entity *) va_arg( args, void*);
                            PushInt(entity?entity->GIndex:0);
                        }
                        break;
                    case 'p':{
                            PushLightUD(va_arg( args, void*));
                        }
                        break;

                    default:
                        TRACE("Script::Call(): ERROR-> invalid parameter format character '%c'\n", params_format[i + 1]);
                        done = true;
                        count--;
                        break;
                }

                count++;
                i++;
            }
        }       
        
        va_end(args);
    }


#ifdef _DEBUG
    int err = lua_pcall(mLua, count, nresults, 0);

    if (err != 0){
        TRACE("Script::Call(): ERROR-> runtime error! (msg = '%s')\n", GetString(-1, "unknown"));

        Assert(0);
    }
#else
    lua_call(mLua, count, nresults);
#endif

    if (nresults==0) lua_settop(mLua,top);
}
//----------------------------------------------------------------------------    
int Script::GetTop()                                
{
    return lua_gettop(mLua);  
}
//----------------------------------------------------------------------------    
void Script::SetTop(int index)                      
{ 
    lua_settop(mLua, index);  
}
//----------------------------------------------------------------------------    
void Script::PushString(const char *s)              
{
    lua_pushstring(mLua, s); 
}
//----------------------------------------------------------------------------    
void Script::PushInt(int number)
{
    lua_pushnumber(mLua, (lua_Number)number); 
}
//----------------------------------------------------------------------------    
void Script::PushFloat(float number)
{
    lua_pushnumber(mLua, number); 
}
//----------------------------------------------------------------------------    
int Script::NewTable() 
{
    lua_newtable(mLua);
    return GetTop();
}
//----------------------------------------------------------------------------    
void Script::SetTable(int idx) 
{
    lua_settable(mLua, idx);
};
//----------------------------------------------------------------------------    
int Script::GetCount(int idx)
{
    return luaL_getn (mLua,idx);
}
//----------------------------------------------------------------------------    
void Script::QueueMessage(const char* msg_name,const char* params_format,...){
    if ( !mLua )
        return;

    int count = 1;

    lua_pushstring(mLua,"Game_GetMsg");
    lua_gettable(mLua,LUA_GLOBALSINDEX);

    PushString(msg_name);

    if (params_format){
        va_list args;
        va_start(args,params_format);   
        bool done = false;
        
        
        for (int i = 0; params_format[i] && !done; i++){
            if (params_format[i] == '%'){
                switch (params_format[i + 1]){
                    case 's':
                        PushString(va_arg( args, char*));
                        break;

                    case 'f':
                        PushFloat((float) va_arg( args, double)); // double from MSDN
                        break;

                    case 'd':
                        PushInt(va_arg( args, int));
                        break;

                    case 'c':
                        PushInt(va_arg( args, int));
                        break;

                    case 'b':
                        PushBool(va_arg( args, int) != 0);
                        break;

                    case 'e':{
                            Entity * entity = (Entity *) va_arg( args, void*);
                            PushInt(entity?entity->GIndex:0);
                        }
                        break;

                    case 'p':{
                            PushLightUD(va_arg( args, void*));
                        }
                        break;

                    default:
                        TRACE("Script::QueueMessage(): ERROR-> invalid format character '%c'\n", params_format[i + 1]);
                        count--;
                        done = true;
                        break;
                }

                count++;
                i++;
            }
        }

        va_end(args);
    }


    lua_call(mLua, count, 0);
}
/*
//----------------------------------------------------------------------------    
const char* Script::GetSafeCfgVar( const char* varname, const char* defaultvalue )
{
    int t = GetTop();

    const char * ret = defaultvalue;
    ScriptObject CfgVar = Globals()["Cfg"][varname];

    if (!CfgVar.IsNil() && CfgVar.IsString())
        ret = CfgVar.GetString();

    SetTop(t);
    return ret;
}
//----------------------------------------------------------------------------    
void Script::SetSafeCfgVar( const char* varname, const char* value )
{
    DoString( "Cfg.%s = '%s'", varname, value );
}
*/
//----------------------------------------------------------------------------    
void Script::SetHook(lua_Hook func, unsigned long mask)
{   
        lua_sethook(mLua,func,mask,1);
}
//----------------------------------------------------------------------------    
void Script::GetInfo(const char *args, lua_Debug *ar)
{
    lua_getinfo(mLua, args, ar);
}


ScriptObject Script::Globals()
{
    //lua_gettable(mLua,LUA_GLOBALSINDEX);
    //lua_pushvalue(mLua, LUA_GLOBALSINDEX);
    //return Script(mLua,GetTop());
    return ScriptObject( mLua, gt(mLua) );
}

//----------------------------------------------------------------------------    
//----------------------------------------------------------------------------    

ScriptObject::ScriptObject(lua_State *pLua,const TObject *obj)
{
    mLua = pLua;
    setnilvalue2n(&mObject);
    if (obj)
        lua_setobj(mLua, &mObject, obj);
    mParent = NULL;
    mSKey = NULL;
}

ScriptObject::~ScriptObject()
{
    if (mLua)
        setnilvalue(&mObject);
    
    if (mSKey)
        free(mSKey);
}

float ScriptObject::GetFloat() const
{
    Assert(mLua  &&  IsNumber());
    return (float)nvalue(&mObject);
}

int ScriptObject::GetInt() const
{
    Assert(mLua  &&  IsNumber());
    return (int)nvalue(&mObject);
}

const char* ScriptObject::GetString() const
{
    Assert(mLua  &&  IsString());
    return svalue(&mObject);
}

bool ScriptObject::GetBool() const
{
    Assert(mLua  &&  IsBool());
    return !l_isfalse(&mObject);
}

void* ScriptObject::GetLightUD() const
{
    Assert(mLua  &&  IsLightUD());
    return pvalue(&mObject);
}

ScriptObject ScriptObject::GetMetatable() const
{
    Assert(mLua  &&  IsTable());
    Table *mt = luaT_getmetatable(mLua, &mObject);
    ScriptObject ret(mLua);
#ifndef LUA_VERSION_NUM // ?
    if (mt != hvalue(defaultmeta(mLua)))
#endif
        sethvalue2n(&ret.mObject, mt);
    return ret;
}


int ScriptObject::GetCount() const
{
    Assert(mLua  &&  IsTable());
    luaA_pushobject(mLua,&mObject);
    int n = luaL_getn(mLua, -1);
    lua_pop(mLua, 1);
    return n;
}

int ScriptObject::GetType() const
{
    Assert(mLua);
    return ttype(&mObject);
}


void ScriptObject::UpdateValue()
{
    /*
    if (!mParent)
        return;

    TObject keyObj;
    if (mSKey)
    {
        lua_setsvalue(mLua, &keyObj, luaS_newlstr(mLua, mSKey, strlen(mSKey)));
    }
    else
        setnvalue(&keyObj, mIKey);
        */

//  luaV_settable(mLua, &mObject, &keyObj, &valueObj);
}

void ScriptObject::SetNil()
{
    setnilvalue(&mObject);
    UpdateValue();
}


void ScriptObject::SetInteger(int val)
{
//  int abc = (int)nvalue(&mObject);

    setnvalue(&mObject, (lua_Number)val);
    //TObject *val = gval(&mObject);

    UpdateValue();
}

void ScriptObject::SetFloat(float val)
{
    setnvalue(&mObject, val);
    UpdateValue();
}

void ScriptObject::SetBool(bool val)
{
    setbvalue(&mObject, val);
    UpdateValue();
}

void ScriptObject::SetString(const char* val)
{
    if (val == NULL)
        setnilvalue(&mObject);
    else
        lua_setsvalue(mLua, &mObject, luaS_newlstr(mLua, val, strlen(val)));

    UpdateValue();
}

void ScriptObject::SetTObject(TObject* value)
{
    lua_setobj(mLua, &mObject, value);
    UpdateValue();
}

bool ScriptObject::IsNumber() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TNUMBER;
}

bool ScriptObject::IsString() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TSTRING;
}

bool ScriptObject::IsBool() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TBOOLEAN;
}

bool ScriptObject::IsFunction() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TFUNCTION;
}

bool ScriptObject::IsLightUD() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TLIGHTUSERDATA;
}

bool ScriptObject::IsTable() const
{
    Assert(mLua);
    return ttype(&mObject) == LUA_TTABLE;
}

bool ScriptObject::IsNil() const
{
    Assert(mLua);
    return (ttype(&mObject) == LUA_TNIL || ttype(&mObject) == LUA_TNONE);
}

ScriptObject &ScriptObject::NewKey(const char* key)
{
    Assert(mLua  &&  IsTable());
    TObject valueObj,keyObj;
    
    setnvalue(&valueObj,-2);
    lua_setsvalue(mLua, &keyObj, luaS_newlstr(mLua, key, strlen(key)));
     
    luaV_settable(mLua, &mObject, &keyObj, &valueObj);
    return *this; //ScriptObject(mLua,&valueObj);
}

ScriptObject &ScriptObject::NewKey(int key)
{
    Assert(mLua  &&  IsTable());

    TObject valueObj,keyObj;

    setnilvalue(&valueObj);
    setnvalue(&keyObj, (lua_Number)key);

    luaV_settable(mLua, &mObject, &keyObj, &valueObj);
    return *this;
}


ScriptObject ScriptObject::operator[](const char* name)
{
    Assert(mLua);
    lua_State* L = mLua;
    TObject str;
    setnilvalue2n(&str);

    // It's safe to assume that if name is not in the hash table, this function can return nil.
    size_t l = strlen(name);
    GCObject *o;
    lu_hash h = (lu_hash)l;  /* seed */
    size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */
    size_t l1;
    for (l1=l; l1>=step; l1-=step)  /* compute hash */
        h = h ^ ((h<<5)+(h>>2)+(unsigned char)(name[l1-1]));
    for (o = G(L)->strt.hash[lmod(h, G(L)->strt.size)];
        o != NULL;
        o = o->gch.next)
    {
        TString *ts = gcotots(o);
        if (ts->tsv.tt == LUA_TSTRING && ts->tsv.len == l && (memcmp(name, getstr(ts), l) == 0))
        {
            lua_setsvalue2n(mLua, &str, ts);
            break;
        }
    }

    if (ttype(&str) == LUA_TNIL){
        TRACE("//////////////////////////////////////////////////////////////////////////////////////////\n");
        TRACE("/// ScriptObject::operator[](): ERROR-> accessed a nil object! (name = '%s')\n", name);
        TRACE("//////////////////////////////////////////////////////////////////////////////////////////\n");

        return ScriptObject(mLua);
    }

    //  lua_setsvalue(mLua, &str, luaS_newlstr(L, name, strlen(name)));
    const TObject* v = luaH_get(hvalue(&mObject), &str);
    setnilvalue(&str);
    return ScriptObject(mLua, v);
}


ScriptObject ScriptObject::operator[](int idx)
{
    Assert(mLua);
    StkId o = &mObject;
    const TObject* v = luaH_getnum(hvalue(o), idx);
    return ScriptObject(mLua, v);
}


bool ScriptTable_next(lua_State* state, ScriptObject* table, ScriptObject* key, ScriptObject* value)
{
    Table* t = hvalue(&table->mObject);
    int i = luaH_index(state, t, &key->mObject);  /* find original element */
    for (i++; i < t->sizearray; i++)
    {  /* try head array part */
        if (!ttisnil(&t->array[i]))
        {  /* a non-nil value? */
            key->SetInteger(i + 1);
            value->SetTObject(&t->array[i]);
            return true;
        }
    }
    for (i -= t->sizearray; i < sizenode(t); i++)
    {  /* then hash part */
        if (!ttisnil(gval(gnode(t, i))))
        {  /* a non-nil value? */
            key->SetTObject(lua_gkey(gnode(t, i)));
            value->SetTObject(lua_gval(gnode(t, i)));
            return true;
        }
    }
    return false;  /* no more elements */
}

