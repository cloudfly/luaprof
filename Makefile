

LUAINC= /usr/local/openresty/lua/include

OUT= luaprof.so

CC= g++
WARN= -fPIC -pedantic -Wall -W
INCS= -I$(LUAINC)

CFLAGS= $(INCS) $(WARN) -O2  

OBJS= luaprof.o

#profile: $(OBJS)
#	$(CC) -shared -o $(OUT) $(OBJS)
profile: 
	$(CC) -shared -o $(OUT) $(CFLAGS) -I./ luaprof.c

clean:
	rm -f *.o a.out

