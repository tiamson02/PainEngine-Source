function testLoadObj()
    local results = {}
    
    
    for i = 1, 5 do
        results[i] = loadObj("scripts/test.CObject")
    end
end

function testClone1()
    local results = {}
    
    
    for i = 1, 5 do
        results[i] = Clone1(CObject)
    end
end

function testClone2()
    local results = {}
    
    
    for i = 1, 5 do
        results[i] = Clone2(CObject)
    end
end

function testCloneObj()
    local results = {}
    
    
    for i = 1, 5 do
        results[i] = CloneObj(CObject)
    end
end

function testCloneObjSafety()
    local obj = CloneObj(CObject)
    
    printList(CObject, "CObject")
    printList(obj, "obj")
    
    local obj2 = CloneObj(CObject)
    
    printList(CObject, "CObject")
    printList(obj2, "obj2")
end