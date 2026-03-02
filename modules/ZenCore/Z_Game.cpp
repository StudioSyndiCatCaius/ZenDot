#include "Z_Game.h"


void Z_Game::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_VARS", "VARS"), &Z_Game::set_VARS);
	ClassDB::bind_method(D_METHOD("get_VARS"), &Z_Game::get_VARS);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "VARS", PROPERTY_HINT_DICTIONARY_TYPE, "", PROPERTY_USAGE_DEFAULT, "Dictionary"), "set_VARS", "get_VARS");

}

