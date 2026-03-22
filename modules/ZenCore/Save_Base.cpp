#include "Save_Base.h"

void ZSave_Base::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_VARS", "VARS"), &ZSave_Base::set_VARS);
	ClassDB::bind_method(D_METHOD("get_VARS"), &ZSave_Base::get_VARS);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "VARS", PROPERTY_HINT_DICTIONARY_TYPE, "", PROPERTY_USAGE_DEFAULT, "Dictionary"), "set_VARS", "get_VARS");
}
