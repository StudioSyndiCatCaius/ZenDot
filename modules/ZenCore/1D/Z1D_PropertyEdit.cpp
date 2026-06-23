#include "Z1D_PropertyEdit.h"

#include "scene/gui/text_edit.h"
#include "scene/gui/check_box.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/slider.h"
#include "scene/gui/label.h"

// ---------------------------------------------------------------------------
// Bind
// ---------------------------------------------------------------------------

void Z1D_PropertyEdit::_bind_methods() {
    // Callbacks
    ClassDB::bind_method(D_METHOD("_on_text_edit_changed"),            &Z1D_PropertyEdit::_on_text_edit_changed);
    ClassDB::bind_method(D_METHOD("_on_check_box_toggled", "pressed"), &Z1D_PropertyEdit::_on_check_box_toggled);
    ClassDB::bind_method(D_METHOD("_on_value_changed",     "value"),   &Z1D_PropertyEdit::_on_value_changed);

    // Dictionary + property
    ClassDB::bind_method(D_METHOD("set_dict",     "dict"),     &Z1D_PropertyEdit::set_dict);
    ClassDB::bind_method(D_METHOD("get_dict"),                 &Z1D_PropertyEdit::get_dict);
    ClassDB::bind_method(D_METHOD("set_property", "property"), &Z1D_PropertyEdit::set_property);
    ClassDB::bind_method(D_METHOD("get_property"),             &Z1D_PropertyEdit::get_property);

    // Node paths
    ClassDB::bind_method(D_METHOD("set_text_edit_path", "path"), &Z1D_PropertyEdit::set_text_edit_path);
    ClassDB::bind_method(D_METHOD("get_text_edit_path"),         &Z1D_PropertyEdit::get_text_edit_path);

    ClassDB::bind_method(D_METHOD("set_check_box_path", "path"), &Z1D_PropertyEdit::set_check_box_path);
    ClassDB::bind_method(D_METHOD("get_check_box_path"),         &Z1D_PropertyEdit::get_check_box_path);

    ClassDB::bind_method(D_METHOD("set_spin_box_path",  "path"), &Z1D_PropertyEdit::set_spin_box_path);
    ClassDB::bind_method(D_METHOD("get_spin_box_path"),          &Z1D_PropertyEdit::get_spin_box_path);

    ClassDB::bind_method(D_METHOD("set_slider_path",    "path"), &Z1D_PropertyEdit::set_slider_path);
    ClassDB::bind_method(D_METHOD("get_slider_path"),            &Z1D_PropertyEdit::get_slider_path);

    ClassDB::bind_method(D_METHOD("set_label_path",     "path"), &Z1D_PropertyEdit::set_label_path);
    ClassDB::bind_method(D_METHOD("get_label_path"),             &Z1D_PropertyEdit::get_label_path);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY,  "dict"),            "set_dict",            "get_dict");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "property"),        "set_property",        "get_property");
    ADD_GROUP("Linked Controls", "");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,   "text_edit_path",  PROPERTY_HINT_NODE_PATH_VALID_TYPES, "TextEdit"),  "set_text_edit_path", "get_text_edit_path");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,   "check_box_path",  PROPERTY_HINT_NODE_PATH_VALID_TYPES, "CheckBox"),  "set_check_box_path", "get_check_box_path");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,   "spin_box_path",   PROPERTY_HINT_NODE_PATH_VALID_TYPES, "SpinBox"),   "set_spin_box_path",  "get_spin_box_path");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,   "slider_path",     PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Slider"),    "set_slider_path",    "get_slider_path");
    ADD_GROUP("Optional", "");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH,   "label_path",      PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Label"),     "set_label_path",     "get_label_path");
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void Z1D_PropertyEdit::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY:
            _connect_controls();
            break;
        case NOTIFICATION_EXIT_TREE:
            _disconnect_controls();
            break;
    }
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void Z1D_PropertyEdit::_connect_controls() {
    if (!is_inside_tree()) { return; }

    if (!text_edit_path.is_empty()) {
        if (TextEdit *n = Object::cast_to<TextEdit>(get_node_or_null(text_edit_path))) {
            n->connect("text_changed", callable_mp(this, &Z1D_PropertyEdit::_on_text_edit_changed));
        }
    }
    if (!check_box_path.is_empty()) {
        if (CheckBox *n = Object::cast_to<CheckBox>(get_node_or_null(check_box_path))) {
            n->connect("toggled", callable_mp(this, &Z1D_PropertyEdit::_on_check_box_toggled));
        }
    }
    if (!spin_box_path.is_empty()) {
        if (SpinBox *n = Object::cast_to<SpinBox>(get_node_or_null(spin_box_path))) {
            n->connect("value_changed", callable_mp(this, &Z1D_PropertyEdit::_on_value_changed));
        }
    }
    if (!slider_path.is_empty()) {
        // HSlider and VSlider both extend Slider — cast to the base
        if (Slider *n = Object::cast_to<Slider>(get_node_or_null(slider_path))) {
            n->connect("value_changed", callable_mp(this, &Z1D_PropertyEdit::_on_value_changed));
        }
    }

    _update_label();
}

