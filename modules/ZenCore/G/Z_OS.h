#ifndef Z_OS_H
#define Z_OS_H

#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/variant/array.h"

class Z_OS : public RefCounted {
	GDCLASS(Z_OS, RefCounted);

protected:
	static void _bind_methods();

public:
	static int do_Python(const String &python_script_path, const PackedStringArray &args, bool show_console, bool is_blocking);

	Z_OS() {}
};

#endif // Z_OS_H
