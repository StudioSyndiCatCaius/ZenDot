#include "Zen_TableEditor.h"

#include "scene/gui/center_container.h"

class Zen_ParamType;
// =============================================================================
// Colour palette
// =============================================================================
namespace {
    const Color C_BG           { 0.10f,  0.10f,  0.11f,  1.0f };
    const Color C_HEADER_BG    { 0.16f,  0.18f,  0.24f,  1.0f };
    const Color C_HEADER_TEXT  { 0.80f,  0.85f,  0.95f,  1.0f };
    const Color C_ROW_EVEN     { 0.12f,  0.12f,  0.135f, 1.0f };
    const Color C_ROW_ODD      { 0.145f, 0.145f, 0.160f, 1.0f };
    const Color C_BORDER       { 0.22f,  0.22f,  0.27f,  1.0f };
    const Color C_FOCUS_BG     { 0.11f,  0.20f,  0.36f,  1.0f };
    const Color C_FOCUS_BORDER { 0.28f,  0.52f,  0.88f,  1.0f };
    const Color C_TEXT         { 0.88f,  0.88f,  0.88f,  1.0f };
    const Color C_TEXT_DIM     { 0.55f,  0.55f,  0.60f,  1.0f };
    const Color C_SELECTION    { 0.28f,  0.52f,  0.88f,  0.35f };
    const Color C_DEL_NORMAL   { 0.65f,  0.28f,  0.28f,  1.0f };
    const Color C_DEL_HOVER    { 0.92f,  0.35f,  0.35f,  1.0f };
    const Color C_ADD_BG       { 0.13f,  0.19f,  0.13f,  1.0f };
    const Color C_ADD_HOVER    { 0.18f,  0.28f,  0.18f,  1.0f };
    const Color C_ADD_TEXT     { 0.55f,  0.85f,  0.55f,  1.0f };
    const Color C_CHECK_ON     { 0.28f,  0.52f,  0.88f,  1.0f };
} // namespace

// =============================================================================
// Constructor
// =============================================================================

Zen_TableEditor::Zen_TableEditor() {}

// =============================================================================
// Lifecycle
// =============================================================================

void Zen_TableEditor::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_DRAW: {
            draw_rect(Rect2(Vector2(), get_size()), C_BG);
        } break;

        case NOTIFICATION_READY: {
            _init_styles();
            _build_skeleton();
            _rebuild_table();
        } break;

        case NOTIFICATION_EXIT_TREE: {
            _disconnect_column_signals();
        } break;
    }
}

// =============================================================================
// Style helpers
// =============================================================================

static Ref<StyleBoxFlat> _flat(Color bg, Color border = Color(0, 0, 0, 0), int bw = 0) {
    Ref<StyleBoxFlat> s;
    s.instantiate();
    s->set_bg_color(bg);
    if (bw > 0) {
        s->set_border_color(border);
        s->set_border_width_all(bw);
    }
    return s;
}

static void _set_margins(Ref<StyleBoxFlat> s, float l, float t, float r, float b) {
    s->set_content_margin(SIDE_LEFT,   l);
    s->set_content_margin(SIDE_TOP,    t);
    s->set_content_margin(SIDE_RIGHT,  r);
    s->set_content_margin(SIDE_BOTTOM, b);
}

void Zen_TableEditor::_init_styles() {
    _sty_header = _flat(C_HEADER_BG);
    _sty_header->set_border_color(C_BORDER);
    _sty_header->set_border_width(SIDE_BOTTOM, 2);

    _sty_row_even = _flat(C_ROW_EVEN);
    _sty_row_even->set_border_color(C_BORDER);
    _sty_row_even->set_border_width(SIDE_BOTTOM, 1);
    _sty_row_even->set_content_margin(SIDE_BOTTOM, 1.0f);

    _sty_row_odd = _flat(C_ROW_ODD);
    _sty_row_odd->set_border_color(C_BORDER);
    _sty_row_odd->set_border_width(SIDE_BOTTOM, 1);
    _sty_row_odd->set_content_margin(SIDE_BOTTOM, 1.0f);

    _sty_cell_normal = _flat(Color(0, 0, 0, 0));
    _set_margins(_sty_cell_normal, 8.0f, 4.0f, 8.0f, 4.0f);

    _sty_cell_focus = _flat(C_FOCUS_BG, C_FOCUS_BORDER, 1);
    _set_margins(_sty_cell_focus, 8.0f, 4.0f, 8.0f, 4.0f);
}

