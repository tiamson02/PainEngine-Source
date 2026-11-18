#include "Scripting.h"
#include "entity.h"


static int luaEntity_Release(lua_State* state){
    Script L(state);

    Entity *e = (Entity *) getEntityByIndex(L.GetInt(1));

    if (e){
        TRACE("luaEntity_Release(): destroying the entity '%s' with index %d\n", e->name, e->GIndex);

        deleteEntity(e);
    }

    return 0;
}

static int luaEntity_Create(lua_State* state){
    Script L(state);

    Entity * pe = createEntity(L.GetInt(1),L.GetString(2),L.GetString(3),L.GetFloat(4,1),L.GetBool(5,false) );

    if (pe){
        TRACE("luaEntity_Create(): creating the entity '%s' with index %d\n", pe->name, pe->GIndex);
        L.PushInt(pe?pe->GIndex:0);
        L.PushInt(pe?pe->GIndex:0);

        return 2;
    }
    
    else{
        TRACE("luaEntity_Create(): ERROR-> could not create the entity '%s'\n", L.GetString(2));
        L.PushInt(-1);

        return -1;
    }
}


static const luaL_reg entity_lib[] = {
    {"Create", luaEntity_Create},
    {"Release", luaEntity_Release},
    {NULL, NULL}
};


static int luaTest_callback(lua_State *state){
    Script L(state);
    Script param(state, 1);


    if (param.IsString())
        TRACE("luaTest_callback(): param is a string: '%s'\n", param.GetString(param.GetIndex(), "<none>"));
    
    else if (param.IsFunction())
        TRACE("luaTest_callback(): param is a function\n");

    else if (param.IsNumber())
        TRACE("luaTest_callback(): param is a number: %g\n", param.GetFloat(param.GetIndex(), -4));

    else if (param.IsTable()){
        TRACE("luaTest_callback(): param is a table\n");

        param.Call("doSomeStuff", 0, "%d", 15);
        param.Call(param, "doSomeStuff", 0, "%d", 13);
    }

    else if (param.IsNil(param.GetIndex()))
        TRACE("luaTest_callback(): param is nil\n");
    

    return 0;
}

static const luaL_reg test_lib[] = {
    {"callback", luaTest_callback},
    {NULL, NULL}
};



static int isKeyCopyable(const char *str){

    //no name!
    if (str[0] == 0)
        return false;

    //potentially a reference member => skip
    else if (str[0] == '_'){
        if (str[1] == 'r' && str[2] == '_')
            return false;

        else if (str[1] == 'O' && str[2] == 'b' && str[3] == 'j')
            return false;

        else
            return true;
    }

    //potentially a static or reference object => skip
    else{
        if (str[0] == 'r' && str[1] == '_')
            return false;

        if (str[0] == 's' && str[1] == '_')
            return false;
    
        else if (str[0] == 'O' && str[1] == 'b' && str[2] == 'j')
            return false;

        else
            return true;
    }

    return true;
}

static int luaMember_isCopyable(lua_State *state){
    const char *str = luaL_optlstring(state, 1, "", NULL);


    //no name!
    if (str[0] == 0)
        lua_pushboolean(state, false);

    //not a table => just copy directly
    else if (!lua_istable(state, 2))
        lua_pushboolean(state, false);

    //metatable => skip
    else if (lua_getmetatable(state, 2))
        lua_pushboolean(state, false);

    //potentially a reference member => skip
    else if (str[0] == '_'){
        if (str[1] == 'r' && str[2] == '_')
            lua_pushboolean(state, false);

        else if (str[1] == 'O' && str[2] == 'b' && str[3] == 'j')
            lua_pushboolean(state, false);

        else
            lua_pushboolean(state, true);
    }

    //potentially a static or reference object => skip
    else{
        if (str[0] == 'r' && str[1] == '_')
            lua_pushboolean(state, false);

        if (str[0] == 's' && str[1] == '_')
            lua_pushboolean(state, false);

        else if (str[0] == 'O' && str[1] == 'b' && str[2] == 'j')
            lua_pushboolean(state, false);

        else
            lua_pushboolean(state, true);
    }

    return 1;
}

