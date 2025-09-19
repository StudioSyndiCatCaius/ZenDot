#include "ZenCore.h"

void ZenCore::add(int p_value) {
	count += p_value;
}

void ZenCore::reset() {
	count = 0;
}

int ZenCore::get_total() const {
	return count;
}

void ZenCore::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add", "value"), &ZenCore::add);
	ClassDB::bind_method(D_METHOD("reset"), &ZenCore::reset);
	ClassDB::bind_method(D_METHOD("get_total"), &ZenCore::get_total);
}

ZenCore::ZenCore() {
	count = 0;
}
