local profile = require"luaprof"

local str = ""

function fibo(n)

    if n < 3 then
        return 1
    else
        return fibo(n - 1) + fibo(n - 2)
    end
end

function test1()
    for i = 1,500,2 do
        str = str .. i
    end
    fibo(10)
end

profile.start("./out_test")

test1()

profile.stop()

profile.output()


