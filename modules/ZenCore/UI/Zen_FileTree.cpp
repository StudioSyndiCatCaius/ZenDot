#include "Zen_FileTree.h"

#include "core/config/project_settings.h"
#include "core/os/os.h"
#include "core/io/dir_access.h"

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

Zen_FileTree::Zen_FileTree() {
    // Create the PopupMenu that will appear on right-click.
    // We add it as a child so it is part of the scene tree and gets input.
    context_menu = memnew(PopupMenu);
    add_child(context_menu);

    // Wire up internal signals after we enter the tree (see _notification).
}

void Zen_FileTree::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            context_menu->connect("id_pressed",         callable_mp(this, &Zen_FileTree::_on_context_menu_id_pressed));
            context_menu->connect("visibility_changed", callable_mp(this, &Zen_FileTree::_on_context_menu_visibility_changed));
            // item_activated fires on double-click or Enter — the Tree equivalent of "open this file"
            connect("item_activated", callable_mp(this, &Zen_FileTree::_on_item_activated));
        } break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// GDScript / Editor bindings
// ---------------------------------------------------------------------------

void Zen_FileTree::_bind_methods() {
    // Properties
    ClassDB::bind_method(D_METHOD("set_root_path", "path"),         &Zen_FileTree::set_root_path);
    ClassDB::bind_method(D_METHOD("get_root_path"),                 &Zen_FileTree::get_root_path);

    ClassDB::bind_method(D_METHOD("set_name_filter", "filter"),     &Zen_FileTree::set_name_filter);
    ClassDB::bind_method(D_METHOD("get_name_filter"),               &Zen_FileTree::get_name_filter);

    ClassDB::bind_method(D_METHOD("set_recursive", "recursive"),    &Zen_FileTree::set_recursive);
    ClassDB::bind_method(D_METHOD("get_recursive"),                 &Zen_FileTree::get_recursive);

    ClassDB::bind_method(D_METHOD("set_file_type_configs", "configs"), &Zen_FileTree::set_file_type_configs);
    ClassDB::bind_method(D_METHOD("get_file_type_configs"),            &Zen_FileTree::get_file_type_configs);

    // Public methods
    ClassDB::bind_method(D_METHOD("rebuild_tree"), &Zen_FileTree::rebuild_tree);

    ClassDB::bind_method(D_METHOD("set_expanded", "path", "expanded"),  &Zen_FileTree::set_expanded);
    ClassDB::bind_method(D_METHOD("get_expanded", "path"),              &Zen_FileTree::get_expanded);
    ClassDB::bind_method(D_METHOD("set_expanded_bulk", "states"),       &Zen_FileTree::set_expanded_bulk);
    ClassDB::bind_method(D_METHOD("get_expanded_bulk"),                 &Zen_FileTree::get_expanded_bulk);

    // Signals
    // option_label: String, file_path: String, option_index: int
    ADD_SIGNAL(MethodInfo("menu_option_selected",
        PropertyInfo(Variant::STRING, "option_label"),
        PropertyInfo(Variant::STRING, "file_path"),
        PropertyInfo(Variant::INT,    "option_index")));

    ADD_SIGNAL(MethodInfo("option_menu_visibility_changed",
        PropertyInfo(Variant::BOOL, "visible")));

    // Emitted on double-click or Enter — the canonical "open this file" action
    ADD_SIGNAL(MethodInfo("file_selected",
        PropertyInfo(Variant::STRING, "file_path")));

    ADD_PROPERTY(PropertyInfo(Variant::STRING,      "root_path",         PROPERTY_HINT_DIR),  "set_root_path",         "get_root_path");
    ADD_PROPERTY(PropertyInfo(Variant::STRING,      "name_filter"),                           "set_name_filter",       "get_name_filter");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,        "recursive"),                             "set_recursive",         "get_recursive");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY,       "file_type_configs",
                              PROPERTY_HINT_ARRAY_TYPE, "Zen_FileTypeConfig"),               "set_file_type_configs", "get_file_type_configs");
}

// ---------------------------------------------------------------------------
// Property accessors
// ---------------------------------------------------------------------------

void Zen_FileTree::set_root_path(const String &p_path) {
    root_path = p_path;
}
String Zen_FileTree::get_root_path() const { return root_path; }