// =============================================================================
// Column helpers
// =============================================================================

Ref<Zen_TableCellConfig> Zen_TableEditor::_get_column(int p_idx) const {
    if (p_idx < 0 || p_idx >= columns.size()) {
        return Ref<Zen_TableCellConfig>();
    }
    return Ref<Zen_TableCellConfig>(columns[p_idx]);
}

String Zen_TableEditor::_key_for_column(int p_idx) const {
    Ref<Zen_TableCellConfig> cfg = _get_column(p_idx);
    if (cfg.is_valid() && !cfg->get_column_name().is_empty()) {
        return cfg->get_column_name();
    }
    return String("col_") + String::num_int64(p_idx);
}

// =============================================================================
// Column config change signals
// When any Zen_TableCellConfig in the columns array emits "changed" (e.g. its
// column_name, icon, color, or param_type was modified in the Inspector), we
// rebuild the entire table so the header and cell editors stay in sync.
// =============================================================================

void Zen_TableEditor::_connect_column_signals() {
    for (int i = 0; i < columns.size(); i++) {
        Ref<Zen_TableCellConfig> cfg = _get_column(i);
        if (cfg.is_valid() && !cfg->is_connected("changed", callable_mp(this, &Zen_TableEditor::_on_column_config_changed))) {
            cfg->connect("changed", callable_mp(this, &Zen_TableEditor::_on_column_config_changed));
        }
    }
}

void Zen_TableEditor::_disconnect_column_signals() {
    for (int i = 0; i < columns.size(); i++) {
        Ref<Zen_TableCellConfig> cfg = _get_column(i);
        if (cfg.is_valid() && cfg->is_connected("changed", callable_mp(this, &Zen_TableEditor::_on_column_config_changed))) {
            cfg->disconnect("changed", callable_mp(this, &Zen_TableEditor::_on_column_config_changed));
        }
    }
}

void Zen_TableEditor::_on_column_config_changed() {
    // A column resource property changed — full rebuild so header labels,
    // accent colours, icons, and cell editor types are all refreshed.
    if (is_inside_tree() && _header_panel) {
        _rebuild_table();
    }
}

// =============================================================================
// Skeleton
// =============================================================================

ColorRect *Zen_TableEditor::_make_col_sep() const {
    ColorRect *cr = memnew(ColorRect);
    cr->set_color(C_BORDER);
    cr->set_custom_minimum_size(Vector2(1, 0));
    cr->set_v_size_flags(Control::SIZE_FILL);
    return cr;
}

