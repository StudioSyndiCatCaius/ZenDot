#include "Z_3D.h"

#include "scene/3d/visual_instance_3d.h"

void Z_3D::_bind_methods() {
	ClassDB::bind_static_method("Z_3D", D_METHOD("GetNodeBoundingBox", "node", "relative"), &Z_3D::GetNodeBoundingBox, DEFVAL(Transform3D()));
}

Transform3D Z_3D::GetNodeBoundingBox(Node3D *p_node, const Transform3D &p_relative) {
	ERR_FAIL_NULL_V(p_node, Transform3D());

	AABB combined;
	bool first = true;

	VisualInstance3D *vi = Object::cast_to<VisualInstance3D>(p_node);
	if (vi) {
		combined = vi->get_global_transform().xform(vi->get_aabb());
		first = false;
	}

	TypedArray<Node> children = p_node->find_children("*", "VisualInstance3D", true, false);
	for (int i = 0; i < children.size(); i++) {
		VisualInstance3D *child = Object::cast_to<VisualInstance3D>(children[i]);
		if (child) {
			AABB child_aabb = child->get_global_transform().xform(child->get_aabb());
			if (first) {
				combined = child_aabb;
				first = false;
			} else {
				combined = combined.merge(child_aabb);
			}
		}
	}

	// Build transform: origin at center, scale = full size
	Vector3 center = combined.get_center();
	Vector3 size = combined.size;

	Transform3D result;
	result.origin = center;
	result.basis.scale(size); // Encodes size in basis scale

	// Apply relative transform
	if (p_relative != Transform3D()) {
		result = p_relative.affine_inverse() * result;
	}

	return result;
}
