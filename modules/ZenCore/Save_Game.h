#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "Save_Base.h"

class ZSave_Game : public ZSave_Base {
	GDCLASS(ZSave_Game, ZSave_Base);

public:
	String current_level;
	Transform3D player_transform;
};


#endif
