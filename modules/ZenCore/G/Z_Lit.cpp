#include "z_lit.h"
#include "core/io/json.h"

#include "core/object/object.h"
#include "core/string/string_name.h"
#include "core/variant/array.h"

// =============================================================================
// _bind_methods — expose every static helper to GDScript / ClassDB
// =============================================================================
void Z_Lit::_bind_methods() {
	// --- Primitive conversions ---
	ClassDB::bind_static_method("Z_Lit", D_METHOD("BOOL",   "val", "fallback"), &Z_Lit::_BOOL,        DEFVAL(false));
	ClassDB::bind_static_method("Z_Lit", D_METHOD("INT",    "val", "fallback"), &Z_Lit::_INT,         DEFVAL(0));
	ClassDB::bind_static_method("Z_Lit", D_METHOD("FLOAT",  "val", "fallback"), &Z_Lit::_FLOAT,       DEFVAL(0.0));
	ClassDB::bind_static_method("Z_Lit", D_METHOD("STRING", "val", "fallback"), &Z_Lit::_STRING,      DEFVAL(String()));

	// --- Container conversions ---
	ClassDB::bind_static_method("Z_Lit", D_METHOD("DICTIONARY", "val", "fallback"), &Z_Lit::_DICTIONARY, DEFVAL(Dictionary()));
	ClassDB::bind_static_method("Z_Lit", D_METHOD("ARRAY",      "val", "fallback"), &Z_Lit::_ARRAY,      DEFVAL(Array()));

	// --- Bridge helpers ---
	ClassDB::bind_static_method("Z_Lit", D_METHOD("dict_to_table", "dict"),  &Z_Lit::dict_to_table);
	ClassDB::bind_static_method("Z_Lit", D_METHOD("table_to_dict", "table"), &Z_Lit::table_to_dict);
	ClassDB::bind_static_method("Z_Lit", D_METHOD("is_table",      "arr"),   &Z_Lit::is_table);
}


// =============================================================================
// to_bool
// =============================================================================
bool Z_Lit::_BOOL(const Variant &p_val, bool p_fallback) {
    switch (p_val.get_type()) {
        case Variant::NIL:
            return p_fallback;

        case Variant::BOOL:
            return (bool)p_val;

        case Variant::INT:
            return (int64_t)p_val != 0;

        case Variant::FLOAT:
            return (double)p_val != 0.0;

        case Variant::STRING: {
            String s = ((String)p_val).strip_edges().to_lower();
            if (s == "true"  || s == "1" || s == "yes" || s == "on")  return true;
            if (s == "false" || s == "0" || s == "no"  || s == "off") return false;
            // Non-empty string that isn't a recognised keyword → truthy
            return !s.is_empty();
        }

        case Variant::DICTIONARY:
            return !((Dictionary)p_val).is_empty();

        case Variant::ARRAY:
            return !((Array)p_val).is_empty();

        case Variant::OBJECT:
            return p_val.get_validated_object() != nullptr;

        default:
            // All other Variant types (Vector2, Color, etc.) are considered
            // truthy if the Variant itself is not nil.
            return true;
    }
}

// =============================================================================
// to_int
// =============================================================================
int64_t Z_Lit::_INT(const Variant &p_val, int64_t p_fallback) {
    switch (p_val.get_type()) {
        case Variant::NIL:
            return p_fallback;

        case Variant::BOOL:
            return (bool)p_val ? 1 : 0;

        case Variant::INT:
            return (int64_t)p_val;

        case Variant::FLOAT:
            return (int64_t)(double)p_val;  // truncate toward zero

        case Variant::STRING: {
            String s = ((String)p_val).strip_edges();
            if (s.is_valid_int())   return s.to_int();
            if (s.is_valid_float()) return (int64_t)s.to_float();
            return p_fallback;
        }

        default:
            return p_fallback;
    }
}

// =============================================================================
// to_float
// =============================================================================
double Z_Lit::_FLOAT(const Variant &p_val, double p_fallback) {
    switch (p_val.get_type()) {
        case Variant::NIL:
            return p_fallback;

        case Variant::BOOL:
            return (bool)p_val ? 1.0 : 0.0;

        case Variant::INT:
            return (double)(int64_t)p_val;

        case Variant::FLOAT:
            return (double)p_val;

        case Variant::STRING: {
            String s = ((String)p_val).strip_edges();
            if (s.is_valid_float()) return s.to_float();
            if (s.is_valid_int())   return (double)s.to_int();
            return p_fallback;
        }

        default:
            return p_fallback;
    }
}

