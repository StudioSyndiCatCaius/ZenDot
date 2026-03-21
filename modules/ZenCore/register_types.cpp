#include "register_types.h"

#include "core/object/class_db.h"
#include "ZenCore.h"
#include "Z.h"

#include "G/Z_Log.h"
#include "G/Z_Math.h"
#include "G/Z_Node.h"
#include "G/Z_Save.h"
#include "G/Z_String.h"
#include "G/Z_File.h"
#include "G/z_parse.h"
#include "G/Z_OS.h"

#include "Save_Base.h"
#include "Save_Game.h"
#include "Save_Global.h"
#include "1D/Z1D_Input.h"

#include "G/Z_Lit.h"
#include "UI/Zen_FileTree.h"
#include "UI/Zen_TableEditor.h"

#include "res/GameplayResource.h"
#include "res/res_Attribute.h"
#include "res/Zen_FileTypeConfig.h"
#include "res/Zen_ParamType.h"
#include "res/Zen_TableCellConfig.h"


static Z* z_game_singleton = nullptr;

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
	ClassDB::register_class<Z_Parse>();
	ClassDB::register_class<Z_File>();
	ClassDB::register_class<Z_OS>();
	ClassDB::register_class<Z_Lit>();

	//Resources
	ClassDB::register_class<GameplayResource>();
	ClassDB::register_class<ZenAttribute>();
	ClassDB::register_class<ZSave_Base>();
	ClassDB::register_class<ZSave_Game>();
	ClassDB::register_class<ZSave_Global>();
	ClassDB::register_class<Zen_FileTypeConfig>();
	ClassDB::register_class<Zen_TableCellConfig>();
	ClassDB::register_class<Zen_InputAction>();
	ClassDB::register_class<Zen_ParamType>();

	ClassDB::register_class<Zen_ParamType>();
	ClassDB::register_class<Zen_ParamType_String>();
	ClassDB::register_class<Zen_ParamType_Number>();
	ClassDB::register_class<Zen_ParamType_Bool>();
	ClassDB::register_class<Zen_ParamType_Options>();


	ClassDB::register_class<Zen_FileTypeConfig>();

	//NODE 1D
	ClassDB::register_class<Z1D_Input>();

	//NODE 3D
	//ClassDB::register_class<ZenCharacter>();

	//NODE UI
	ClassDB::register_class<Zen_FileTree>();
	ClassDB::register_class<Zen_TableEditor>();


	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(Z);  // use this macro, not ClassDB::register_class

		// Create and register the singleton
		z_game_singleton = memnew(Z);
		Engine::get_singleton()->add_singleton(
			Engine::Singleton("Z", Z::get_singleton())
		);
	}
}

void uninitialize_ZenCore_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (z_game_singleton) {
			memdelete(z_game_singleton);
			z_game_singleton = nullptr;
		}
	}
}
