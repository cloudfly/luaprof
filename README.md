#luaprof
a simple lua profile tool, check the time of lua function cost, and the count of they are called.

##Installation

edit Makefile, change the LUAINC, it is the directory of lua.h and lauxlib.h stay in,
then run commands below

```shell
sudo make
mv luaprof.so /usr/local/lib/lua/5.1/  #this dir may be different to your computer
```
##API
###start(filepath)
start the luaprof, then the luaprof will begin to track your code from here

NOTE : the argument filepath should be an absolute path, and the file must can be writen

###stop()
stop the luaprof

###output()
write the data into the file, which you assigned before in the function luaprof.start()

###release()
release the memory using by luaprof

###save2dot(dfile, pfile)
generate dot code, write into dfile. if pfile is given, it will generate a png image by calling `dot` command

###save2js(jfile)
generate javascript code, saving into jfile

###save2txt(tfile)
save data into a text file

##Example
see test.lua

```lua
local profile = require"luaprof"

function fibo(n)
    if n < 2 then
        return 1
    else
        return fibo(n - 1) + fibo(n - 2)
    end
end

function test()
    arr = {"hello", "world"}
    print(str)
    for i=0,10000000,2 do
        i = i - 1
    end
    fibo(6)
end

profile.start("/home/cloudfly/work/logs/test_prof.out")

test()

profile.stop()


profile.save2dot("./data.dot", "./data.png")
profile.save2js("./data.js")
profile.save2txt("./data.txt")

profile.release()
```




