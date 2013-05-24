local profile = require"luaprof"

local str = ""

function sum(c){
    local s = 0
    for i = 0, c do
        s = s + 1
    end
    return s;
}
function test1()

    for i = 1,500,1 do
        str = str .. i
    end
    print(str)
end

function test2()

    local t = {};
    for i = 1, 500, 1 do
        table.insert(t, i)
    end
    print(table.concat(t))
end

profile.start("./out_test2")

test2()
test1()

profile.stop()

profile.save2dot("/home/cloudfly/data.dot", "/home/cloudfly/data.png");

profile.printr();

profile.release()
