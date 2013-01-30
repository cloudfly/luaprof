

LUAINC= /usr/local/openresty/lua/include

OUT= luaprof.so

CC= gcc
WARN= -fPIC -pedantic -Wall -W
INCS= -I$(LUAINC)

CFLAGS= $(INCS) $(WARN) -O2  

OBJS= luaprof.o

profile: $(OBJS)
	$(CC) -shared -o $(OUT) $(OBJS)

clean:
	rm -f *.o a.out

