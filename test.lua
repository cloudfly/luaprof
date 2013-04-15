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
    fibo(4)
end

function test2()
    local a = 0;
    test1()
    for i = 1, 1000000, 1 do
        a = a + 1
    end
end

profile.start("./out_test2")

test2()
test1()
fibo(5)

profile.stop()

profile.print_result(print);

profile.release()


