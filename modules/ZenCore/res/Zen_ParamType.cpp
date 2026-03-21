#include "Zen_ParamType.h"

// ---------------------------------------------------------------------------
// Zen_ParamType_String
// ---------------------------------------------------------------------------

void Zen_ParamType_String::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_max_length", "max_length"), &Zen_ParamType_String::set_max_length);
    ClassDB::bind_method(D_METHOD("get_max_length"),               &Zen_ParamType_String::get_max_length);

    ClassDB::bind_method(D_METHOD("set_placeholder", "placeholder"), &Zen_ParamType_String::set_placeholder);
    ClassDB::bind_method(D_METHOD("get_placeholder"),                &Zen_ParamType_String::get_placeholder);

    ADD_PROPERTY(PropertyInfo(Variant::INT,    "max_length",   PROPERTY_HINT_RANGE, "0,9999,1"), "set_max_length",  "get_max_length");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "placeholder"),                                   "set_placeholder", "get_placeholder");
}

void Zen_ParamType_String::set_max_length(int p_max)               { max_length  = p_max; }
int  Zen_ParamType_String::get_max_length() const                  { return max_length; }
void Zen_ParamType_String::set_placeholder(const String &p_str)    { placeholder = p_str; }
String Zen_ParamType_String::get_placeholder() const               { return placeholder; }


// ---------------------------------------------------------------------------
// Zen_ParamType_Number
// ---------------------------------------------------------------------------

void Zen_ParamType_Number::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_is_float", "is_float"), &Zen_ParamType_Number::set_is_float);
    ClassDB::bind_method(D_METHOD("get_is_float"),             &Zen_ParamType_Number::get_is_float);

    ClassDB::bind_method(D_METHOD("set_min_value", "min"), &Zen_ParamType_Number::set_min_value);
    ClassDB::bind_method(D_METHOD("get_min_value"),        &Zen_ParamType_Number::get_min_value);

    ClassDB::bind_method(D_METHOD("set_max_value", "max"), &Zen_ParamType_Number::set_max_value);
    ClassDB::bind_method(D_METHOD("get_max_value"),        &Zen_ParamType_Number::get_max_value);

    ClassDB::bind_method(D_METHOD("set_clamp", "clamp"), &Zen_ParamType_Number::set_clamp);
    ClassDB::bind_method(D_METHOD("get_clamp"),          &Zen_ParamType_Number::get_clamp);

    ClassDB::bind_method(D_METHOD("set_step", "step"), &Zen_ParamType_Number::set_step);
    ClassDB::bind_method(D_METHOD("get_step"),         &Zen_ParamType_Number::get_step);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL,  "is_float"),                                       "set_is_float",  "get_is_float");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_value"),                                      "set_min_value", "get_min_value");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_value"),                                      "set_max_value", "get_max_value");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,  "clamp"),                                          "set_clamp",     "get_clamp");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "step",       PROPERTY_HINT_RANGE, "0.001,100,0.001"), "set_step",  "get_step");
}

void  Zen_ParamType_Number::set_is_float(bool p_float)  { is_float  = p_float; }
bool  Zen_ParamType_Number::get_is_float() const        { return is_float; }
void  Zen_ParamType_Number::set_min_value(float p_min)  { min_value = p_min; }
float Zen_ParamType_Number::get_min_value() const       { return min_value; }
void  Zen_ParamType_Number::set_max_value(float p_max)  { max_value = p_max; }
float Zen_ParamType_Number::get_max_value() const       { return max_value; }
void  Zen_ParamType_Number::set_clamp(bool p_clamp)     { clamp     = p_clamp; }
bool  Zen_ParamType_Number::get_clamp() const           { return clamp; }
void  Zen_ParamType_Number::set_step(float p_step)      { step      = p_step; }
float Zen_ParamType_Number::get_step() const            { return step; }


// ---------------------------------------------------------------------------
// Zen_ParamType_Bool
// ---------------------------------------------------------------------------

void Zen_ParamType_Bool::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_true_label",  "label"), &Zen_ParamType_Bool::set_true_label);
    ClassDB::bind_method(D_METHOD("get_true_label"),           &Zen_ParamType_Bool::get_true_label);

    ClassDB::bind_method(D_METHOD("set_false_label", "label"), &Zen_ParamType_Bool::set_false_label);
    ClassDB::bind_method(D_METHOD("get_false_label"),          &Zen_ParamType_Bool::get_false_label);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "true_label"),  "set_true_label",  "get_true_label");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "false_label"), "set_false_label", "get_false_label");
}

void   Zen_ParamType_Bool::set_true_label(const String &p_label)  { true_label  = p_label; }
String Zen_ParamType_Bool::get_true_label() const                 { return true_label; }
void   Zen_ParamType_Bool::set_false_label(const String &p_label) { false_label = p_label; }
String Zen_ParamType_Bool::get_false_label() const                { return false_label; }


// ---------------------------------------------------------------------------
// Zen_ParamType_Options
// ---------------------------------------------------------------------------

void Zen_ParamType_Options::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_options", "options"), &Zen_ParamType_Options::set_options);
    ClassDB::bind_method(D_METHOD("get_options"),            &Zen_ParamType_Options::get_options);

    ClassDB::bind_method(D_METHOD("set_allow_multiple", "allow"), &Zen_ParamType_Options::set_allow_multiple);
    ClassDB::bind_method(D_METHOD("get_allow_multiple"),          &Zen_ParamType_Options::get_allow_multiple);

    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "options"),        "set_options",        "get_options");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,                "allow_multiple"), "set_allow_multiple", "get_allow_multiple");
}

void              Zen_ParamType_Options::set_options(const PackedStringArray &p_options) { options        = p_options; }
PackedStringArray Zen_ParamType_Options::get_options() const                             { return options; }
void              Zen_ParamType_Options::set_allow_multiple(bool p_allow)                { allow_multiple = p_allow; }
bool              Zen_ParamType_Options::get_allow_multiple() const                      { return allow_multiple; }
