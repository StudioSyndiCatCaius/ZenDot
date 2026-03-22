#ifndef Z_GAME_H
#define Z_GAME_H

#include "Save_Game.h"
#include "Save_Global.h"
#include "core/object/ref_counted.h"
#include "core/math/vector3.h"
#include "modules/luaAPI/src/classes/luaAPI.h"
#include "scene/main/node.h"

class Z : public Node {
	GDCLASS(Z, Node);

	static Z* singleton;
public:
	static Z* get_singleton();

	Z();
	~Z() override;
protected:
	static void _bind_methods();

public:
	void _enter_tree_deferred();
	void shutdown();
	void _notification(int p_what);

	// global variables
	Dictionary VARS;
	void set_VARS(const Dictionary &p_VARS) { VARS=p_VARS;}
	Dictionary get_VARS() const { return VARS; }

	Ref<LuaAPI> L;  // Ref<> keeps the refcount alive
	LuaAPI* get_lua() const { return L.ptr(); }


	// Getter/Setter for the resource
	Ref<ZSave_Game> save_game;
	Ref<ZSave_Game> get_save_game() const { return save_game; }
	Ref<ZSave_Global> save_global;
	Ref<ZSave_Global> get_save_global() const { return save_global; }

	void StartGame(const Ref<ZSave_Game> &p_data,bool load_level);
	Ref<ZSave_Game> CreateGame() const;

	bool SaveGame(String save_name) const;
	Ref<ZSave_Game> LoadGame(String save_name) const;
};

#endif