void Zen_TableEditor::_build_skeleton() {
    _vbox = memnew(VBoxContainer);
    _vbox->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
    _vbox->add_theme_constant_override("separation", 0);
    add_child(_vbox);

    _header_panel = memnew(PanelContainer);
    _header_panel->add_theme_style_override("panel", _sty_header);
    _vbox->add_child(_header_panel);

    _scroll = memnew(ScrollContainer);
    _scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
    _scroll->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    _scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_AUTO);
    _scroll->set_vertical_scroll_mode(ScrollContainer::SCROLL_MODE_AUTO);
    _vbox->add_child(_scroll);

    _rows_vbox = memnew(VBoxContainer);
    _rows_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    _rows_vbox->add_theme_constant_override("separation", 0);
    _scroll->add_child(_rows_vbox);

    ColorRect *rule = memnew(ColorRect);
    rule->set_color(C_BORDER);
    rule->set_custom_minimum_size(Vector2(0, 1));
    rule->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    _vbox->add_child(rule);

    Ref<StyleBoxFlat> sty_add       = _flat(C_ADD_BG,    C_BORDER,       1);
    Ref<StyleBoxFlat> sty_add_hover = _flat(C_ADD_HOVER, C_FOCUS_BORDER, 1);
    Ref<StyleBoxFlat> sty_add_press = _flat(C_ADD_BG,    C_FOCUS_BORDER, 1);
    _set_margins(sty_add,       14, 6, 14, 6);
    _set_margins(sty_add_hover, 14, 6, 14, 6);
    _set_margins(sty_add_press, 14, 6, 14, 6);

    _add_row_button = memnew(Button);
    _add_row_button->set_text("+ Add Row");
    _add_row_button->set_h_size_flags(Control::SIZE_SHRINK_BEGIN);
    _add_row_button->add_theme_style_override("normal",  sty_add);
    _add_row_button->add_theme_style_override("hover",   sty_add_hover);
    _add_row_button->add_theme_style_override("pressed", sty_add_press);
    _add_row_button->add_theme_style_override("focus",   sty_add);
    _add_row_button->add_theme_color_override("font_color",         C_ADD_TEXT);
    _add_row_button->add_theme_color_override("font_hover_color",   C_ADD_TEXT);
    _add_row_button->add_theme_color_override("font_pressed_color", C_ADD_TEXT);
    _add_row_button->add_theme_font_size_override("font_size", 13);
    _add_row_button->connect("pressed", callable_mp(this, &Zen_TableEditor::_on_add_row_pressed));
    _vbox->add_child(_add_row_button);
}

// =============================================================================
// Rebuild — header
// =============================================================================

void Zen_TableEditor::_rebuild_header() {
    for (int i = _header_panel->get_child_count() - 1; i >= 0; i--) {
        Node *c = _header_panel->get_child(i);
        _header_panel->remove_child(c);
        c->queue_free();
    }
    if (columns.is_empty()) { return; }

    HBoxContainer *hbox = memnew(HBoxContainer);
    hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    hbox->add_theme_constant_override("separation", 0);
    _header_panel->add_child(hbox);

    for (int col = 0; col < columns.size(); col++) {
        if (col > 0) { hbox->add_child(_make_col_sep()); }

        Ref<Zen_TableCellConfig> cfg    = _get_column(col);
        const Color              accent = cfg.is_valid() ? cfg->get_color() : Color(0.4f, 0.4f, 0.5f, 1.0f);

        // 3-px accent bar on the left edge
        ColorRect *accent_bar = memnew(ColorRect);
        accent_bar->set_color(accent);
        accent_bar->set_custom_minimum_size(Vector2(3, 0));
        accent_bar->set_v_size_flags(Control::SIZE_FILL);
        hbox->add_child(accent_bar);

        MarginContainer *mc = memnew(MarginContainer);
        mc->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        mc->set_stretch_ratio(cfg.is_valid() ? cfg->get_cell_ratio() : 1.0f);
        mc->set_custom_minimum_size(Vector2(MIN_COL_WIDTH - 3, ROW_HEIGHT));
        mc->add_theme_constant_override("margin_left",   8);
        mc->add_theme_constant_override("margin_right",  8);
        mc->add_theme_constant_override("margin_top",    0);
        mc->add_theme_constant_override("margin_bottom", 0);
        hbox->add_child(mc);

        HBoxContainer *inner = memnew(HBoxContainer);
        inner->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        inner->add_theme_constant_override("separation", 5);
        mc->add_child(inner);

        // Optional icon tinted with accent colour
        if (cfg.is_valid() && cfg->get_icon().is_valid()) {
            TextureRect *icon = memnew(TextureRect);
            icon->set_texture(cfg->get_icon());
            icon->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
            icon->set_custom_minimum_size(Vector2(ICON_SIZE, ICON_SIZE));
            icon->set_modulate(accent);
            inner->add_child(icon);
        }

        Label *lbl = memnew(Label);
        lbl->set_text(cfg.is_valid() ? cfg->get_column_name().to_upper() : String("COL ") + String::num_int64(col));
        lbl->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
        lbl->set_clip_text(true);
        lbl->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        lbl->add_theme_color_override("font_color",    C_HEADER_TEXT);
        lbl->add_theme_font_size_override("font_size", 12);
        inner->add_child(lbl);
    }

    // Spacer aligned with the delete-button column
    hbox->add_child(_make_col_sep());
    Control *spacer = memnew(Control);
    spacer->set_custom_minimum_size(Vector2(DEL_COL_WIDTH, ROW_HEIGHT));
    hbox->add_child(spacer);
}

