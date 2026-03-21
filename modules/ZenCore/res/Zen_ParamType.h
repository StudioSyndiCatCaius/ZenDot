#pragma once

#include "core/io/resource.h"
#include "scene/resources/texture.h"

// ---------------------------------------------------------------------------
// Base class — represents "a typed parameter definition".
// Subclass this to add type-specific constraints and metadata.
// Never instantiate directly; use one of the concrete subclasses below.
// ---------------------------------------------------------------------------
class Zen_ParamType : public Resource {
    GDCLASS(Zen_ParamType, Resource);

protected:
    static void _bind_methods() {}

public:
    // Returns a human-readable type identifier, e.g. "string", "number", "bool", "options".
    // Subclasses override this for runtime type inspection from GDScript.
    virtual String get_type_name() const { return "base"; }
};


// ---------------------------------------------------------------------------
// String — plain text value, with optional max length constraint.
// ---------------------------------------------------------------------------
class Zen_ParamType_String : public Zen_ParamType {
    GDCLASS(Zen_ParamType_String, Zen_ParamType);

    int max_length = 0;         // 0 = unlimited
    String placeholder;         // Hint text shown in empty fields

protected:
    static void _bind_methods();

public:
    String get_type_name() const override { return "string"; }

    void set_max_length(int p_max);
    int  get_max_length() const;

    void   set_placeholder(const String &p_placeholder);
    String get_placeholder() const;
};


// ---------------------------------------------------------------------------
// Number — integer or float, with optional min/max range.
// ---------------------------------------------------------------------------
class Zen_ParamType_Number : public Zen_ParamType {
    GDCLASS(Zen_ParamType_Number, Zen_ParamType);

    bool  is_float  = false;    // false = integer, true = float
    float min_value = 0.0f;
    float max_value = 0.0f;     // If min == max == 0, no range enforced
    bool  clamp     = false;    // Whether to enforce min/max at the UI level
    float step      = 1.0f;     // Step size for spinboxes (float mode: e.g. 0.01)

protected:
    static void _bind_methods();

public:
    String get_type_name() const override { return is_float ? "float" : "int"; }

    void set_is_float(bool p_float);
    bool get_is_float() const;

    void  set_min_value(float p_min);
    float get_min_value() const;

    void  set_max_value(float p_max);
    float get_max_value() const;

    void set_clamp(bool p_clamp);
    bool get_clamp() const;

    void  set_step(float p_step);
    float get_step() const;
};


// ---------------------------------------------------------------------------
// Bool — true/false toggle.
// Optionally customise the label shown for each state.
// ---------------------------------------------------------------------------
class Zen_ParamType_Bool : public Zen_ParamType {
    GDCLASS(Zen_ParamType_Bool, Zen_ParamType);

    String true_label  = "True";
    String false_label = "False";

protected:
    static void _bind_methods();

public:
    String get_type_name() const override { return "bool"; }

    void   set_true_label(const String &p_label);
    String get_true_label() const;

    void   set_false_label(const String &p_label);
    String get_false_label() const;
};


// ---------------------------------------------------------------------------
// Options — a fixed set of string choices (like an enum).
// ---------------------------------------------------------------------------
class Zen_ParamType_Options : public Zen_ParamType {
    GDCLASS(Zen_ParamType_Options, Zen_ParamType);

    PackedStringArray options;
    bool allow_multiple = false; // If true, multiple options can be selected

protected:
    static void _bind_methods();

public:
    String get_type_name() const override { return "options"; }

    void              set_options(const PackedStringArray &p_options);
    PackedStringArray get_options() const;

    void set_allow_multiple(bool p_allow);
    bool get_allow_multiple() const;
};
