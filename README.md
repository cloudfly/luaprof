#luaprof
a simple lua profile tool, check the time of all the lua function cost, and the count of they are called.

##Install

edit Makefile, change the LUAINC depends on your own computer, then run commands below

```shell
sudo make
mv luaprof.so /usr/local/lib/lua/5.1/  #this dir may be different with your computer
```
##API
###start(filepath)
start the luaprof, then the luaprof will begin to work from here

the argument filepath should be an absolute path, and the file must can be writen

###stop()
stop the luaprof

###output()
write the data into the file, which you assigned before in the api start()

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
profile.output()
```