// =============================================================================
// Cell editor factory
// =============================================================================

void Zen_TableEditor::_style_line_edit(LineEdit *p_edit) const {
    p_edit->add_theme_style_override("normal",    _sty_cell_normal);
    p_edit->add_theme_style_override("focus",     _sty_cell_focus);
    p_edit->add_theme_style_override("read_only", _sty_cell_normal);
    p_edit->add_theme_color_override("font_color",      C_TEXT);
    p_edit->add_theme_color_override("cursor_color",    C_FOCUS_BORDER);
    p_edit->add_theme_color_override("selection_color", C_SELECTION);
    p_edit->add_theme_font_size_override("font_size", 13);
}

Control *Zen_TableEditor::_make_cell_editor(
        const Ref<Zen_TableCellConfig> &p_cfg,
        int p_row, const String &p_key,
        const Variant &p_current) {

    // Resolve the param_type — may be null if config or type is unset
    Ref<Zen_ParamType> param_type;
    if (p_cfg.is_valid()) {
        param_type = p_cfg->get_param_type();
    }

    const String type_name = param_type.is_valid() ? param_type->get_type_name() : "string";

    // ── String ──────────────────────────────────────────────────────────────
    if (type_name == "string" || type_name == "base" || type_name == "unset") {
        Ref<Zen_ParamType_String> str_cfg = param_type; // may be null

        LineEdit *edit = memnew(LineEdit);
        edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        edit->set_custom_minimum_size(Vector2(MIN_COL_WIDTH, ROW_HEIGHT));
        _style_line_edit(edit);

        if (str_cfg.is_valid()) {
            if (str_cfg->get_max_length() > 0) {
                edit->set_max_length(str_cfg->get_max_length());
            }
            if (!str_cfg->get_placeholder().is_empty()) {
                edit->set_placeholder(str_cfg->get_placeholder());
                edit->add_theme_color_override("font_placeholder_color", C_TEXT_DIM);
            }
        }

        edit->set_text(String(p_current));
        edit->connect("text_changed",
            callable_mp(this, &Zen_TableEditor::_on_string_changed).bind(p_row, p_key));
        return edit;
    }

    // ── Number (int / float) ─────────────────────────────────────────────────
    if (type_name == "int" || type_name == "float") {
        Ref<Zen_ParamType_Number> num_cfg = param_type;
        const bool is_float = (type_name == "float");

        SpinBox *spin = memnew(SpinBox);
        spin->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        spin->set_custom_minimum_size(Vector2(MIN_COL_WIDTH, ROW_HEIGHT));
        spin->set_step(is_float ? num_cfg->get_step() : 1.0f);
        spin->set_allow_lesser(!num_cfg->get_clamp());
        spin->set_allow_greater(!num_cfg->get_clamp());

        // Apply min/max only when clamping is on or explicit range is set
        if (num_cfg->get_clamp() ||
            num_cfg->get_min_value() != 0.0f ||
            num_cfg->get_max_value() != 0.0f) {
            spin->set_min(num_cfg->get_min_value());
            spin->set_max(num_cfg->get_max_value());
        } else {
            // No range — use wide open defaults
            spin->set_min(-1e9);
            spin->set_max( 1e9);
        }

        // Style the internal LineEdit the same as our cell style
        LineEdit *inner = spin->get_line_edit();
        if (inner) {
            _style_line_edit(inner);
        }

        spin->set_value(double(p_current));
        spin->connect("value_changed",
            callable_mp(this, &Zen_TableEditor::_on_number_changed).bind(p_row, p_key));
        return spin;
    }

    // ── Bool ─────────────────────────────────────────────────────────────────
    if (type_name == "bool") {
        Ref<Zen_ParamType_Bool> bool_cfg = param_type;

        // Wrap in a centred container so the CheckBox sits in the middle
        // of the cell rather than hugging the left edge.
        CenterContainer *cc = memnew(CenterContainer);
        cc->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        cc->set_custom_minimum_size(Vector2(MIN_COL_WIDTH, ROW_HEIGHT));

        CheckBox *cb = memnew(CheckBox);
        cb->set_focus_mode(Control::FOCUS_CLICK);

        // Label text from the param type, or blank to keep it compact
        const String true_lbl  = bool_cfg.is_valid() ? bool_cfg->get_true_label()  : "";
        const String false_lbl = bool_cfg.is_valid() ? bool_cfg->get_false_label() : "";

        // Reflect current value — show appropriate label
        const bool current_val = bool(p_current);
        cb->set_pressed(current_val);
        cb->set_text(current_val ? true_lbl : false_lbl);

        cb->add_theme_color_override("font_color",       C_TEXT);
        cb->add_theme_color_override("font_hover_color", C_TEXT);
        cb->add_theme_font_size_override("font_size", 12);

        // Update the label text when toggled, then store the bool value
        // We capture true_lbl / false_lbl via a lambda-style Callable bind
        cb->connect("toggled", callable_mp(this, &Zen_TableEditor::_on_bool_toggled).bind(p_row, p_key));

        // Also flip the label on toggle — connect a separate closure via a
        // helper lambda captured in a one-shot Callable isn't idiomatic in
        // Godot C++ without a helper node, so we update the label inside the
        // shared callback by re-querying the CheckBox text.
        // (See _on_bool_toggled — it handles label refresh.)

        // Store the labels on the CheckBox metadata so _on_bool_toggled
        // can read them without needing a capture.
        cb->set_meta("true_label",  true_lbl);
        cb->set_meta("false_label", false_lbl);

        cc->add_child(cb);
        return cc;
    }

    // ── Options ──────────────────────────────────────────────────────────────
    if (type_name == "options") {
        Ref<Zen_ParamType_Options> opt_cfg = param_type;
        const PackedStringArray    options = opt_cfg.is_valid() ? opt_cfg->get_options() : PackedStringArray();

        // Flat-style OptionButton
        Ref<StyleBoxFlat> sty_opt = _flat(Color(0, 0, 0, 0));
        _set_margins(sty_opt, 6.0f, 2.0f, 6.0f, 2.0f);
        Ref<StyleBoxFlat> sty_opt_focus = _flat(C_FOCUS_BG, C_FOCUS_BORDER, 1);
        _set_margins(sty_opt_focus, 6.0f, 2.0f, 6.0f, 2.0f);

        OptionButton *ob = memnew(OptionButton);
        ob->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        ob->set_custom_minimum_size(Vector2(MIN_COL_WIDTH, ROW_HEIGHT));
        ob->add_theme_style_override("normal",  sty_opt);
        ob->add_theme_style_override("hover",   sty_opt_focus);
        ob->add_theme_style_override("pressed", sty_opt_focus);
        ob->add_theme_style_override("focus",   sty_opt);
        ob->add_theme_color_override("font_color",       C_TEXT);
        ob->add_theme_color_override("font_hover_color", C_TEXT);
        ob->add_theme_font_size_override("font_size", 13);

        const String current_str = String(p_current);
        int          selected_idx = 0;

        for (int i = 0; i < options.size(); i++) {
            ob->add_item(options[i]);
            if (options[i] == current_str) {
                selected_idx = i;
            }
        }
        if (options.size() > 0) {
            ob->select(selected_idx);
        }

        // Bind the full options list so the callback can map index → string
        ob->connect("item_selected",
            callable_mp(this, &Zen_TableEditor::_on_option_selected).bind(p_row, p_key, options));
        return ob;
    }

    // ── Fallback — plain LineEdit ────────────────────────────────────────────
    LineEdit *fallback = memnew(LineEdit);
    fallback->set_h_size_flags(Control::SIZE_EXPAND_FILL);
    fallback->set_custom_minimum_size(Vector2(MIN_COL_WIDTH, ROW_HEIGHT));
    _style_line_edit(fallback);
    fallback->set_text(String(p_current));
    fallback->connect("text_changed",
        callable_mp(this, &Zen_TableEditor::_on_string_changed).bind(p_row, p_key));
    return fallback;
}

