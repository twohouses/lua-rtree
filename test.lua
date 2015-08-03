local rtree = require "rtree.c"

local my_rtree = rtree()

for i=0,1000 , 3 do
	my_rtree:insert(i , i , i +2 , i +2 , i+1 )
	if i < 13 then
		my_rtree:delete(i , i , i +2 , i +2 , i+1 )
	end
end


local result = my_rtree:search(0,0,100,100)
for i,v in ipairs(result) do
	print(i,v)
end