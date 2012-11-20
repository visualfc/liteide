require"lfs"

local sources = {}

function finddir (path)
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file
            local attr = lfs.attributes (f)
            assert (type(attr) == "table")
            if attr.mode == "directory" then
                finddir (f)
            else
                sources[file] = f
            end
        end
    end
end


function process_file(file,out,tag)
	local f = io.open(file,"rb")
	local data = f:read("*all")
	f:close()
	data = string.gsub(data,"Hello",tag)
	data = string.gsub(data,"hello",string.lower(tag))
	data = string.gsub(data,"HELLO",string.upper(tag))
	print (out)
	f = io.open(out,"wb")
	f:write(data)
	f:close()
end

function make_plugin(files,tag)
	for k,v in pairs(files) do
		print(k,v)
		local out = string.gsub(k,"hello",string.lower(tag))
		process_file(v,"./output/"..out,tag)
	end
end

function makelib(dir)
	sources = {}
	finddir(dir)
	print("make form "..dir.." input name:")
	name = io.read()
	if #name >= 4 then
		make_plugin(sources,name)
	end
end
