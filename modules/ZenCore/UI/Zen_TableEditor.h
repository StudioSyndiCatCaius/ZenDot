#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/color_rect.h"
#include "scene/gui/control.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/option_button.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/texture_rect.h"
#include "scene/resources/style_box_flat.h"

#include "res/Zen_TableCellConfig.h"
#include "res/Zen_ParamType.h"

class Zen_TableCellConfig;
// -----------------------------------------------------------------------------
// Zen_TableEditor
//
// A Control that renders and edits an Array of Dictionaries as a styled table.
// Each column is driven by a Zen_TableCellConfig resource which provides the
// column_name (dictionary key), icon, accent colour, and param_type.
//
// The param_type determines the cell editor widget:
//   Zen_ParamType_String  → LineEdit   (respects max_length, placeholder)
//   Zen_ParamType_Number  → SpinBox    (int or float, optional min/max/step)
//   Zen_ParamType_Bool    → CheckBox   (labelled from true_label/false_label)
//   Zen_ParamType_Options → OptionButton (single-select from options list)
//   null / base           → LineEdit   (fallback)
//
// Properties:
//   data    — Array (of Dictionaries)           rows being edited
//   columns — Array (of Zen_TableCellConfig)    column definitions
//
// Signal:
//   data_changed(data: Array)
// -----------------------------------------------------------------------------
class Zen_TableEditor : public Control {
    GDCLASS(Zen_TableEditor, Control);

private:
    Array data;
    Array columns; // Array[Ref<Zen_TableCellConfig>]

    // ── Structural nodes ──────────────────────────────────────────────────────
    VBoxContainer   *_vbox           = nullptr;
    PanelContainer  *_header_panel   = nullptr;
    ScrollContainer *_scroll         = nullptr;
    VBoxContainer   *_rows_vbox      = nullptr;
    Button          *_add_row_button = nullptr;

    // ── Shared StyleBoxes ─────────────────────────────────────────────────────
    Ref<StyleBoxFlat> _sty_header;
    Ref<StyleBoxFlat> _sty_row_even;
    Ref<StyleBoxFlat> _sty_row_odd;
    Ref<StyleBoxFlat> _sty_cell_normal;
    Ref<StyleBoxFlat> _sty_cell_focus;

    // ── Layout constants ──────────────────────────────────────────────────────
    static constexpr int MIN_COL_WIDTH = 100;
    static constexpr int ROW_HEIGHT    = 28;
    static constexpr int DEL_COL_WIDTH = 28;
    static constexpr int ICON_SIZE     = 14;

    // ── Internals ─────────────────────────────────────────────────────────────
    void _init_styles();
    void _build_skeleton();
    void _rebuild_header();
    void _rebuild_rows();
    void _rebuild_table();

    Ref<Zen_TableCellConfig> _get_column(int p_idx) const;
    String                   _key_for_column(int p_idx) const;
    ColorRect               *_make_col_sep() const;

    // Builds the appropriate editor widget for a cell based on the column's
    // param_type. Connects its change signal to store the new value.
    Control *_make_cell_editor(const Ref<Zen_TableCellConfig> &p_cfg,
                               int p_row, const String &p_key,
                               const Variant &p_current);

    // Apply shared LineEdit styling
    void _style_line_edit(LineEdit *p_edit) const;

    // ── Cell-change callbacks (one per supported type) ────────────────────────
    void _on_string_changed(const String &p_text,   int p_row, const String &p_key);
    void _on_number_changed(double p_value,          int p_row, const String &p_key);
    void _on_bool_toggled  (bool p_pressed,          int p_row, const String &p_key);
    void _on_option_selected(int p_index,            int p_row, const String &p_key,
                             const PackedStringArray &p_options);

    // ── Row add / remove ──────────────────────────────────────────────────────
    void _on_add_row_pressed();
    void _on_remove_row_pressed(int p_row);

    // ── Column config change notification ────────────────────────────────────
    // Called when any connected Zen_TableCellConfig emits "changed".
    void _on_column_config_changed();
    void _connect_column_signals();
    void _disconnect_column_signals();

protected:
    static void _bind_methods();
    void        _notification(int p_what);

public:
    void  set_data(const Array &p_data);
    Array get_data() const;

    void  set_columns(const Array &p_columns);
    Array get_columns() const;

    Zen_TableEditor();
};