// =============================================================================
// to_string
// =============================================================================
String Z_Lit::_STRING(const Variant &p_val, const String &p_fallback) {
    if (p_val.get_type() == Variant::NIL) {
        return p_fallback;
    }
    // Variant::operator String() handles all built-in types gracefully.
    return p_val.stringify();
}

// =============================================================================
// is_table  (helper used internally and exposed to GDScript)
// =============================================================================
bool Z_Lit::is_table(const Array &p_arr) {
    if (p_arr.is_empty()) {
        return true; // empty array is a valid (empty) table
    }
    for (int i = 0; i < p_arr.size(); ++i) {
        if (p_arr[i].get_type() != Variant::DICTIONARY) {
            return false;
        }
    }
    return true;
}

// =============================================================================
// dict_to_table
//   { "name": "Alice", "age": 30 }
//   → [ {"key":"name","value":"Alice"}, {"key":"age","value":30} ]
// =============================================================================
Array Z_Lit::dict_to_table(const Dictionary &p_dict) {
    Array table;
    Array keys = p_dict.keys();
    for (int i = 0; i < keys.size(); ++i) {
        Dictionary row;
        row["key"]   = keys[i];
        row["value"] = p_dict[keys[i]];
        table.push_back(row);
    }
    return table;
}

// =============================================================================
// table_to_dict
//   Strategy A — every row has "key" and "value" fields:
//     [ {"key":"name","value":"Alice"}, {"key":"age","value":30} ]
//     → { "name": "Alice", "age": 30 }
//
//   Strategy B — rows are arbitrary dicts (e.g. multi-column records):
//     [ {"id":1,"score":99}, {"id":2,"score":42} ]
//     → { 0: {"id":1,"score":99}, 1: {"id":2,"score":42} }
// =============================================================================
Dictionary Z_Lit::table_to_dict(const Array &p_table) {
    Dictionary result;

    if (p_table.is_empty()) {
        return result;
    }

    // Determine whether every row looks like a key/value record.
    bool all_kv = true;
    for (int i = 0; i < p_table.size(); ++i) {
        if (p_table[i].get_type() != Variant::DICTIONARY) {
            all_kv = false;
            break;
        }
        const Dictionary row = p_table[i];
        if (!row.has("key") || !row.has("value")) {
            all_kv = false;
            break;
        }
    }

    if (all_kv) {
        // Strategy A: reconstruct the original flat dictionary.
        for (int i = 0; i < p_table.size(); ++i) {
            const Dictionary row = p_table[i];
            result[row["key"]] = row["value"];
        }
    } else {
        // Strategy B: use row index as key, entire row dict as value.
        for (int i = 0; i < p_table.size(); ++i) {
            result[i] = p_table[i];
        }
    }

    return result;
}

// =============================================================================
// to_dict
// =============================================================================
Dictionary Z_Lit::_DICTIONARY(const Variant &p_val, const Dictionary &p_fallback) {
    switch (p_val.get_type()) {
        case Variant::NIL:
            return p_fallback;

        case Variant::DICTIONARY:
            return (Dictionary)p_val;

        case Variant::ARRAY: {
            const Array arr = (Array)p_val;
            // If it looks like a TABLE, use the smart bridge.
            if (is_table(arr)) {
                return table_to_dict(arr);
            }
            // Plain array → { index: element }
            Dictionary d;
            for (int i = 0; i < arr.size(); ++i) {
                d[i] = arr[i];
            }
            return d;
        }

        case Variant::OBJECT: {
            // Snapshot the object's exported / script properties.
            Object *obj = p_val.get_validated_object();
            if (!obj) return p_fallback;
            Dictionary d;
            List<PropertyInfo> props;
            obj->get_property_list(&props);
            for (const PropertyInfo &pi : props) {
                if (pi.usage & PROPERTY_USAGE_SCRIPT_VARIABLE) {
                    d[pi.name] = obj->get(pi.name);
                }
            }
            return d.is_empty() ? p_fallback : d;
        }


        default:
            return p_fallback;
    }
}

// =============================================================================
// to_table
// =============================================================================
Array Z_Lit::_ARRAY(const Variant &p_val, const Array &p_fallback) {
    switch (p_val.get_type()) {
        case Variant::NIL:
            return p_fallback;

        case Variant::ARRAY: {
            const Array arr = (Array)p_val;
            if (is_table(arr)) {
                return arr; // already a valid TABLE
            }
            // Plain array → wrap each element as {"index": i, "value": v}
            Array table;
            for (int i = 0; i < arr.size(); ++i) {
                Dictionary row;
                row["index"] = i;
                row["value"] = arr[i];
                table.push_back(row);
            }
            return table;
        }

        case Variant::DICTIONARY:
            return dict_to_table((Dictionary)p_val);



        default:
            return p_fallback;
    }
}
