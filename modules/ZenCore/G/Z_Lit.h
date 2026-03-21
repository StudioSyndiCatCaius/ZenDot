#ifndef Z_LIT_H
#define Z_LIT_H

#include "core/object/ref_counted.h"
#include "core/variant/variant.h"
#include "core/variant/dictionary.h"
#include "core/templates/vector.h"

// =============================================================================
// Z_Lit — ZenCore Literal Converter
//
// Provides static helper methods to safely coerce any Variant into a specific
// primitive or container type. Every method accepts a typed fallback value
// that is returned when conversion is impossible.
//
// TABLE convention: Array of Dictionary, where each Dictionary is one row.
//   [ {"col_a": val, "col_b": val}, ... ]
//
// DICT <-> TABLE:
//   dict_to_table  : { k: v }  →  [ {"key": k, "value": v}, ... ]
//   table_to_dict  : rows with "key"/"value" fields → { k: v }
//                    rows without those fields       → { 0: row0, 1: row1, ... }
// =============================================================================

class Z_Lit : public RefCounted {
    GDCLASS(Z_Lit, RefCounted)

protected:
    static void _bind_methods();

public:
    // -------------------------------------------------------------------------
    // Primitive conversions
    // -------------------------------------------------------------------------

    /// Coerce any Variant to bool.
    /// Non-zero numbers, non-empty strings/arrays/dicts, and true booleans → true.
    static bool _BOOL(const Variant &p_val, bool p_fallback = false);

    /// Coerce any Variant to int (int64_t).
    /// Strings are parsed; floats are truncated; bools become 0/1.
    static int64_t _INT(const Variant &p_val, int64_t p_fallback = 0);

    /// Coerce any Variant to float (double).
    /// Strings are parsed; ints/bools are widened.
    static double _FLOAT(const Variant &p_val, double p_fallback = 0.0);

    /// Coerce any Variant to String.
    /// Falls back only when the Variant type is NIL and p_fallback is provided.
    static String _STRING(const Variant &p_val, const String &p_fallback = String());

    // -------------------------------------------------------------------------
    // Container conversions
    // -------------------------------------------------------------------------

    /// Coerce any Variant to Dictionary.
    /// Accepts: Dictionary (pass-through), Array-of-Dicts (table_to_dict),
    ///          Array (index → value), Object (property list snapshot).
    static Dictionary _DICTIONARY(const Variant &p_val, const Dictionary &p_fallback = Dictionary());

    /// Coerce any Variant to an Array.
    /// Accepts: Array-of-Dicts (pass-through), Dictionary (dict_to_table),
    ///          plain Array (each element wrapped as {"value": element, "index": i}).
    static Array _ARRAY(const Variant &p_val, const Array &p_fallback = Array());

    // -------------------------------------------------------------------------
    // Explicit container bridge helpers (also exposed to GDScript)
    // -------------------------------------------------------------------------

    /// Convert a flat Dictionary → TABLE.
    /// Each key-value pair becomes one row: {"key": k, "value": v}
    static Array dict_to_table(const Dictionary &p_dict);

    /// Convert a TABLE → flat Dictionary.
    /// If every row has "key" and "value" fields, those are used directly.
    /// Otherwise the row index is used as the key and the whole row Dict as value.
    static Dictionary table_to_dict(const Array &p_table);

    // -------------------------------------------------------------------------
    // Utility: check whether an Array qualifies as a TABLE
    // -------------------------------------------------------------------------
    /// Returns true when every element of p_arr is a Dictionary.
    static bool is_table(const Array &p_arr);
};

#endif // Z_LIT_H