void Zen_FileTree::set_name_filter(const String &p_filter) {
    name_filter = p_filter;
}
String Zen_FileTree::get_name_filter() const { return name_filter; }

void Zen_FileTree::set_recursive(bool p_recursive) {
    recursive = p_recursive;
}
bool Zen_FileTree::get_recursive() const { return recursive; }

void Zen_FileTree::set_file_type_configs(const TypedArray<Zen_FileTypeConfig> &p_configs) {
    file_type_configs = p_configs;
}
TypedArray<Zen_FileTypeConfig> Zen_FileTree::get_file_type_configs() const { return file_type_configs; }

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void Zen_FileTree::rebuild_tree() {
    clear(); // Clears all TreeItems from the base Tree

    if (root_path.is_empty()) {
        return;
    }

    // Resolve the path (handles "res://", "user://", or absolute paths)
    String abs_path = ProjectSettings::get_singleton()->globalize_path(root_path);

    Ref<DirAccess> da = DirAccess::open(abs_path);
    if (da.is_null()) {
        WARN_PRINT("Zen_FileTree: Cannot open directory: " + abs_path);
        return;
    }

    // Create an invisible root so the tree looks clean
    TreeItem *root_item = create_item();
    root_item->set_text(0, root_path.get_file());
    root_item->set_metadata(0, abs_path);

    _populate_dir(root_item, abs_path);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void Zen_FileTree::_populate_dir(TreeItem *p_parent, const String &p_dir_path) {
    Ref<DirAccess> da = DirAccess::open(p_dir_path);
    if (da.is_null()) {
        return;
    }

    da->list_dir_begin();
    String entry = da->get_next();

    // Collect dirs and files separately so we can sort them
    Vector<String> subdirs;
    Vector<String> files;

    while (!entry.is_empty()) {
        if (entry == "." || entry == "..") {
            entry = da->get_next();
            continue;
        }

        if (da->current_is_dir()) {
            // Never apply name filter to folder names — a folder must be included
            // if ANY descendant file matches the filter (handled by _dir_has_valid_files).
            subdirs.push_back(entry);
        } else {
            String ext = entry.get_extension().to_lower();
            Ref<Zen_FileTypeConfig> cfg = _get_config_for_ext(ext);

            // Allow the file if:
            //   - configs list is empty (show everything), OR
            //   - a matching non-folder config exists
            if ((file_type_configs.is_empty() || cfg.is_valid()) && _name_passes_filter(entry)) {
                files.push_back(entry);
            }
        }

        entry = da->get_next();
    }

    da->list_dir_end();

    // Sort alphabetically
    subdirs.sort();
    files.sort();

    // Add sub-directories first
    for (const String &dir_name : subdirs) {
        String full_path = p_dir_path.path_join(dir_name);

        // Only create the TreeItem if the directory will actually have visible content.
        // This avoids touching TreeItem lifetime management, which Tree owns internally.
        if (recursive && !_dir_has_valid_files(full_path)) {
            continue;
        }

        TreeItem *dir_item = create_item(p_parent);
        dir_item->set_text(0, dir_name);
        dir_item->set_metadata(0, full_path);
        dir_item->set_metadata(1, true); // is_dir flag

        // Apply folder config styling if one matches
        Ref<Zen_FileTypeConfig> folder_cfg = _get_folder_config(dir_name);
        if (folder_cfg.is_valid()) {
            if (folder_cfg->get_icon().is_valid()) {
                dir_item->set_icon(0, folder_cfg->get_icon());
            }
            dir_item->set_custom_color(0, folder_cfg->get_color());
        }

        if (recursive) {
            _populate_dir(dir_item, full_path);
        }
    }

    // Add files
    for (const String &file_name : files) {
        String full_path = p_dir_path.path_join(file_name);
        String ext = file_name.get_extension().to_lower();
        Ref<Zen_FileTypeConfig> cfg = _get_config_for_ext(ext);

        TreeItem *file_item = create_item(p_parent);
        file_item->set_text(0, file_name);
        file_item->set_metadata(0, full_path);
        file_item->set_metadata(1, false); // is_dir flag

        if (cfg.is_valid()) {
            if (cfg->get_icon().is_valid()) {
                file_item->set_icon(0, cfg->get_icon());
            }
            file_item->set_custom_color(0, cfg->get_color());
        }
    }
}

// Returns true if p_dir_path contains at least one file that passes both the
// name filter and the extension allowlist. Recurses into subdirectories.
bool Zen_FileTree::_dir_has_valid_files(const String &p_dir_path) const {
    Ref<DirAccess> da = DirAccess::open(p_dir_path);
    if (da.is_null()) {
        return false;
    }

    da->list_dir_begin();
    String entry = da->get_next();

    while (!entry.is_empty()) {
        if (entry != "." && entry != "..") {
            if (da->current_is_dir()) {
                if (_dir_has_valid_files(p_dir_path.path_join(entry))) {
                    da->list_dir_end();
                    return true;
                }
            } else {
                String ext = entry.get_extension().to_lower();
                // Allow all files when no configs defined, otherwise require a matching file config
                bool allowed = file_type_configs.is_empty() || _get_config_for_ext(ext).is_valid();
                if (allowed && _name_passes_filter(entry)) {
                    da->list_dir_end();
                    return true;
                }
            }
        }
        entry = da->get_next();
    }

    da->list_dir_end();
    return false;
}

bool Zen_FileTree::_name_passes_filter(const String &p_name) const {
    if (name_filter.is_empty()) {
        return true; // No filter — everything passes
    }
    // Case-insensitive substring include filter
    return p_name.to_lower().contains(name_filter.to_lower());
}

Ref<Zen_FileTypeConfig> Zen_FileTree::_get_folder_config(const String &p_dir_name) const {
    String dir_ext = p_dir_name.get_extension().to_lower();
    Ref<Zen_FileTypeConfig> default_cfg; // is_folder=true, no extension

    for (int i = 0; i < file_type_configs.size(); i++) {
        Ref<Zen_FileTypeConfig> cfg = file_type_configs[i];
        if (!cfg.is_valid() || !cfg->get_is_folder()) {
            continue;
        }

        String cfg_ext = cfg->get_extension().to_lower();

        if (cfg_ext.is_empty()) {
            // This is a default folder config — keep as fallback
            default_cfg = cfg;
        } else if (cfg_ext == dir_ext) {
            // Specific match wins immediately
            return cfg;
        }
    }

    return default_cfg; // May be null if no folder configs exist at all
}

Ref<Zen_FileTypeConfig> Zen_FileTree::_get_config_for_ext(const String &p_ext) const {
    for (int i = 0; i < file_type_configs.size(); i++) {
        Ref<Zen_FileTypeConfig> cfg = file_type_configs[i];
        if (cfg.is_valid() && !cfg->get_is_folder() && cfg->get_extension().to_lower() == p_ext) {
            return cfg;
        }
    }
    return Ref<Zen_FileTypeConfig>();
}

// ---------------------------------------------------------------------------
// Expand / collapse API
// ---------------------------------------------------------------------------

TreeItem *Zen_FileTree::_find_item_by_path(TreeItem *p_item, const String &p_path) const {
    if (!p_item) {
        return nullptr;
    }

    if (String(p_item->get_metadata(0)) == p_path) {
        return p_item;
    }

    // Depth-first search through children
    TreeItem *child = p_item->get_first_child();
    while (child) {
        TreeItem *found = _find_item_by_path(child, p_path);
        if (found) {
            return found;
        }
        child = child->get_next();
    }

    return nullptr;
}

void Zen_FileTree::_collect_dir_items(TreeItem *p_item, List<TreeItem *> &p_out) const {
    if (!p_item) {
        return;
    }

    // An item is a directory if it has children OR its path has no file extension.
    // We check children because that's what matters for expand state.
    if (p_item->get_child_count() > 0) {
        p_out.push_back(p_item);
    }

    TreeItem *child = p_item->get_first_child();
    while (child) {
        _collect_dir_items(child, p_out);
        child = child->get_next();
    }
}

void Zen_FileTree::set_expanded(const String &p_path, bool p_expanded) {
    TreeItem *root = get_root();
    if (!root) {
        return;
    }

    TreeItem *item = _find_item_by_path(root, p_path);
    if (item) {
        item->set_collapsed(!p_expanded);
    }
}

bool Zen_FileTree::get_expanded(const String &p_path) const {
    TreeItem *root = get_root();
    if (!root) {
        return false;
    }

    TreeItem *item = _find_item_by_path(root, p_path);
    if (item) {
        return !item->is_collapsed();
    }

    return false; // Path not found — treat as collapsed
}

void Zen_FileTree::set_expanded_bulk(const Dictionary &p_states) {
    TreeItem *root = get_root();
    if (!root) {
        return;
    }

    Array paths = p_states.keys();
    for (int i = 0; i < paths.size(); i++) {
        String path = paths[i];
        bool expanded = p_states[path];

        TreeItem *item = _find_item_by_path(root, path);
        if (item) {
            item->set_collapsed(!expanded);
        }
    }
}

Dictionary Zen_FileTree::get_expanded_bulk() const {
    Dictionary result;

    TreeItem *root = get_root();
    if (!root) {
        return result;
    }

    // Collect all directory items and record their expanded state
    List<TreeItem *> dir_items;
    _collect_dir_items(root, dir_items);

    for (TreeItem *item : dir_items) {
        String path = item->get_metadata(0);
        result[path] = !item->is_collapsed();
    }

    return result;
}

// ---------------------------------------------------------------------------
// Right-click context menu
// ---------------------------------------------------------------------------

void Zen_FileTree::gui_input(const Ref<InputEvent> &p_event) {
    // Let the base Tree handle everything first (selection, scrolling, etc.)
    Tree::gui_input(p_event);

    Ref<InputEventMouseButton> mb = p_event;
    if (mb.is_null() || !mb->is_pressed() || mb->get_button_index() != MouseButton::RIGHT) {
        return;
    }

    TreeItem *item = get_item_at_position(mb->get_position());
    if (!item) {
        return;
    }

    print_line(vformat("Zen_FileTree: right click on item: %s", String(item->get_metadata(0))));

    String item_path = item->get_metadata(0);
    bool is_dir = bool(item->get_metadata(1));

    Ref<Zen_FileTypeConfig> cfg;
    if (is_dir) {
        cfg = _get_folder_config(item_path.get_file());
        if (!cfg.is_valid()) {
            print_line("Zen_FileTree: no folder config found for: " + item_path.get_file());
            return;
        }
    } else {
        String ext = item_path.get_extension().to_lower();
        cfg = _get_config_for_ext(ext);
        if (!cfg.is_valid()) {
            print_line("Zen_FileTree: no config found for extension: " + ext);
            return;
        }
    }

    PackedStringArray options = cfg->get_menu_options();
    if (options.is_empty()) {
        print_line("Zen_FileTree: config has no menu_options for: " + item_path.get_file());
        return;
    }

    _context_item_path = item_path;
    context_menu->clear();

    for (int i = 0; i < options.size(); i++) {
        context_menu->add_item(options[i], i);
    }

    context_menu->reset_size();

    // Use the real screen-space mouse position so the menu appears exactly
    // under the cursor regardless of window position, scale, or embedding.
    Point2i mouse_screen_pos = DisplayServer::get_singleton()->mouse_get_position();
    context_menu->set_position(mouse_screen_pos);
    context_menu->popup();
}

void Zen_FileTree::_on_item_activated() {
    TreeItem *item = get_selected();
    if (!item) {
        return;
    }

    String file_path = item->get_metadata(0);

    // Ignore directories — only emit for actual files
    if (file_path.get_extension().is_empty()) {
        return;
    }

    emit_signal("file_selected", file_path);
}

void Zen_FileTree::_on_context_menu_visibility_changed() {
    bool visible = context_menu->is_visible();
    print_line(vformat("Zen_FileTree: context menu visibility_changed → %s", visible ? "shown" : "hidden"));
    emit_signal("option_menu_visibility_changed", visible);
}

void Zen_FileTree::_on_context_menu_id_pressed(int p_id) {
    // Resolve the config the same way gui_input did — check the tree item's is_dir flag
    TreeItem *root = get_root();
    TreeItem *item = root ? _find_item_by_path(root, _context_item_path) : nullptr;

    Ref<Zen_FileTypeConfig> cfg;
    if (item && bool(item->get_metadata(1))) {
        cfg = _get_folder_config(_context_item_path.get_file());
    } else {
        cfg = _get_config_for_ext(_context_item_path.get_extension().to_lower());
    }

    if (!cfg.is_valid()) {
        return;
    }

    PackedStringArray options = cfg->get_menu_options();
    if (p_id < 0 || p_id >= options.size()) {
        return;
    }

    emit_signal("menu_option_selected", options[p_id], _context_item_path, p_id);
}
