local tmp = "/tmp"
local sep = "/"
local upper = ".."

require"lfs"
print ("license update")

local license = [[
/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
]]

local info = [[
// Module: $FILENAME
// Creator: $ANCHOR $EMAIL

]]

function update_head(file,name)
	local f = io.open(file,"r")
	if f == nil then return end
	local data = f:read("*all")
	f:close()
	local i0 = string.find(data,"Module:")
	local i1 = string.find(data,"Creator:")
	if i0 ~= nil and i1 ~= nil then
		print("skip",file)
		return
	end
	local i = string.find(data,"#")
	if i == nil then
		i = 1
	end
	f = io.open(file,"w")
	if f == nil then return end
	local w_info = string.gsub(info,"$FILENAME",name)
	w_info = string.gsub(w_info,"$ANCHOR","visualfc")
	w_info = string.gsub(w_info,"$EMAIL","<visualfc@gmail.com>")
	f:write(license..w_info..string.sub(data,i,#data))
	f:close()
	print(file)
end

function process_file(file,name)
	local ext = string.match(file,"%.%w+$")
	if  ext == ".h" or ext == ".cpp" then
		update_head(file,name)
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
attrdir("./plugins")
attrdir("./utils")
attrdir("./api")
