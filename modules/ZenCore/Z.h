#ifndef Z_GAME_H
#define Z_GAME_H

#include "core/object/ref_counted.h"
#include "core/math/vector3.h"
#include "modules/luaAPI/src/classes/luaAPI.h"
#include "scene/main/node.h"

class Z : public Node {
	GDCLASS(Z, Node);

	static Z* singleton;
public:
	static Z* get_singleton();

	Z();
	~Z();
protected:
	static void _bind_methods();

public:
	// global variables
	Dictionary VARS;
	void set_VARS(const Dictionary &p_VARS) { VARS=p_VARS;}
	Dictionary get_VARS() const { return VARS; }

	Ref<LuaAPI> L;  // Ref<> keeps the refcount alive
	LuaAPI* get_lua() const { return L.ptr(); }
};

#endif
