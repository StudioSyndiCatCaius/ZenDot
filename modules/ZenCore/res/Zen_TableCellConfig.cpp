#include "Zen_TableCellConfig.h"

void Zen_TableCellConfig::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_column_name", "column_name"), &Zen_TableCellConfig::set_column_name);
    ClassDB::bind_method(D_METHOD("get_column_name"),                &Zen_TableCellConfig::get_column_name);

    ClassDB::bind_method(D_METHOD("set_icon",        "icon"),        &Zen_TableCellConfig::set_icon);
    ClassDB::bind_method(D_METHOD("get_icon"),                       &Zen_TableCellConfig::get_icon);

    ClassDB::bind_method(D_METHOD("set_color",       "color"),       &Zen_TableCellConfig::set_color);
    ClassDB::bind_method(D_METHOD("get_color"),                      &Zen_TableCellConfig::get_color);

    ClassDB::bind_method(D_METHOD("set_param_type",  "param_type"),  &Zen_TableCellConfig::set_param_type);
    ClassDB::bind_method(D_METHOD("get_param_type"),                 &Zen_TableCellConfig::get_param_type);

    ClassDB::bind_method(D_METHOD("set_cell_ratio",  "cell_ratio"),  &Zen_TableCellConfig::set_cell_ratio);
    ClassDB::bind_method(D_METHOD("get_cell_ratio"),                 &Zen_TableCellConfig::get_cell_ratio);

    ClassDB::bind_method(D_METHOD("get_type_name"),                  &Zen_TableCellConfig::get_type_name);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "column_name"),                                                              "set_column_name", "get_column_name");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon",        PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"),                   "set_icon",        "get_icon");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR,  "color"),                                                                    "set_color",       "get_color");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "param_type",  PROPERTY_HINT_RESOURCE_TYPE, "Zen_ParamType"),               "set_param_type",  "get_param_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,  "cell_ratio",  PROPERTY_HINT_RANGE, "0.1,10.0,0.1"),                        "set_cell_ratio",  "get_cell_ratio");
}

void   Zen_TableCellConfig::set_column_name(const String &p_name)          { column_name = p_name; }
String Zen_TableCellConfig::get_column_name() const                        { return column_name; }

void           Zen_TableCellConfig::set_icon(const Ref<Texture2D> &p_icon) { icon       = p_icon; }
Ref<Texture2D> Zen_TableCellConfig::get_icon() const                       { return icon; }

void  Zen_TableCellConfig::set_color(const Color &p_color)                 { color      = p_color; }
Color Zen_TableCellConfig::get_color() const                               { return color; }

void               Zen_TableCellConfig::set_param_type(const Ref<Zen_ParamType> &p_type) { param_type = p_type; }
Ref<Zen_ParamType> Zen_TableCellConfig::get_param_type() const                           { return param_type; }

void  Zen_TableCellConfig::set_cell_ratio(float p_ratio) { cell_ratio = MAX(0.1f, p_ratio); }
float Zen_TableCellConfig::get_cell_ratio() const        { return cell_ratio; }

String Zen_TableCellConfig::get_type_name() const {
    if (param_type.is_valid()) {
        return param_type->get_type_name();
    }
    return "unset";
}
