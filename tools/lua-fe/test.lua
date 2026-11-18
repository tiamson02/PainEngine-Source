-- this is a comment
a = 1
b = 2

if a and not b then
	c = 3
elseif b and not a then
	d = 4
elseif a and b then
	e = 5
else
	f = 6
end

do
	j = 2
end

i = 10
while i do
	i = i - 1
end

-- FOR -------------------------------------------------------
t = {"a", "b", "c", "d"}
for i, v in ipairs(t) do
	print(i,v)
end

--------------------------------------------------------------

j = 1
repeat
	j = j + 1
until j == 10

-- function definition statements:

function foo ()
	local a = 1
end

function bar (a, b, c)
	local b = 2
end

function baz(a, ...)
	local pi = 3.14159
end

function weird(arg, ...)	-- rest of parameters are named "arg"; what happens?
	local e = 2.71828
	local f = ...
end

function t.a.b.c.g()
	local a = 1
end

local function h(d, e, f)
	local a = 1
end

function obj:method(a, b, ...)
	local i = -1
end

local a, b = 1, 2
local c, d, e = 1

-- UNARY OPERATORS ------------------------------------------------------------

x = not nil
n = #t
y = -j

-- BINARY OPERATORS ------------------------------------------------------------
z = 1 + 2
x = 3 * (1 + 2)
y = foo(3)

-- OPERATOR NESTING ------------------------------------------------------------

z = 2 * #t + 1
