#include "Z.h"

#include "core/config/project_settings.h"
#include "scene/main/node.h"


Z* Z::singleton = nullptr;

Z* Z::get_singleton() {
	return singleton;
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
}
Z::~Z() {
	L.unref();  // let the Ref<> handle cleanup
	singleton = nullptr;
}

void Z::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_VARS", "VARS"), &Z::set_VARS);
	ClassDB::bind_method(D_METHOD("get_VARS"), &Z::get_VARS);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "VARS", PROPERTY_HINT_DICTIONARY_TYPE, "", PROPERTY_USAGE_DEFAULT, "Dictionary"), "set_VARS", "get_VARS");
	ClassDB::bind_method(D_METHOD("get_lua"), &Z::get_lua);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "L", PROPERTY_HINT_RESOURCE_TYPE, "LuaAPI"), "", "get_lua");
}

