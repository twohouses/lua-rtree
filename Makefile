all: rtree.so

CC = gcc
CFLAGS = -g3 -O0 -Wall -fPIC  -lm --shared
LUA_INCLUDE_DIR = /usr/local/include
DEFS = -DLUA_COMPAT_5_2

rtree.so: rtree.h rtree.c lua-rtree.c
	$(CC)  $(CFLAGS)  -I$(LUA_INCLUDE_DIR) $(DEFS)  $^ -o $@

test:
	lua test.lua

clean:
	-rm rtree.so
