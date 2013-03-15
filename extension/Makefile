

LUAINC= /usr/local/lua/include

OUT= luaprof.so

CC= gcc
WARN= -fPIC -pedantic -Wall -W
INCS= -I$(LUAINC)

CFLAGS= $(INCS) $(WARN) -O2  

OBJS= luaprof.o

#profil: $(OBJS)
#	$(CC) -shared -o $(OUT) $(OBJS)
profile: 
	$(CC) -shared -o $(OUT) $(CFLAGS) -I./ luaprof.c common.c tree.c data.c
	cp luaprof.so /usr/local/lib/lua/5.1/luaprof.so

clean:
	rm -f *.o a.out

