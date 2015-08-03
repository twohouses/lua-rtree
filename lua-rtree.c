#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "rtree.h"


typedef struct _rtree_context
{
	lua_State *L ;
	int count ;
}rtree_context;

static inline RTREENODE* 
_to_rtree(lua_State *L) {
	RTREENODE **proot = lua_touserdata(L, 1);
	if(proot==NULL) {
		luaL_error(L, "must be rtree object");
	}
	return *proot;
}

static int
_insert(lua_State *L) {
	RTREENODE *root = _to_rtree(L);
	double min_x = luaL_checknumber(L, 2);
	double min_y = luaL_checknumber(L, 3);
	double max_x = luaL_checknumber(L, 4);
	double max_y = luaL_checknumber(L, 5);
	int id = luaL_checkinteger(L, 6);

	RTREEMBR rect = { {min_x, min_y,  max_x, max_y}  };
	RTreeInsertRect(&rect, id,  &root,  0 );

	RTREENODE **proot = lua_touserdata(L, 1);
	*proot = root;
	return 0;
}

static int
_delete(lua_State *L) {
	RTREENODE* root = RTreeCreate();
	double min_x = luaL_checknumber(L, 2);
	double min_y = luaL_checknumber(L, 3);
	double max_x = luaL_checknumber(L, 4);
	double max_y = luaL_checknumber(L, 5);
	int id = luaL_checkinteger(L, 6);

	RTREEMBR rect = { {min_x, min_y,  max_x, max_y}  };
	RTreeDeleteRect(&rect, id,  &root);
	RTREENODE **proot = lua_touserdata(L, 1);
	*proot = root;
	return 0;
}

static int
_new(lua_State *L) {
	RTREENODE* root = RTreeCreate();
	RTREENODE **proot = (RTREENODE**) lua_newuserdata(L, sizeof(RTREENODE*));
	*proot = root;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	return 1;
}

static int 
search_callback(int id, void* arg) 
{
   	rtree_context * context = (rtree_context *)arg;
   	lua_pushinteger(context->L, id);
	lua_rawseti(context->L,-2,context->count);
	context->count++;
	return 1;
}

static int
_search(lua_State *L) {
	RTREENODE *root = _to_rtree(L);
	double min_x = luaL_checknumber(L, 2);
	double min_y = luaL_checknumber(L, 3);
	double max_x = luaL_checknumber(L, 4);
	double max_y = luaL_checknumber(L, 5);

	rtree_context * context = malloc(sizeof(rtree_context));
	context->count = 1;
	context->L = L;
	lua_createtable(L,0,0);

	RTREEMBR search_rect = { {min_x, min_y,  max_x, max_y}  };
	RTreeSearch(root, &search_rect, search_callback , (void *)context);
	free(context);
	return 1;
}	

static int
_release(lua_State *L) {
	RTREENODE *root = _to_rtree(L);
	RTreeDestroy (root);
	return 0;
}

int luaopen_rtree_c(lua_State *L) {
	luaL_checkversion(L);

	luaL_Reg l[] = {
		{"insert", _insert},
		{"delete", _delete},
		{"search", _search},
		{NULL, NULL}
	};

	lua_createtable(L, 0, 2);

	luaL_newlib(L, l);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, _release);
	lua_setfield(L, -2, "__gc");

	lua_pushcclosure(L, _new, 1);
	return 1;
}


