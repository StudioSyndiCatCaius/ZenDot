#pragma once

#include "scene/gui/tree.h"
#include "scene/gui/popup_menu.h"
#include "res/Zen_FileTypeConfig.h"

class Zen_FileTypeConfig;
// A Tree subclass that auto-populates itself from a filesystem path.
// Filtering, icons, colors, and right-click context menus are driven
// by an Array of Zen_FileTypeConfig resources.
class Zen_FileTree : public Tree {
    GDCLASS(Zen_FileTree, Tree);

public:
    // Emitted when the user selects a context-menu option on a tree item.
    // option_label : the string from Zen_FileTypeConfig::menu_options
    // file_path    : the full absolute path of the clicked file
    // option_index : index within that config's menu_options array
    //
    // Signal: menu_option_selected(option_label: String, file_path: String, option_index: int)

private:
    // --- Configurable properties ---
    String root_path;
    String name_filter;         // Include-only filter applied to file/folder names
    bool recursive = true;

    // Array<Ref<Zen_FileTypeConfig>> — only extensions present here are shown
    TypedArray<Zen_FileTypeConfig> file_type_configs;

    // --- Internal state ---
    PopupMenu *context_menu = nullptr;

    // The full path of the item that was right-clicked (used when the popup fires)
    String _context_item_path;

    // Build helpers
    void _populate_dir(TreeItem *p_parent, const String &p_dir_path);
    bool _dir_has_valid_files(const String &p_dir_path) const;
    bool _name_passes_filter(const String &p_name) const;
    Ref<Zen_FileTypeConfig> _get_config_for_ext(const String &p_ext) const;

    // Finds a folder config for a given directory name.
    // Prefers an is_folder config whose extension matches the dir's "extension".
    // Falls back to an is_folder config with no extension (default folder config).
    Ref<Zen_FileTypeConfig> _get_folder_config(const String &p_dir_name) const;

    // Walks the TreeItem hierarchy to find the item whose metadata matches p_path.
    // Returns nullptr if not found. Used by the expand/collapse API.
    TreeItem *_find_item_by_path(TreeItem *p_root, const String &p_path) const;

    // Recursively collects all directory items into p_out (files are skipped).
    void _collect_dir_items(TreeItem *p_item, List<TreeItem *> &p_out) const;

    // Callbacks
    void _on_context_menu_id_pressed(int p_id);
    void _on_context_menu_visibility_changed();
    void _on_item_activated();

protected:
    void gui_input(const Ref<InputEvent> &p_event) override;

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    Zen_FileTree();

    // --- Property accessors ---
    void set_root_path(const String &p_path);
    String get_root_path() const;

    void set_name_filter(const String &p_filter);
    String get_name_filter() const;

    void set_recursive(bool p_recursive);
    bool get_recursive() const;

    void set_file_type_configs(const TypedArray<Zen_FileTypeConfig> &p_configs);
    TypedArray<Zen_FileTypeConfig> get_file_type_configs() const;

    // --- Public API ---

    // Clears and rebuilds the entire tree from root_path.
    void rebuild_tree();

    // Set/get the collapsed state of a single directory by its absolute path.
    void set_expanded(const String &p_path, bool p_expanded);
    bool get_expanded(const String &p_path) const;

    // Bulk set/get — Dictionary maps absolute path (String) → expanded state (bool).
    // Useful for saving and restoring the full tree expansion state.
    void set_expanded_bulk(const Dictionary &p_states);
    Dictionary get_expanded_bulk() const;
};
