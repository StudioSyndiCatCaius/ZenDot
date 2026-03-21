#pragma once

#include "core/io/resource.h"
#include "scene/resources/texture.h"
#include "Zen_ParamType.h"

// Describes a single column/cell definition for a table-style UI.
// Holds display properties (column_name, icon, color) and a Zen_ParamType
// that defines what kind of value this cell holds and how to validate it.
//
// NOTE: The field is named `column_name` (not `name`) to avoid conflicting
// with Object's reserved "name" property in the Godot property system.
class Zen_TableCellConfig : public Resource {
	GDCLASS(Zen_TableCellConfig, Resource);

	String             column_name;
	Ref<Texture2D>     icon;
	Color              color      = Color(1, 1, 1, 1);
	Ref<Zen_ParamType> param_type;
	float              cell_ratio = 1.0f; // Relative width of this column. 2.0 = twice as wide as a column with ratio 1.0.

protected:
	static void _bind_methods();

public:
	void   set_column_name(const String &p_name);
	String get_column_name() const;

	void           set_icon(const Ref<Texture2D> &p_icon);
	Ref<Texture2D> get_icon() const;

	void  set_color(const Color &p_color);
	Color get_color() const;

	void               set_param_type(const Ref<Zen_ParamType> &p_type);
	Ref<Zen_ParamType> get_param_type() const;

	void  set_cell_ratio(float p_ratio);
	float get_cell_ratio() const;

	// Convenience — returns the type name string from the assigned Zen_ParamType,
	// or "unset" if no param_type has been assigned.
	String get_type_name() const;
};
