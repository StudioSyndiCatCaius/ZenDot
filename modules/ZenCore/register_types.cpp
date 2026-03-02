#include "register_types.h"

#include "core/object/class_db.h"
#include "ZenCore.h"
#include "Z_Math.h"
#include "Z_Node.h"
#include "Z_Save.h"
#include "Z_String.h"
#include "1D/N1D_Input.h"
#include "Save_Base.h"
#include "Save_Game.h"
#include "Save_Global.h"
#include "Z_Game.h"
#include "Z_Log.h"
#include "3D/N3D_ZenCharacter.h"
#include "res/GameplayResource.h"
#include "res/res_Attribute.h"


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
	ClassDB::register_class<Z_Log>();
	ClassDB::register_class<Z_Game>();

	//Resources
	ClassDB::register_class<GameplayResource>();
	ClassDB::register_class<ZenAttribute>();
	ClassDB::register_class<ZSave_Base>();
	ClassDB::register_class<ZSave_Game>();
	ClassDB::register_class<ZSave_Global>();

	//NODE 1D
	ClassDB::register_class<N1D_Input>();

	ClassDB::register_class<ZenCharacter>();
}

void uninitialize_ZenCore_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Nothing to do here in this example.
}
