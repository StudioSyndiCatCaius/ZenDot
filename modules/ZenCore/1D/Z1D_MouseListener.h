#pragma once

#include "scene/main/node.h"
#include "scene/2d/node_2d.h"
#include "scene/3d/node_3d.h"
#include "core/input/input_event.h"
#include "core/templates/hash_map.h"

class Z1D_MouseListener : public Node {
	GDCLASS(Z1D_MouseListener, Node);

	NodePath target_2d_path;
	NodePath target_3d_path;

	Node2D *target_2d = nullptr;
	Node3D *target_3d = nullptr;

	// Tracks hover state for children of target nodes
	HashMap<Node *, bool> hovered_children;

	void _update_targets();
	void _rebuild_tracked_children();
	void _check_hover(const Vector2 &mouse_pos);
	bool _point_in_node(Node *p_node, const Vector2 &mouse_pos);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_target_2d(const NodePath &p_path);
	NodePath get_target_2d() const;

	void set_target_3d(const NodePath &p_path);
	NodePath get_target_3d() const;

	void _input(const Ref<InputEvent> &p_event);
};
