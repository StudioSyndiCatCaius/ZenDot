#include "res_Attribute.h"

ZenAttribute::ZenAttribute() {
}

ZenAttribute::~ZenAttribute() {
}

void ZenAttribute::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_color", "color"), &ZenAttribute::set_color);
	ClassDB::bind_method(D_METHOD("get_color"), &ZenAttribute::get_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");

	ClassDB::bind_method(D_METHOD("set_maxValue", "maxValue"), &ZenAttribute::set_maxValue);
	ClassDB::bind_method(D_METHOD("get_maxValue"), &ZenAttribute::get_maxValue);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "maxValue"), "set_maxValue", "get_maxValue");

	ClassDB::bind_method(D_METHOD("set_isStatic", "isStatic"), &ZenAttribute::set_isStatic);
	ClassDB::bind_method(D_METHOD("get_isStatic"), &ZenAttribute::get_isStatic);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "isStatic"), "set_isStatic", "get_isStatic");
}
