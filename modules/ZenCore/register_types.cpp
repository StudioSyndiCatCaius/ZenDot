#include "register_types.h"

#include "core/object/class_db.h"
#include "ZenCore.h"
#include "Z_Math.h"
#include "Z_Node.h"
#include "Z_Save.h"
#include "Z_String.h"
#include "GameplayResource.h"
#include "Save_Base.h"
#include "Save_Game.h"
#include "Save_Global.h"


void initialize_ZenCore_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<ZenCore>();

	//Globals
	ClassDB::register_class<Z_Math>();
	ClassDB::register_class<Z_Node>();
	ClassDB::register_class<Z_Save>();
	ClassDB::register_class<Z_String>();

	//Resources
	ClassDB::register_class<GameplayResource>();
	ClassDB::register_class<ZSave_Base>();
	ClassDB::register_class<ZSave_Game>();
	ClassDB::register_class<ZSave_Global>();

}

void uninitialize_ZenCore_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}
