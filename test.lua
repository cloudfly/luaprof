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



