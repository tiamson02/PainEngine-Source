function Clone1(t)
    if not t then return nil end   
    local new = {}
    local i, v = next(t, nil) 
    while i do        
        if type(v)== "table" and not getmetatable(v) and string.sub(i,1,2) ~= "s_" and string.sub(i,1,3) ~= "Obj" and string.sub(i,1,4) ~= "_Obj" and string.sub(i,1,2) ~= "r_"  and string.sub(i,1,3) ~= "_r_" then
            v = Clone(v)
        end
        new[i] = v          
        i, v = next(t, i) 
    end
    return new
end

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


Clone = Clone1


function loadObj(file)
    o = Clone(CObject)
    
    DoFile(file)
    
    return o
end