#ifndef ZENCORE_H
#define ZENCORE_H

#include "core/object/ref_counted.h"

class ZenCore : public RefCounted {
	GDCLASS(ZenCore, RefCounted);

	int count;

protected:
	static void _bind_methods();

public:
	void add(int p_value);
	void reset();
	int get_total() const;

	ZenCore();
};

#endif // ZENCORE_H
