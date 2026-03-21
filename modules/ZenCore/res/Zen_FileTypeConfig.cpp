#include "Zen_FileTypeConfig.h"

void Zen_FileTypeConfig::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_extension", "extension"), &Zen_FileTypeConfig::set_extension);
    ClassDB::bind_method(D_METHOD("get_extension"), &Zen_FileTypeConfig::get_extension);

    ClassDB::bind_method(D_METHOD("set_icon", "icon"), &Zen_FileTypeConfig::set_icon);
    ClassDB::bind_method(D_METHOD("get_icon"), &Zen_FileTypeConfig::get_icon);

    ClassDB::bind_method(D_METHOD("set_color", "color"), &Zen_FileTypeConfig::set_color);
    ClassDB::bind_method(D_METHOD("get_color"), &Zen_FileTypeConfig::get_color);

    ClassDB::bind_method(D_METHOD("set_menu_options", "options"), &Zen_FileTypeConfig::set_menu_options);
    ClassDB::bind_method(D_METHOD("get_menu_options"), &Zen_FileTypeConfig::get_menu_options);

    ClassDB::bind_method(D_METHOD("set_is_folder", "is_folder"), &Zen_FileTypeConfig::set_is_folder);
    ClassDB::bind_method(D_METHOD("get_is_folder"), &Zen_FileTypeConfig::get_is_folder);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "extension"), "set_extension", "get_extension");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,   "is_folder"), "set_is_folder", "get_is_folder");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_icon", "get_icon");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR,  "color"), "set_color", "get_color");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "menu_options"), "set_menu_options", "get_menu_options");
}

// --- extension ---
void Zen_FileTypeConfig::set_extension(const String &p_ext) {
    // Strip a leading dot if the user accidentally included one
    extension = p_ext.trim_prefix(".");
}
String Zen_FileTypeConfig::get_extension() const { return extension; }

// --- icon ---
void Zen_FileTypeConfig::set_icon(const Ref<Texture2D> &p_icon) { icon = p_icon; }
Ref<Texture2D> Zen_FileTypeConfig::get_icon() const { return icon; }

// --- color ---
void Zen_FileTypeConfig::set_color(const Color &p_color) { color = p_color; }
Color Zen_FileTypeConfig::get_color() const { return color; }

// --- menu_options ---
void Zen_FileTypeConfig::set_menu_options(const PackedStringArray &p_options) { menu_options = p_options; }
PackedStringArray Zen_FileTypeConfig::get_menu_options() const { return menu_options; }

// --- is_folder ---
void Zen_FileTypeConfig::set_is_folder(bool p_is_folder) { is_folder = p_is_folder; }
bool Zen_FileTypeConfig::get_is_folder() const { return is_folder; }
