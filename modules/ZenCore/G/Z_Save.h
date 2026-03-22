#ifndef ZSAVE_H
#define ZSAVE_H

#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "modules/ZenCore/Save_Game.h"
#include "modules/ZenCore/Save_Global.h"

class Z_Save : public RefCounted {
	GDCLASS(Z_Save, RefCounted);


protected:
	static void _bind_methods();

public:

	Z_Save();


};

#endif
