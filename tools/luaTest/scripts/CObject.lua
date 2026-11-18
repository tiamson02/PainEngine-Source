CObject = {
    Name = "CObject",
    value1 = 4,
    value2 = 7,
    pos = {0, 0, 0},
}

function CObject:isCheese()
    if (self.cheeseCurds) then
        print("CObject:isCheese(): the object '"..self.Name.."' is made of cheese")
        return true
    else
        print("CObject:isCheese(): the object '"..self.Name.."' is not made of cheese")
        return false 
    end
end

function CObject:getPos()
    return self.pos
end