void Z1D_PropertyEdit::_disconnect_controls() {
    if (!text_edit_path.is_empty()) {
        if (TextEdit *n = Object::cast_to<TextEdit>(get_node_or_null(text_edit_path))) {
            n->disconnect("text_changed", callable_mp(this, &Z1D_PropertyEdit::_on_text_edit_changed));
        }
    }
    if (!check_box_path.is_empty()) {
        if (CheckBox *n = Object::cast_to<CheckBox>(get_node_or_null(check_box_path))) {
            n->disconnect("toggled", callable_mp(this, &Z1D_PropertyEdit::_on_check_box_toggled));
        }
    }
    if (!spin_box_path.is_empty()) {
        if (SpinBox *n = Object::cast_to<SpinBox>(get_node_or_null(spin_box_path))) {
            n->disconnect("value_changed", callable_mp(this, &Z1D_PropertyEdit::_on_value_changed));
        }
    }
    if (!slider_path.is_empty()) {
        if (Slider *n = Object::cast_to<Slider>(get_node_or_null(slider_path))) {
            n->disconnect("value_changed", callable_mp(this, &Z1D_PropertyEdit::_on_value_changed));
        }
    }
}

void Z1D_PropertyEdit::_set_dict_value(const Variant &p_value) {
    if (property == StringName()) { return; }
    dict[property] = p_value;
}

void Z1D_PropertyEdit::_update_label() {
    if (label_path.is_empty()) { return; }
    if (Label *lbl = Object::cast_to<Label>(get_node_or_null(label_path))) {
        lbl->set_text(property);
    }
}

// ---------------------------------------------------------------------------
// Signal callbacks
// ---------------------------------------------------------------------------

void Z1D_PropertyEdit::_on_text_edit_changed() {
    if (TextEdit *n = Object::cast_to<TextEdit>(get_node_or_null(text_edit_path))) {
        _set_dict_value(n->get_text());
    }
}

void Z1D_PropertyEdit::_on_check_box_toggled(bool p_pressed) {
    _set_dict_value(p_pressed);
}

void Z1D_PropertyEdit::_on_value_changed(double p_value) {
    _set_dict_value(p_value);
}

// ---------------------------------------------------------------------------
// Setters / Getters
// ---------------------------------------------------------------------------

void       Z1D_PropertyEdit::set_dict(const Dictionary &p_dict)         { dict     = p_dict; }
Dictionary Z1D_PropertyEdit::get_dict() const                           { return dict; }

void       Z1D_PropertyEdit::set_property(const StringName &p_property) { property = p_property; _update_label(); }
StringName Z1D_PropertyEdit::get_property() const                       { return property; }

void     Z1D_PropertyEdit::set_text_edit_path(const NodePath &p_path)   { text_edit_path = p_path; }
NodePath Z1D_PropertyEdit::get_text_edit_path() const                   { return text_edit_path; }

void     Z1D_PropertyEdit::set_check_box_path(const NodePath &p_path)   { check_box_path = p_path; }
NodePath Z1D_PropertyEdit::get_check_box_path() const                   { return check_box_path; }

void     Z1D_PropertyEdit::set_spin_box_path(const NodePath &p_path)    { spin_box_path  = p_path; }
NodePath Z1D_PropertyEdit::get_spin_box_path() const                    { return spin_box_path; }

void     Z1D_PropertyEdit::set_slider_path(const NodePath &p_path)      { slider_path    = p_path; }
NodePath Z1D_PropertyEdit::get_slider_path() const                      { return slider_path; }

void     Z1D_PropertyEdit::set_label_path(const NodePath &p_path)       { label_path     = p_path; }
NodePath Z1D_PropertyEdit::get_label_path() const                       { return label_path; }