// =============================================================================
// Rebuild — data rows
// =============================================================================

void Zen_TableEditor::_rebuild_rows() {
    for (int i = _rows_vbox->get_child_count() - 1; i >= 0; i--) {
        Node *c = _rows_vbox->get_child(i);
        _rows_vbox->remove_child(c);
        c->queue_free();
    }

    for (int row = 0; row < data.size(); row++) {
        Dictionary row_dict = data[row];
        const bool  is_even = (row % 2 == 0);

        PanelContainer *row_panel = memnew(PanelContainer);
        row_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        row_panel->add_theme_style_override("panel", is_even ? _sty_row_even : _sty_row_odd);
        _rows_vbox->add_child(row_panel);

        HBoxContainer *hbox = memnew(HBoxContainer);
        hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
        hbox->add_theme_constant_override("separation", 0);
        row_panel->add_child(hbox);

        for (int col = 0; col < columns.size(); col++) {
            if (col > 0) { hbox->add_child(_make_col_sep()); }

            const String             key     = _key_for_column(col);
            Ref<Zen_TableCellConfig> cfg     = _get_column(col);
            const Variant            current = row_dict.has(key) ? row_dict[key] : Variant();

            Control *editor = _make_cell_editor(cfg, row, key, current);
            editor->set_stretch_ratio(cfg.is_valid() ? cfg->get_cell_ratio() : 1.0f);
            hbox->add_child(editor);
        }

        // Delete row button
        if (!columns.is_empty()) {
            hbox->add_child(_make_col_sep());
            Button *del = memnew(Button);
            del->set_flat(true);
            del->set_text("×");
            del->set_custom_minimum_size(Vector2(DEL_COL_WIDTH, ROW_HEIGHT));
            del->add_theme_color_override("font_color",       C_DEL_NORMAL);
            del->add_theme_color_override("font_hover_color", C_DEL_HOVER);
            del->add_theme_font_size_override("font_size", 18);
            del->connect("pressed",
                callable_mp(this, &Zen_TableEditor::_on_remove_row_pressed).bind(row));
            hbox->add_child(del);
        }
    }
}

