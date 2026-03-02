#ifndef ZENATTRIBUTE_H
#define ZENATTRIBUTE_H

#include "GameplayResource.h"
#include "core/io/resource.h"

class ZenAttribute : public GameplayResource {
	GDCLASS(ZenAttribute, GameplayResource);

protected:
	ZenAttribute();
	~ZenAttribute();

	static void _bind_methods();
public:
	Color color;
	Color get_color() const { return color; }
	void set_color(Color p_color) { color=p_color; }

	float MaxValue;
	float get_maxValue() const { return MaxValue; }
	void set_maxValue(float p_MaxValue) { MaxValue=p_MaxValue; }

	bool isStatic;
	bool get_isStatic() const { return isStatic; }
	void set_isStatic(bool p_isStatic) { isStatic=p_isStatic; }
};

#endif
