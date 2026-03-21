#include "z_file.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/os/os.h"

void Z_File::_bind_methods() {
	ClassDB::bind_static_method("Z_File", D_METHOD("ListFilesInDir", "path", "prepend_path", "recursive"), &Z_File::ListFilesInDir);
	ClassDB::bind_static_method("Z_File", D_METHOD("SaveStringAsFile", "path", "content", "force_overwrite","force_create"), &Z_File::SaveStringAsFile);
	ClassDB::bind_static_method("Z_File",D_METHOD("LoadFileAsString", "path"),&Z_File::LoadFileAsString);

	ClassDB::bind_static_method("Z_File",D_METHOD("GetDir_Game"),&Z_File::GetDir_Game);
	ClassDB::bind_static_method("Z_File",D_METHOD("GetDir_Mods"),&Z_File::GetDir_Mods);
	ClassDB::bind_static_method("Z_File",D_METHOD("GetDir_Config"),&Z_File::GetDir_Config);
}

TypedArray<String> Z_File::ListFilesInDir(const String &p_path, bool p_prepend_path, bool p_recursive) {
	TypedArray<String> result;

	Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		return result;
	}

	dir->list_dir_begin();
	String file_name = dir->get_next();

	while (!file_name.is_empty()) {
		if (file_name == "." || file_name == "..") {
			file_name = dir->get_next();
			continue;
		}

		String full_path = p_path.path_join(file_name);

		if (dir->current_is_dir()) {
			if (p_recursive) {
				TypedArray<String> sub = ListFilesInDir(full_path, p_prepend_path, true);
				for (int i = 0; i < sub.size(); i++) {
					result.push_back(sub[i]);
				}
			}
		} else {
			result.push_back(p_prepend_path ? full_path : file_name);
		}

		file_name = dir->get_next();
	}

	dir->list_dir_end();
	return result;
}

Error Z_File::SaveStringAsFile(const String &p_path, const String &p_content, bool p_force_overwrite, bool p_force_create) {
	if (!p_force_overwrite && FileAccess::exists(p_path)) {
		return ERR_ALREADY_EXISTS;
	}

	if (p_force_create) {
		String dir_path = p_path.get_base_dir();
		if (!DirAccess::dir_exists_absolute(dir_path)) {
			Error dir_err = DirAccess::make_dir_recursive_absolute(dir_path);
			if (dir_err != OK) {
				ERR_PRINT(vformat("Z_File: Failed to create directories for path: %s", p_path));
				return dir_err;
			}
		}
	}

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	if (file.is_null()) {
		ERR_PRINT(vformat("Z_File: Failed to open file for writing: %s", p_path));
		return ERR_CANT_OPEN;
	}

	file->store_string(p_content);
	file->close();
	print_line("Z: Successfully saved string file to: "+p_path);

	return OK;
}

String Z_File::LoadFileAsString(const String &p_path) {
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);
	if (file.is_null()) {
		ERR_PRINT(vformat("Z_File: Failed to open file for reading: %s", p_path));
		return String();
	}

	String content = file->get_as_text();
	file->close();
	return content;
}

String Z_File::GetDir_Game()
{
	if (OS::get_singleton()->has_feature("editor")) {
		return "res://";
	}
	return OS::get_singleton()->get_executable_path().get_base_dir() + "/";
}

String Z_File::GetDir_Mods()
{
	return GetDir_Game()+"Mods/";
}

String Z_File::GetDir_Config() {
	return GetDir_Game()+"Config/";
}
