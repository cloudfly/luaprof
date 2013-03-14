

LUAINC= /usr/local/lua/include

OUT= luaprof.so

CC= gcc
WARN= -fPIC -pedantic -Wall -W
INCS= -I$(LUAINC)

CFLAGS= $(INCS) $(WARN) -O2  

OBJS= luaprof.o

#profile: $(OBJS)
#	$(CC) -shared -o $(OUT) $(OBJS)
profile: 
	$(CC) -shared -o $(OUT) $(CFLAGS) -I./ luaprof.c common.c tree.c

clean:
	rm -f *.o a.out

