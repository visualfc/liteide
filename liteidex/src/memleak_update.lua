local tmp = "/tmp"
local sep = "/"
local upper = ".."

require"lfs"
print (lfs._VERSION)

local memcheck = [[
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end
]]

function update_memleak(file,name)
	local f = io.open(file,"r")
	if f == nil then return end
	local data = f:read("*all")
	f:close()
	local find = string.find(data,"//lite_memory_check_begin")
	if find ~= nil then
		print("skip file:",file)
		return
	end
	print(">>process file:",file)
	local i = 0
	local last = 0
	while true do
		i = string.find(data,"#include",i+1)
		if i == nil then break end
		last = i
	end
	i = string.find(data,"\n",last+1)
	if i == nil then
		return
	end
	f = io.open(file,"w")
	if f == nil then return end
	f:write(string.sub(data,1,i)..memcheck..string.sub(data,i+1,#data))
	f:close()
end

function process_file(file,name)
	local ext = string.match(file,"%.%w+$")
	if ext == ".cpp" then
		update_memleak(file,name)
	end
end

function attrdir (path)
	for file in lfs.dir(path) do
		if file ~= "." and file ~= ".." then
			local f = path..sep..file
			local attr = lfs.attributes (f)
			assert (type(attr) == "table")
			if attr.mode == "directory" then
				attrdir (f)
			else
				process_file(f,file)
			end
		end
	end
end
attrdir("./liteapp")
attrdir("./api")
attrdir("./plugins")
attrdir("./utils")

