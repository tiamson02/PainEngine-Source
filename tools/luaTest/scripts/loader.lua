EntityToObject = {}
setmetatable(EntityToObject, {__mode="vk"})
Objects = {}

Object = {
    _Name = "",
    _Entity = 0,
    maxIndex = 0,
}


function Object:doSomeStuff(value)
    print("Object:doSomeStuff(): name = '"..self._Name.."', entity = "..self._Entity..", value = "..value)
end

function checkConstant(msg)
    if (table.isconstant(EntityToObject)) then
        print("[loader.lua]: "..msg.."-> EntityToObject[] is constant!")
    end
end

checkConstant("load1")


function Clone3(t)
    if not t then return nil end   
    local new = {}
    local i, v = next(t, nil) 
    while i do        
        -- LUA PLUS if type(v)== "table" and not getmetatable(v).__call and string.sub(i,1,2) ~= "s_" and string.sub(i,1,3) ~= "Obj" and string.sub(i,1,4) ~= "_Obj"  then
        if type(v)== "table" and not getmetatable(v) and string.sub(i,1,2) ~= "s_" and string.sub(i,1,3) ~= "Obj" and string.sub(i,1,4) ~= "_Obj" and string.sub(i,1,2) ~= "r_"  and string.sub(i,1,3) ~= "_r_" then
            v = Clone3(v)
        end
        new[i] = v          
        i, v = next(t, i) 
    end
    return new
end

checkConstant("[loader0]")


function newObject(name, filename)
    checkConstant("adding '"..name.."'")
    local obj = Clone3(Object)

    checkConstant("clone done on '"..name.."'")
    local entity = ENTITY.Create(5, filename, name, 1.0, false)
    checkConstant("created entity '"..entity.."'")
    
    obj._Entity = entity
    obj._Name = name
    checkConstant("values set on '"..entity.."'")
    if (entity > Object.maxIndex) then
        Object.maxIndex = entity
    end
    checkConstant("index set on '"..entity.."'")
    
    print("newObject(): adding the object '"..name.."' as entity "..entity)
    checkConstant("before add '"..entity.."'")
    EntityToObject[entity] = obj
    --table.insert(EntityToObject, {entity, obj})
    checkConstant("after add '"..entity.."'")
    
    return obj
end
checkConstant("[loader1]")

function deleteObject(entity)
    print("deleteObject(): destroying the entity "..entity)
    checkConstant("before remove '"..entity.."'")
    EntityToObject[entity] = nil
    --table.remove(EntityToObject, entity)
    checkConstant("after add '"..entity.."'")
    
    ENTITY.Release(entity)
end
checkConstant("[loader2]")

function printList(list, name)
    print("printList(): the list '"..name.."' has "..table.getn(list).." elements")
    local index = 1
    
    for i,v in list do
        if (type(v) == "function") then
            print("  "..index..") key = '"..i.."()' (function)")
        elseif (type(v) == "table") then
            if (v._Name) then
                print("  "..index..") key = '"..i.."', value = '"..v._Name.."' (table)")
            else
                print("  "..index..") key = '"..i.."' (table)")
            end
        else
            print("  "..index..") key = '"..i.."', value = '"..v.."'")
        end
        
        index = index + 1
    end
    
    --[[for i=1,table.getn(list) do
        if (list[i] == nil) then
            print("  "..index..") key = '"..i.."', value = nil")
        else
            print("  "..index..") key = '"..i.."', value = '"..list[i]._Name.."'")
        end
        
        index = index + 1
    end--]]
    
    print("")
end

checkConstant("[loader3]")

function main()
    print("*** main(): starting entity table test")
    checkConstant("main 0")
    
    local r = 0
    local i = 0
    local count = 1;
    checkConstant("main 1")
    
    
    while i < 20 do 
        local obj = newObject("tmpName"..i, "filename"..i)

        
        print("main(): calling doSomeStuff() on object '"..obj._Name.."'")
        obj.doSomeStuff(obj, 17)
        obj:doSomeStuff(19)
        
        r = math.random()
        if (r < 0.5) then
            print("main(): queueing '"..obj._Name.."' for deleting")
            Objects[count] = obj
            count = count + 1
        elseif (r > 0.7) then
            print("main(): calling a function on {'"..obj._Name.."', "..obj._Entity.."}")
            TEST.callback(obj)
        end
        
        i = i + 1
    end
    
    printList(EntityToObject, "EntityToObject")
    printList(Objects, "Objects")
    
    for i = 1, table.getn(Objects) do
        deleteObject(Objects[i]._Entity)
        Objects[i] = nil
    end
    
    printList(EntityToObject, "EntityToObject")
    
    
    print("*** main(): done entity table test")
end

checkConstant("[loader4]")


function timingFunction()
    local x = 1
    
    
    for i = 1, 10000000 do
        x = x * 2;
        x = x + 3735928559;
    end
end

DoFile("scripts/utils.lua")
DoFile("scripts/CObject.lua")
DoFile("scripts/test.lua")