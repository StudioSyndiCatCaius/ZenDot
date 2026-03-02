#ifndef Z_GAME_H
#define Z_GAME_H

#include "core/object/ref_counted.h"
#include "core/math/vector3.h"

class Z_Game : public RefCounted {
	GDCLASS(Z_Game, RefCounted);
public:

protected:
	static void _bind_methods();

public:
	// global variables
	Dictionary VARS;
	void set_VARS(const Dictionary &p_VARS) { VARS=p_VARS;}
	Dictionary get_VARS() const { return VARS; }

};

#endif
