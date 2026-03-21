#include "z_parse.h"

#include "core/io/json.h"
#include "core/string/ustring.h"
#include "core/variant/array.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"

// ─────────────────────────────────────────────
//  Bind
// ─────────────────────────────────────────────

void Z_Parse::_bind_methods() {
    ClassDB::bind_static_method("Z_Parse", D_METHOD("from_JSON", "string"),   &Z_Parse::from_JSON);
    ClassDB::bind_static_method("Z_Parse", D_METHOD("to_JSON",   "dict"),     &Z_Parse::to_JSON);

    ClassDB::bind_static_method("Z_Parse", D_METHOD("from_TOML", "string"),   &Z_Parse::from_TOML);
    ClassDB::bind_static_method("Z_Parse", D_METHOD("to_TOML",   "dict"),     &Z_Parse::to_TOML);

    ClassDB::bind_static_method("Z_Parse", D_METHOD("from_CSV", "string", "as_array", "key_column"),
                                &Z_Parse::from_CSV, DEFVAL(false), DEFVAL(0));
    ClassDB::bind_static_method("Z_Parse", D_METHOD("to_CSV", "data"),        &Z_Parse::to_CSV);

	// Bind without the success param — it's a C++-only out arg.
	ClassDB::bind_static_method("Z_Parse", D_METHOD("JSON_to_LUA", "json"), &Z_Parse::JSON_to_LUA);
	ClassDB::bind_static_method("Z_Parse", D_METHOD("LUA_to_JSON", "lua"),  &Z_Parse::LUA_to_JSON);
}

// ─────────────────────────────────────────────
//  JSON
// ─────────────────────────────────────────────

Dictionary Z_Parse::from_JSON(const String &p_string) {
    JSON json;
    Error err = json.parse(p_string);
    if (err != OK) {
        ERR_PRINT("Z_Parse::from_json — parse error at line "
                  + itos(json.get_error_line()) + ": " + json.get_error_message());
        return Dictionary();
    }
    Variant result = json.get_data();
    if (result.get_type() != Variant::DICTIONARY) {
        ERR_PRINT("Z_Parse::from_json — root element is not a Dictionary.");
        return Dictionary();
    }
    return result;
}

String Z_Parse::to_JSON(const Dictionary &p_dict) {
    return JSON::stringify(p_dict, "\t");
}

// ─────────────────────────────────────────────
//  TOML helpers (file-scope)
// ─────────────────────────────────────────────

static String _toml_strip_comment(const String &line) {
    bool in_str = false;
    char32_t quote_char = 0;
    for (int i = 0; i < line.length(); ++i) {
        char32_t c = line[i];
        if (in_str) {
            if (c == quote_char) in_str = false;
        } else {
            if (c == '"' || c == '\'') { in_str = true; quote_char = c; }
            else if (c == '#') return line.substr(0, i);
        }
    }
    return line;
}

static Array _toml_parse_inline_array(const String &raw);

static Variant _toml_parse_value(const String &raw) {
    if (raw.begins_with("[") && raw.ends_with("]"))
        return _toml_parse_inline_array(raw.substr(1, raw.length() - 2));
    if (raw.begins_with("\"") && raw.ends_with("\"") && raw.length() >= 2)
        return raw.substr(1, raw.length() - 2);
    if (raw.begins_with("'") && raw.ends_with("'") && raw.length() >= 2)
        return raw.substr(1, raw.length() - 2);
    if (raw == "true")  return true;
    if (raw == "false") return false;
    if (raw.contains(".") || raw.to_lower().contains("e")) return raw.to_float();
    if (raw.is_valid_int()) return raw.to_int();
    return raw;
}

