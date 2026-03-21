#pragma once

#include "core/io/resource.h"
#include "scene/resources/texture.h"

// A Resource that configures how a specific file extension is displayed
// and interacted with inside a FileTree node.
class Zen_FileTypeConfig : public Resource {
	GDCLASS(Zen_FileTypeConfig, Resource);

	String extension;
	Ref<Texture2D> icon;
	Color color = Color(1, 1, 1, 1);
	PackedStringArray menu_options;

	// If true, this config matches directories rather than files.
	// - extension set   → matches folders whose name has that "extension" (e.g. "maps.flow")
	// - extension empty → default folder config, applied to any folder with no specific match
	bool is_folder = false;

protected:
	static void _bind_methods();

public:
	void set_extension(const String &p_ext);
	String get_extension() const;

	void set_icon(const Ref<Texture2D> &p_icon);
	Ref<Texture2D> get_icon() const;

	void set_color(const Color &p_color);
	Color get_color() const;

	void set_menu_options(const PackedStringArray &p_options);
	PackedStringArray get_menu_options() const;

	void set_is_folder(bool p_is_folder);
	bool get_is_folder() const;
};
