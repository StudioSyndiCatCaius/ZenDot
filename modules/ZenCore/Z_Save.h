#ifndef ZSAVE_H
#define ZSAVE_H

#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "modules/ZenCore/Save_Game.h"
#include "modules/ZenCore/Save_Global.h"

class Z_Save : public RefCounted {
	GDCLASS(Z_Save, RefCounted);

	Ref<ZSave_Game> Save_Game;
	Ref<ZSave_Global> Save_Global;

protected:
	static void _bind_methods();

public:

	Z_Save();

	// Getter/Setter for the resource
	void set_SaveGame(const Ref<ZSave_Game> &p_data);
	Ref<ZSave_Game> get_SaveGame() const;

	void set_SaveGlobal(const Ref<ZSave_Global> &p_data);
	Ref<ZSave_Global> get_SaveGlobal() const;

};

#endif