static Array _toml_parse_inline_array(const String &raw) {
    Array result;
    int depth = 0; bool in_str = false; char32_t qc = 0; int start = 0;
    for (int i = 0; i <= raw.length(); ++i) {
        char32_t c = (i < raw.length()) ? raw[i] : ',';
        if (in_str) { if (c == qc) in_str = false; }
        else if (c == '"' || c == '\'') { in_str = true; qc = c; }
        else if (c == '[' || c == '{') { depth++; }
        else if (c == ']' || c == '}') { depth--; }
        else if (c == ',' && depth == 0) {
            String el = raw.substr(start, i - start).strip_edges();
            if (!el.is_empty()) result.push_back(_toml_parse_value(el));
            start = i + 1;
        }
    }
    return result;
}

// Serialize a Variant to a TOML value string.
static String _toml_serialize_value(const Variant &v);

static String _toml_serialize_inline_array(const Array &arr) {
    String out = "[";
    for (int i = 0; i < arr.size(); ++i) {
        if (i > 0) out += ", ";
        out += _toml_serialize_value(arr[i]);
    }
    return out + "]";
}

static String _toml_serialize_value(const Variant &v) {
    switch (v.get_type()) {
        case Variant::BOOL:   return (bool)v ? "true" : "false";
        case Variant::INT:    return itos((int64_t)v);
        case Variant::FLOAT:  return rtos((double)v);
        case Variant::STRING: return "\"" + ((String)v).replace("\\", "\\\\").replace("\"", "\\\"") + "\"";
        case Variant::ARRAY:  return _toml_serialize_inline_array((Array)v);
        default:              return "\"" + (String)v + "\"";
    }
}

// Recursively write [sections] and key=value pairs.
static void _toml_write_section(const Dictionary &dict, const String &prefix, String &out) {
    // First pass: write scalar/array key=value pairs at this level.
    Array keys = dict.keys();
    for (int i = 0; i < keys.size(); ++i) {
        Variant v = dict[keys[i]];
        if (v.get_type() != Variant::DICTIONARY) {
            out += (String)keys[i] + " = " + _toml_serialize_value(v) + "\n";
        }
    }
    // Second pass: recurse into sub-tables.
    for (int i = 0; i < keys.size(); ++i) {
        Variant v = dict[keys[i]];
        if (v.get_type() == Variant::DICTIONARY) {
            String section = prefix.is_empty()
                             ? (String)keys[i]
                             : prefix + "." + (String)keys[i];
            out += "\n[" + section + "]\n";
            _toml_write_section((Dictionary)v, section, out);
        }
    }
}

// ─────────────────────────────────────────────
//  TOML
// ─────────────────────────────────────────────

Dictionary Z_Parse::from_TOML(const String &p_string) {
    Dictionary root;
    Dictionary *current_table = &root;
    Array current_path;

    Vector<String> lines = p_string.split("\n");

    for (int i = 0; i < lines.size(); ++i) {
        String line = _toml_strip_comment(lines[i]).strip_edges();
        if (line.is_empty()) continue;

        // [[array-of-tables]]
        if (line.begins_with("[[") && line.ends_with("]]")) {
            String key_path = line.substr(2, line.length() - 4).strip_edges();
            Vector<String> parts = key_path.split(".");
            Dictionary *d = &root;
            for (int p = 0; p < parts.size() - 1; ++p) {
                String k = parts[p].strip_edges();
                if (!d->has(k)) d->operator[](k) = Dictionary();
                d = (Dictionary *)(&d->operator[](k));
            }
            String last_key = parts[parts.size() - 1].strip_edges();
            if (!d->has(last_key)) d->operator[](last_key) = Array();
            Array arr = d->operator[](last_key);
            arr.push_back(Dictionary());
            d->operator[](last_key) = arr;
            current_path.clear();
            for (int p = 0; p < parts.size(); ++p) current_path.push_back(parts[p].strip_edges());
            current_table = nullptr;
            continue;
        }

        // [section]
        if (line.begins_with("[") && line.ends_with("]")) {
            String key_path = line.substr(1, line.length() - 2).strip_edges();
            Vector<String> parts = key_path.split(".");
            current_path.clear();
            Dictionary *d = &root;
            for (int p = 0; p < parts.size(); ++p) {
                String k = parts[p].strip_edges();
                current_path.push_back(k);
                if (!d->has(k)) d->operator[](k) = Dictionary();
                Variant v = d->operator[](k);
                if (v.get_type() == Variant::ARRAY) {
                    Array a = v;
                    d = (Dictionary *)(&a[a.size() - 1]);
                } else {
                    d = (Dictionary *)(&d->operator[](k));
                }
            }
            current_table = d;
            continue;
        }

        // key = value
        int eq = line.find("=");
        if (eq < 1) continue;
        String key   = line.substr(0, eq).strip_edges().trim_prefix("\"").trim_suffix("\"");
        String value = line.substr(eq + 1).strip_edges();
        Variant parsed_value = _toml_parse_value(value);

        if (current_table != nullptr) {
            current_table->operator[](key) = parsed_value;
        } else {
            Dictionary *d = &root;
            for (int p = 0; p < current_path.size(); ++p) {
                String k = current_path[p];
                Variant v = d->operator[](k);
                if (v.get_type() == Variant::ARRAY) {
                    Array a = v; d = (Dictionary *)(&a[a.size() - 1]);
                } else {
                    d = (Dictionary *)(&d->operator[](k));
                }
            }
            d->operator[](key) = parsed_value;
        }
    }
    return root;
}

