#include "Z_Save.h"

void Z_Save::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_SaveGame", "data"), &Z_Save::set_SaveGame);
	ClassDB::bind_method(D_METHOD("get_SaveGame"), &Z_Save::get_SaveGame);

	ClassDB::bind_method(D_METHOD("set_SaveGlobal", "data"), &Z_Save::set_SaveGlobal);
	ClassDB::bind_method(D_METHOD("get_SaveGlobal"), &Z_Save::get_SaveGlobal);



	// Add as property for editor
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "Save_Game", PROPERTY_HINT_RESOURCE_TYPE, "ZSave_Game"), "set_SaveGame", "get_SaveGame");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "Save_Global", PROPERTY_HINT_RESOURCE_TYPE, "ZSave_Global"), "set_SaveGlobal", "get_SaveGlobal");

}

Z_Save::Z_Save() {
	Save_Game.instantiate();
	Save_Global.instantiate();
}

void Z_Save::set_SaveGame(const Ref<ZSave_Game> &p_data) {
	Save_Game=p_data;
}

Ref<ZSave_Game> Z_Save::get_SaveGame() const {
	return Save_Game;
}

void Z_Save::set_SaveGlobal(const Ref<ZSave_Global> &p_data) {
	Save_Global=p_data;
}

Ref<ZSave_Global> Z_Save::get_SaveGlobal() const {
	return Save_Global;
}

