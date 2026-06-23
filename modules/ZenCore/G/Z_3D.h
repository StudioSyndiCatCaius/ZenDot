#ifndef Z3D_H
#define Z3D_H

#include "core/object/ref_counted.h"
#include "scene/main/node.h"

class Z_3D : public RefCounted  {
	GDCLASS(Z_3D, RefCounted );

protected:
	static void _bind_methods();

public:

	static Transform3D GetNodeBoundingBox(Node3D *p_node, const Transform3D& p_relative);

};

#endif
