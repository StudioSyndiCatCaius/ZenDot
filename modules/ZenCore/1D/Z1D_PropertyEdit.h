#pragma once

#include "scene/main/node.h"
#include "core/variant/dictionary.h"

// ---------------------------------------------------------------------------
// Z1D_PropertyEdit
// Links a dictionary property to one or more control nodes.
// When any linked control changes, the dictionary value is updated automatically.
// Optionally links a Label to display the property key name.
// ---------------------------------------------------------------------------
class Z1D_PropertyEdit : public Node {
    GDCLASS(Z1D_PropertyEdit, Node);

    Dictionary dict;
    StringName property;

    NodePath text_edit_path;    // TextEdit  — string values
    NodePath check_box_path;    // CheckBox  — bool values
    NodePath spin_box_path;     // SpinBox   — numeric values
    NodePath slider_path;       // HSlider / VSlider — numeric values
    NodePath label_path;        // (optional) Label to show the property key name

    void _connect_controls();
    void _disconnect_controls();
    void _set_dict_value(const Variant &p_value);
    void _update_label();

protected:
    static void _bind_methods();
    void        _notification(int p_what);

public:
    // Signal callbacks — public so ClassDB can bind them
    void _on_text_edit_changed();
    void _on_check_box_toggled(bool p_pressed);
    void _on_value_changed(double p_value);

    void       set_dict(const Dictionary &p_dict);
    Dictionary get_dict() const;

    void       set_property(const StringName &p_property);
    StringName get_property() const;

    void     set_text_edit_path(const NodePath &p_path);
    NodePath get_text_edit_path() const;

    void     set_check_box_path(const NodePath &p_path);
    NodePath get_check_box_path() const;

    void     set_spin_box_path(const NodePath &p_path);
    NodePath get_spin_box_path() const;

    void     set_slider_path(const NodePath &p_path);
    NodePath get_slider_path() const;

    void     set_label_path(const NodePath &p_path);
    NodePath get_label_path() const;
};