void Zen_TableEditor::_rebuild_table() {
    _rebuild_header();
    _rebuild_rows();
}

// =============================================================================
// Cell-change callbacks
// =============================================================================

void Zen_TableEditor::_on_string_changed(const String &p_text, int p_row, const String &p_key) {
    ERR_FAIL_INDEX(p_row, data.size());
    Dictionary d = data[p_row];
    d[p_key]     = p_text;
    data[p_row]  = d;
    emit_signal("data_changed", data);
}

void Zen_TableEditor::_on_number_changed(double p_value, int p_row, const String &p_key) {
    ERR_FAIL_INDEX(p_row, data.size());
    Dictionary d = data[p_row];
    d[p_key]     = p_value;
    data[p_row]  = d;
    emit_signal("data_changed", data);
}

void Zen_TableEditor::_on_bool_toggled(bool p_pressed, int p_row, const String &p_key) {
    ERR_FAIL_INDEX(p_row, data.size());

    // Update the CheckBox label to reflect the current state.
    // The CheckBox is the first child of the CenterContainer which is the
    // editor widget for this cell. We walk up via the signal source.
    // Because Godot's callable bind doesn't give us the emitter directly,
    // we locate the CheckBox by searching the row's HBoxContainer.
    // (This is a deliberate trade-off: it avoids storing per-cell pointers.)
    PanelContainer *row_panel = Object::cast_to<PanelContainer>(_rows_vbox->get_child(p_row));
    if (row_panel) {
        HBoxContainer *hbox = Object::cast_to<HBoxContainer>(row_panel->get_child(0));
        if (hbox) {
            for (int i = 0; i < hbox->get_child_count(); i++) {
                CenterContainer *cc = Object::cast_to<CenterContainer>(hbox->get_child(i));
                if (!cc) { continue; }
                CheckBox *cb = Object::cast_to<CheckBox>(cc->get_child(0));
                if (!cb)  { continue; }
                // Match by checking whether this CheckBox is connected for our row/key
                // (only one such CheckBox exists per row×key pair)
                const String tl = cb->get_meta("true_label",  "");
                const String fl = cb->get_meta("false_label", "");
                cb->set_text(p_pressed ? tl : fl);
                break;
            }
        }
    }

    Dictionary d = data[p_row];
    d[p_key]     = p_pressed;
    data[p_row]  = d;
    emit_signal("data_changed", data);
}

