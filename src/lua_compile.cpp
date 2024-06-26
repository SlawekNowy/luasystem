/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "luasystem.h"
#include <fsys/filesystem.h>
#include <sharedutils/util_file.h>

VFilePtrReal fLuaCompile = nullptr;
static int luaWriteBinary(lua_State *, unsigned char *str, size_t len, struct luaL_Buffer *)
{
	fLuaCompile->Write(str, len);
	return 0;
}

bool Lua::compile_file(lua_State *l, const std::string &path)
{
	auto lpath = ufile::get_path_from_filename(path);
	FileManager::CreatePath(lpath.c_str());
	fLuaCompile = FileManager::OpenFile<VFilePtrReal>(path.c_str(), "wb");
	if(fLuaCompile == nullptr) {
		Lua::Pop(l, 1);
		return false;
	}
	luaL_Buffer buf;
	luaL_buffinit(l, &buf);
#ifdef USE_LUAJIT
	lua_dump_strip(l, (lua_Writer)luaWriteBinary, &buf, 1);
#else
    lua_dump(l, (lua_Writer)luaWriteBinary, &buf);
#endif
	fLuaCompile = nullptr;
	return true;
}
