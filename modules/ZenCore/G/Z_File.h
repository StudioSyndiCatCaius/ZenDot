#ifndef Z_FILE_H
#define Z_FILE_H

#include "core/object/ref_counted.h"
#include "core/string/ustring.h"
#include "core/variant/typed_array.h"

class Z_File : public RefCounted {
	GDCLASS(Z_File, RefCounted);

protected:
	static void _bind_methods();

public:
	static TypedArray<String> ListFilesInDir(const String &p_path, bool p_prepend_path, bool p_recursive);
	static Error SaveStringAsFile(const String &p_path, const String &p_content, bool p_force_overwrite, bool p_force_create=true);
	static String LoadFileAsString(const String &p_path);

	//Directories
	static String GetDir_Game();
	static String GetDir_Mods();
	static String GetDir_Config();
	static String GetDir_Saves();
};

#endif // Z_FILE_H
