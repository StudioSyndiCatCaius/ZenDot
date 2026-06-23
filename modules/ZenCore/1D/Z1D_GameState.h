#pragma once

#include "scene/main/node.h"
#include "core/input/input_event.h"

class Z1D_GameState : public Node {
	GDCLASS(Z1D_GameState, Node);


public:
	Z1D_GameState();

	HashSet<StringName> active_inputs;
};
