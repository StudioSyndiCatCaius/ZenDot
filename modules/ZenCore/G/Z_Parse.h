#ifndef Z_PARSE_H
#define Z_PARSE_H

#include "core/object/ref_counted.h"
#include "core/variant/array.h"
#include "core/variant/dictionary.h"
#include "core/string/ustring.h"

class Z_Parse : public RefCounted {
	GDCLASS(Z_Parse, RefCounted);

protected:
	static void _bind_methods();

public:
	// JSON
	static Dictionary from_JSON(const String &p_string);
	static String     to_JSON(const Dictionary &p_dict);

	// TOML
	static Dictionary from_TOML(const String &p_string);
	static String     to_TOML(const Dictionary &p_dict);

	// CSV
	static Variant    from_CSV(const String &p_string, bool as_array = false, int key_column = 0);
	static String     to_CSV(const Variant &p_data); // accepts Array or Dictionary

	static String JSON_to_LUA(const String &p_json);
	static String LUA_to_JSON(const String &p_lua);
};

#endif // Z_PARSE_H
