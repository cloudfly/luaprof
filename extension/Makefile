LUAINC = /usr/local/lua/include/

LUALIB = /usr/local/lib/lua/5.1/

OUT = luaprof.so

############################################

WARN= -fPIC -pedantic -Wall -W

CFLAGS= -I$(LUAINC) $(WARN) -O2

CC = gcc -c $(CFLAGS)

FILEO = luaprof.o common.o tree.o data.o

profile: $(FILEO)
	gcc -shared -o luaprof.so $(FILEO)
	cp $(OUT) ../

luaprof.o: luaprof.c luaprof.h
	$(CC) -o luaprof.o luaprof.c
common.o: common.c common.h
	$(CC) -o common.o common.c
tree.o: tree.c tree.h
	$(CC) -o tree.o tree.c
data.o: data.c data.h
	$(CC) -o data.o data.c

install:
	mv $(OUT) $(LUALIB)

clean:
	rm -f *.o *.so