static const luaL_reg member_lib[] = {
    {"isCopyable", luaMember_isCopyable},
    {NULL, NULL}
};

/*
function Clone2(t)
    if not t then return nil end   
    local new = {}
    local i, v = next(t, nil)
    
    while i do
        if MEMBER.isCopyable(i, v) then
            new[i] = Clone(v)
        else
            new[i] = v
        end
        
        i, v = next(t, i) 
    end
    return new
end
//*/
static int cloneObjHelper(lua_State *state){
    const char *key;

    //table to copy
    int table = lua_gettop(state);

    //create the new table (at table + 1 on the stack).  This will replace table on the stack when complete
    lua_newtable(state);

    //table being created
    int newTable = lua_gettop(state);    


    lua_pushnil(state);

    while (lua_next(state, table)){
        //stack fudge: we want the key value duplicated on the stack since the lua_rawset() command
        //  would consume it otherwise, and the lua_next() function expects the previous key to be
        //  on top of the stack.  To accomplish this, we'll copy the key (at index -2) to the top
        //  of the stack, then re-insert it at position -2 (between the original key and the original
        //  matching value).  This way, after lua_rawset() is called, the key will still be at the top
        //  of the stack ready for lua_next() to consume it.
        lua_pushvalue(state, -2);
        lua_insert(state, -2);


        //current key-value pair is a table => copy recursively if copyable
        if (lua_istable(state, -1) && !lua_getmetatable(state, -1)){
            key = luaL_optlstring(state, -2, "", NULL);

            //the key-value pair is copyable as a table => copy recursively
            if (isKeyCopyable(key))
                cloneObjHelper(state);

            lua_rawset(state, newTable);
        }

        //current key-value pair is not a table => just copy
        else
            lua_rawset(state, newTable);
    }


    lua_replace(state, -2);

    return table;
}


static int CloneObj(lua_State *state){
    const char *key;


    //the table passed in was nil => return nil and fail
    if (lua_isnoneornil(state, 1)){
        lua_pushnil(state);

        return 1;
    }

    //the object on top of the stack is not a table => return nil and fail
    if (!lua_istable(state, 1)){
        lua_pushnil(state);

        return 1;
    }


    //create the new table on top of the stack
    lua_newtable(state);
    int table = lua_gettop(state);
    
    lua_pushnil(state);

    while (lua_next(state, 1) != 0){
        //stack fudge: we want the key value duplicated on the stack since the lua_rawset() command
        //  would consume it otherwise, and the lua_next() function expects the previous key to be
        //  on top of the stack.  To accomplish this, we'll copy the key (at index -2) to the top
        //  of the stack, then re-insert it at position -2 (between the original key and the original
        //  matching value).  This way, after lua_rawset() is called, the key will still be at the top
        //  of the stack ready for lua_next() to consume it.
        lua_pushvalue(state, -2);
        lua_insert(state, -2);


        //current key-value pair is a table => copy recursively if copyable
        if (lua_istable(state, -1) && !lua_getmetatable(state, -1)){
            key = luaL_optlstring(state, -2, "", NULL);

            //the key-value pair is copyable as a table => copy recursively
            if (isKeyCopyable(key))
                cloneObjHelper(state);

            lua_rawset(state, table);
        }

        //current key-value pair is not a table => just copy
        else
            lua_rawset(state, table);

    }

    lua_settop(state, table);

    return 1;
}

static int luaDoFile(lua_State* state){
    Script L(state);
    L.DoFile(L.GetString(1),L.GetBool(2,1));
    return 0;
}


void engine2Lua(Script &script){
    script.RegisterLibrary("ENTITY", entity_lib);	
    script.RegisterLibrary("TEST", test_lib);
    script.RegisterLibrary("MEMBER", member_lib);

    script.RegisterFunction("DoFile", luaDoFile);
    script.RegisterFunction("CloneObj", CloneObj);
}
