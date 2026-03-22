#include "Z.h"

#include "G/z_file.h"
#include "core/config/project_settings.h"
#include "scene/main/node.h"
#include "scene/main/window.h"


Z* Z::singleton = nullptr;

Z* Z::get_singleton() {
	return singleton;
}


String L_GetPath_GlobalSave() {
	return Z_File::GetDir_Saves()+"Global.tres";
}

Z::Z() {
	singleton = this;

	PackedStringArray p;
	p.push_back(String("base"));
	p.push_back(String("table"));
	p.push_back(String("string"));
	p.push_back(String("math"));

	// Register project settings
	GLOBAL_DEF("lua/autogenerate_lua_machine", true);
	GLOBAL_DEF("lua/auto_lua_libraries", p);

	// Set hints so the editor shows them nicely
	ProjectSettings::get_singleton()->set_custom_property_info(PropertyInfo(
		Variant::BOOL, "lua/autogenerate_lua_machine"
	));
	ProjectSettings::get_singleton()->set_custom_property_info(PropertyInfo(
		Variant::PACKED_STRING_ARRAY, "lua/auto_lua_libraries"
	));

	// Read settings
	bool auto_init = GLOBAL_GET("lua/autogenerate_lua_machine");
	PackedStringArray libs = GLOBAL_GET("lua/auto_lua_libraries");

	if (auto_init) {
		L.instantiate();

		// Use settings libraries if set, otherwise fall back to defaults
		Array bind_libs;
		if (libs.size() > 0) {
			for (int i = 0; i < libs.size(); i++) {
				bind_libs.push_back(libs[i]);
			}
		} else {
			bind_libs.push_back(String("base"));
			bind_libs.push_back(String("table"));
			bind_libs.push_back(String("string"));
			bind_libs.push_back(String("math"));
		}

		L->bindLibraries(bind_libs);
	}

	//init saves

	//init global
	if (ResourceLoader::exists(L_GetPath_GlobalSave())) {
		save_global=ResourceLoader::load(L_GetPath_GlobalSave());
	} else {
		save_global=memnew(ZSave_Global);
	}

	//ini game
}
Z::~Z() {
	L.unref();  // let the Ref<> handle cleanup
	singleton = nullptr;

	print_line("Z: DESTRUCTOR");

}

void Z::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_VARS", "VARS"), &Z::set_VARS);
	ClassDB::bind_method(D_METHOD("get_VARS"), &Z::get_VARS);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "VARS", PROPERTY_HINT_DICTIONARY_TYPE, "", PROPERTY_USAGE_DEFAULT, "Dictionary"), "set_VARS", "get_VARS");
	ClassDB::bind_method(D_METHOD("get_lua"), &Z::get_lua);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "L", PROPERTY_HINT_RESOURCE_TYPE, "LuaAPI"), "", "get_lua");


	ClassDB::bind_method(D_METHOD("get_save_game"), &Z::get_save_game);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "save_game", PROPERTY_HINT_RESOURCE_TYPE, "save_game"), "", "get_save_game");
	ClassDB::bind_method(D_METHOD("get_save_global"), &Z::get_save_global);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "save_global", PROPERTY_HINT_RESOURCE_TYPE, "save_global"), "", "get_save_global");
}

void Z::_enter_tree_deferred() {
	SceneTree *tree = Object::cast_to<SceneTree>(OS::get_singleton()->get_main_loop());
	tree->get_root()->add_child(this);
	print_line("Z: added to tree");
}

void Z::shutdown() {
	print_line("Zen: begine shutdown");
	if (save_global.is_valid()) {
		ResourceSaver::save(save_global,L_GetPath_GlobalSave());
	}
}

void Z::_notification(int p_what) {
	//print_line("got a notify");
	if (p_what == NOTIFICATION_PREDELETE ) {
		print_line("Z: NOTIFICATION_PREDELETE");
	}

	if (p_what == NOTIFICATION_WM_CLOSE_REQUEST) {
		print_line("Z: WM_CLOSE_REQUEST");
		// on finish, create global save
		if (save_global.is_valid()) {
			String path = L_GetPath_GlobalSave();
			print_line("Z: saving to: " + path);
			Error err = ResourceSaver::save(save_global, path);
			if (err != OK) {
				print_line("Z: save_global FAILED — error code: " + itos(err));
			} else {
				print_line("Z: save_global OK");
			}
		}
	}
}

void Z::StartGame(const Ref<ZSave_Game> &p_data,bool load_level) {
	save_game=p_data;

	if (load_level) {
		get_tree()->change_scene_to_file(save_game->current_level);
	}
}

Ref<ZSave_Game> Z::CreateGame() const {
	return memnew(ZSave_Game);
}

bool Z::SaveGame(String save_name) const {
	if (*save_game) {
		save_game->current_level=get_tree()->get_current_scene()->get_scene_file_path();
		ResourceSaver::save(save_game,L_GetPath_GlobalSave());
		return true;
	}
	return false;
}

Ref<ZSave_Game> Z::LoadGame(String save_name) const {
	if (ResourceLoader::exists(Z_File::GetDir_Saves()+save_name+".zSave")) {
		return ResourceLoader::load(Z_File::GetDir_Saves()+save_name+".zSave");
	}
	return Ref<ZSave_Game>();
}