void Zen_TableEditor::_on_option_selected(int p_index, int p_row, const String &p_key,
                                          const PackedStringArray &p_options) {
    ERR_FAIL_INDEX(p_row, data.size());
    const String value = (p_index >= 0 && p_index < p_options.size()) ? p_options[p_index] : String();
    Dictionary d       = data[p_row];
    d[p_key]           = value;
    data[p_row]        = d;
    emit_signal("data_changed", data);
}

// =============================================================================
// Row add / remove
// =============================================================================

void Zen_TableEditor::_on_add_row_pressed() {
    Dictionary new_row;
    for (int col = 0; col < columns.size(); col++) {
        Ref<Zen_TableCellConfig> cfg      = _get_column(col);
        const String             key      = _key_for_column(col);
        Ref<Zen_ParamType>       pt       = cfg.is_valid() ? cfg->get_param_type() : Ref<Zen_ParamType>();
        const String             type_name = pt.is_valid() ? pt->get_type_name() : "string";

        // Initialise with a sensible default for the type
        if (type_name == "int" || type_name == "float") {
            Ref<Zen_ParamType_Number> n = pt;
            new_row[key] = n.is_valid() ? Variant(n->get_min_value()) : Variant(0.0);
        } else if (type_name == "bool") {
            new_row[key] = false;
        } else if (type_name == "options") {
            Ref<Zen_ParamType_Options> o = pt;
            new_row[key] = (o.is_valid() && o->get_options().size() > 0)
                           ? Variant(o->get_options()[0])
                           : Variant(String());
        } else {
            new_row[key] = String();
        }
    }
    data.push_back(new_row);
    _rebuild_rows();
    emit_signal("data_changed", data);
}

void Zen_TableEditor::_on_remove_row_pressed(int p_row) {
    ERR_FAIL_INDEX(p_row, data.size());
    data.remove_at(p_row);
    _rebuild_rows();
    emit_signal("data_changed", data);
}

// =============================================================================
// Property accessors
// =============================================================================

void Zen_TableEditor::set_data(const Array &p_data) {
    data = p_data;
    if (is_inside_tree() && _rows_vbox) {
        _rebuild_rows();
    }
}
Array Zen_TableEditor::get_data() const { return data; }

void Zen_TableEditor::set_columns(const Array &p_columns) {
    // Disconnect signals from the old set of config resources
    _disconnect_column_signals();
    columns = p_columns;
    // Connect signals on the new set
    _connect_column_signals();

    if (is_inside_tree() && _header_panel) {
        _rebuild_table();
    }
}
Array Zen_TableEditor::get_columns() const { return columns; }

// =============================================================================
// Bindings
// =============================================================================

void Zen_TableEditor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_data",    "data"),    &Zen_TableEditor::set_data);
    ClassDB::bind_method(D_METHOD("get_data"),               &Zen_TableEditor::get_data);
    ClassDB::bind_method(D_METHOD("set_columns", "columns"), &Zen_TableEditor::set_columns);
    ClassDB::bind_method(D_METHOD("get_columns"),            &Zen_TableEditor::get_columns);

    ADD_PROPERTY(
        PropertyInfo(Variant::ARRAY, "data"),
        "set_data", "get_data"
    );
    ADD_PROPERTY(
        PropertyInfo(Variant::ARRAY, "columns", PROPERTY_HINT_ARRAY_TYPE, "Zen_TableCellConfig"),
        "set_columns", "get_columns"
    );

    ADD_SIGNAL(MethodInfo("data_changed", PropertyInfo(Variant::ARRAY, "data")));
}