String Z_Parse::to_TOML(const Dictionary &p_dict) {
    String out;
    _toml_write_section(p_dict, "", out);
    return out;
}

// ─────────────────────────────────────────────
//  CSV helpers (file-scope)
// ─────────────────────────────────────────────

static Vector<String> _csv_split_row(const String &row) {
    Vector<String> fields;
    String current;
    bool in_quotes = false;
    for (int i = 0; i < row.length(); ++i) {
        char32_t c = row[i];
        if (c == '"') {
            if (in_quotes && i + 1 < row.length() && row[i + 1] == '"') {
                current += '"'; ++i;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (c == ',' && !in_quotes) {
            fields.push_back(current);
            current = String();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

static String _csv_quote_field(const String &field) {
    if (field.contains(",") || field.contains("\"") || field.contains("\n")) {
        return "\"" + field.replace("\"", "\"\"") + "\"";
    }
    return field;
}

// ─────────────────────────────────────────────
//  CSV
// ─────────────────────────────────────────────

Variant Z_Parse::from_CSV(const String &p_string, bool as_array, int key_column) {
    Vector<String> lines = p_string.split("\n");
    while (!lines.is_empty() && lines[lines.size() - 1].strip_edges().is_empty())
        lines.resize(lines.size() - 1);

    if (lines.is_empty())
        return as_array ? Variant(Array()) : Variant(Dictionary());

    Vector<String> headers = _csv_split_row(lines[0]);

	if (as_array) {
		Array result;
		for (int i = 1; i < lines.size(); ++i) {  // start at 1 to skip header row
			if (lines[i].strip_edges().is_empty()) continue;
			Vector<String> fields = _csv_split_row(lines[i]);
			while (fields.size() < headers.size()) fields.push_back(String());
			Dictionary row_dict;
			for (int f = 0; f < headers.size(); ++f) {
				row_dict[headers[f]] = fields[f];
			}
			result.push_back(row_dict);
		}
		return result;
	}
    ERR_FAIL_COND_V_MSG(key_column < 0 || key_column >= headers.size(), Dictionary(),
                        "Z_Parse::from_csv — key_column out of range.");

    Dictionary result;
    for (int i = 1; i < lines.size(); ++i) {
        if (lines[i].strip_edges().is_empty()) continue;
        Vector<String> fields = _csv_split_row(lines[i]);
        while (fields.size() < headers.size()) fields.push_back(String());
        Dictionary row_dict;
        for (int f = 0; f < headers.size(); ++f) row_dict[headers[f]] = fields[f];
        result[fields[key_column]] = row_dict;
    }
    return result;
}

String Z_Parse::to_CSV(const Variant &p_data) {
    String out;

    // Array of Arrays
	if (p_data.get_type() == Variant::ARRAY) {
		Array rows = p_data;
		if (rows.is_empty()) return "";

		ERR_FAIL_COND_V_MSG(rows[0].get_type() != Variant::DICTIONARY, "",
							"Z_Parse::to_csv — Array entries must be Dictionaries.");

		// Build header from first row's keys.
		Array headers = ((Dictionary)rows[0]).keys();

		// Header row
		String out;
		for (int h = 0; h < headers.size(); ++h) {
			out += _csv_quote_field((String)headers[h]) + ",";
		}
		out += "\n";

		// Data rows
		for (int i = 0; i < rows.size(); ++i) {
			ERR_CONTINUE(rows[i].get_type() != Variant::DICTIONARY);
			Dictionary row = rows[i];
			String line;
			for (int h = 0; h < headers.size(); ++h) {
				line += _csv_quote_field(row.has(headers[h]) ? (String)row[headers[h]] : "") + ",";
			}
			out += line + "\n";
		}
		return out;
	}

    // Dictionary → header row from sub-dict keys, row key in first column.
    if (p_data.get_type() == Variant::DICTIONARY) {
        Dictionary dict = p_data;
        Array keys = dict.keys();
        if (keys.is_empty()) return "";

        // Build header from first row's sub-dictionary.
        ERR_FAIL_COND_V_MSG(dict[keys[0]].get_type() != Variant::DICTIONARY, "",
                            "Z_Parse::to_csv — Dictionary values must be Dictionaries.");
        Array headers = ((Dictionary)dict[keys[0]]).keys();

        // Header row
        String header_line;
        for (int h = 0; h < headers.size(); ++h) {
            if (h > 0) header_line += ",";
            header_line += _csv_quote_field((String)headers[h]);
        }
        out += header_line + "\n";

        // Data rows
        for (int i = 0; i < keys.size(); ++i) {
            Dictionary row_dict = dict[keys[i]];
            String line;
            for (int h = 0; h < headers.size(); ++h) {
                if (h > 0) line += ",";
                line += _csv_quote_field(row_dict.has(headers[h])
                                         ? (String)row_dict[headers[h]]
                                         : "");
            }
            out += line + "\n";
        }
        return out;
    }

    ERR_PRINT("Z_Parse::to_csv — input must be an Array or Dictionary.");
    return "";
}

// ─────────────────────────────────────────────
//  Lua helpers (file-scope)
// ─────────────────────────────────────────────

// Returns true if a string can be used as a bare Lua identifier key.
static bool _lua_is_bare_key(const String &s) {
    if (s.is_empty()) return false;
    char32_t first = s[0];
    if (!((first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z') || first == '_'))
        return false;
    for (int i = 1; i < s.length(); ++i) {
        char32_t c = s[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_'))
            return false;
    }
    return true;
}

static String _lua_serialize_value(const Variant &v, int depth = 0);

static String _lua_serialize_table(const Variant &v, int depth) {
    String indent(String("\t").repeat(depth));
    String inner(String("\t").repeat(depth + 1));

    if (v.get_type() == Variant::ARRAY) {
        Array arr = v;
        if (arr.is_empty()) return "{}";
        String out = "{\n";
        for (int i = 0; i < arr.size(); ++i) {
            out += inner + _lua_serialize_value(arr[i], depth + 1);
            if (i < arr.size() - 1) out += ",";
            out += "\n";
        }
        return out + indent + "}";
    }

    if (v.get_type() == Variant::DICTIONARY) {
        Dictionary dict = v;
        Array keys = dict.keys();
        if (keys.is_empty()) return "{}";
        String out = "{\n";
        for (int i = 0; i < keys.size(); ++i) {
            String key = (String)keys[i];
            String val = _lua_serialize_value(dict[keys[i]], depth + 1);
            if (_lua_is_bare_key(key)) {
                out += inner + key + " = " + val;
            } else {
                out += inner + "[\"" + key.replace("\"", "\\\"") + "\"] = " + val;
            }
            if (i < keys.size() - 1) out += ",";
            out += "\n";
        }
        return out + indent + "}";
    }

    return "{}";
}

static String _lua_serialize_value(const Variant &v, int depth) {
    switch (v.get_type()) {
        case Variant::NIL:     return "nil";
        case Variant::BOOL:    return (bool)v ? "true" : "false";
        case Variant::INT:     return itos((int64_t)v);
        case Variant::FLOAT:   return rtos((double)v);
        case Variant::STRING:  return "\"" + ((String)v).replace("\\", "\\\\").replace("\"", "\\\"") + "\"";
        case Variant::ARRAY:
        case Variant::DICTIONARY:
            return _lua_serialize_table(v, depth);
        default:
            return "\"" + ((String)v) + "\"";
    }
}

// ── Lua parser ────────────────────────────────────────────────────────────────
// Simple recursive-descent parser covering the subset that JSON can round-trip:
// tables, arrays, strings, numbers, bools, nil, and -- comments.

struct LuaParser {
    const String &src;
    int pos = 0;

    LuaParser(const String &p_src) : src(p_src) {}

    bool at_end() const { return pos >= src.length(); }
    char32_t peek()    const { return at_end() ? 0 : src[pos]; }
    char32_t advance() { return at_end() ? 0 : src[pos++]; }

    void skip_whitespace_and_comments() {
        while (!at_end()) {
            char32_t c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                advance();
            } else if (c == '-' && pos + 1 < src.length() && src[pos + 1] == '-') {
                // Single-line comment; skip to end of line.
                while (!at_end() && peek() != '\n') advance();
            } else {
                break;
            }
        }
    }

    // Returns false on parse failure, sets out_error.
    bool parse_value(Variant &out, String &out_error) {
        skip_whitespace_and_comments();
        if (at_end()) { out_error = "Unexpected end of input."; return false; }

        char32_t c = peek();

        if (c == '{')                          return parse_table(out, out_error);
        if (c == '"' || c == '\'')             return parse_string(out, out_error);
        if (c == '-' || (c >= '0' && c <= '9')) return parse_number(out, out_error);

        // Literals: true / false / nil
        if (src.substr(pos, 4) == "true")  { pos += 4; out = true;    return true; }
        if (src.substr(pos, 5) == "false") { pos += 5; out = false;   return true; }
        if (src.substr(pos, 3) == "nil")   { pos += 3; out = Variant(); return true; }

        out_error = "Unexpected character: '" + String::chr(c) + "' at position " + itos(pos);
        return false;
    }

    bool parse_table(Variant &out, String &out_error) {
        advance(); // consume '{'
        Dictionary dict;
        Array arr;
        bool is_array = true; // Assume array until we see a key=value pair.
        int arr_index = 1;    // Lua arrays are 1-indexed; we use it to detect sequence tables.

        while (true) {
            skip_whitespace_and_comments();
            if (at_end()) { out_error = "Unterminated table."; return false; }
            if (peek() == '}') { advance(); break; }

            // Optional comma between entries.
            if (peek() == ',') { advance(); skip_whitespace_and_comments(); }
            if (peek() == '}') { advance(); break; }

            // Determine if this entry is key = value or a plain value.
            // Look ahead: identifier/string followed by '=' → keyed entry.
            int saved_pos = pos;
            String key;
            bool is_keyed = false;

            // [" expr "] = val  or  bare_key = val
            if (peek() == '[') {
                advance(); // consume '['
                skip_whitespace_and_comments();
                Variant key_v;
                if (!parse_value(key_v, out_error)) return false;
                skip_whitespace_and_comments();
                if (peek() != ']') { out_error = "Expected ']' in table key."; return false; }
                advance(); // consume ']'
                skip_whitespace_and_comments();
                if (peek() != '=') { out_error = "Expected '=' after table key."; return false; }
                advance(); // consume '='
                key = (String)key_v;
                is_keyed = true;
            } else {
                // Try to read a bare identifier followed by '='.
                String ident;
                int scan = pos;
                while (scan < src.length()) {
                    char32_t sc = src[scan];
                    if ((sc >= 'a' && sc <= 'z') || (sc >= 'A' && sc <= 'Z') ||
                        (sc >= '0' && sc <= '9') || sc == '_') {
                        ident += sc; scan++;
                    } else { break; }
                }
                // Skip whitespace to check for '='
                int scan2 = scan;
                while (scan2 < src.length() && (src[scan2] == ' ' || src[scan2] == '\t')) scan2++;
                if (!ident.is_empty() && scan2 < src.length() && src[scan2] == '=') {
                    pos = scan2 + 1; // consume ident + whitespace + '='
                    key = ident;
                    is_keyed = true;
                }
            }

            Variant value;
            if (!parse_value(value, out_error)) return false;

            if (is_keyed) {
                is_array = false;
                dict[key] = value;
            } else {
                arr.push_back(value);
                dict[itos(arr_index++)] = value;
            }
        }

        out = is_array ? Variant(arr) : Variant(dict);
        return true;
    }

    bool parse_string(Variant &out, String &out_error) {
        char32_t quote = advance(); // ' or "
        String result;
        while (!at_end()) {
            char32_t c = advance();
            if (c == quote) { out = result; return true; }
            if (c == '\\') {
                if (at_end()) break;
                char32_t esc = advance();
                switch (esc) {
                    case 'n':  result += '\n'; break;
                    case 't':  result += '\t'; break;
                    case '"':  result += '"';  break;
                    case '\'': result += '\''; break;
                    case '\\': result += '\\'; break;
                    default:   result += esc;  break;
                }
            } else {
                result += c;
            }
        }
        out_error = "Unterminated string.";
        return false;
    }

    bool parse_number(Variant &out, String &out_error) {
        int start = pos;
        if (peek() == '-') advance();
        while (!at_end() && peek() >= '0' && peek() <= '9') advance();
        bool is_float = false;
        if (!at_end() && peek() == '.') { is_float = true; advance(); }
        while (!at_end() && peek() >= '0' && peek() <= '9') advance();
        if (!at_end() && (peek() == 'e' || peek() == 'E')) {
            is_float = true; advance();
            if (!at_end() && (peek() == '+' || peek() == '-')) advance();
            while (!at_end() && peek() >= '0' && peek() <= '9') advance();
        }
        String num_str = src.substr(start, pos - start);
        if (is_float) { out = num_str.to_float(); }
        else          { out = num_str.to_int();   }
        return true;
    }
};

// ─────────────────────────────────────────────
//  JSON ↔ Lua bridges
// ─────────────────────────────────────────────

String Z_Parse::JSON_to_LUA(const String &p_json) {
	JSON json;
	Error err = json.parse(p_json);
	if (err != OK) {
		WARN_PRINT("Z_Parse::json_to_lua — JSON parse error at line "
				   + itos(json.get_error_line()) + ": " + json.get_error_message());
		return p_json;
	}
	return _lua_serialize_value(json.get_data());
}

String Z_Parse::LUA_to_JSON(const String &p_lua) {
	LuaParser parser(p_lua);
	Variant result;
	String error;
	parser.skip_whitespace_and_comments();
	if (!parser.parse_value(result, error)) {
		WARN_PRINT("Z_Parse::lua_to_json — Lua parse error: " + error);
		return p_lua;
	}
	return JSON::stringify(result, "\t");
}
