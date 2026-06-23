#include "Z1D_WorldEditor.h"
#include "3D/Z3D_Gizmo.h"
#include "core/math/math_defs.h"
// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

Z1D_WorldEditor::Z1D_WorldEditor() {
}

Z1D_WorldEditor::~Z1D_WorldEditor() {
	_destroy_default_gizmo();
}


// ---------------------------------------------------------------------------
// Binding
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::_bind_methods() {
	// --- Edit Mode ---
	ClassDB::bind_method(D_METHOD("set_edit_mode", "enabled"), &Z1D_WorldEditor::set_edit_mode);
	ClassDB::bind_method(D_METHOD("get_edit_mode"),            &Z1D_WorldEditor::get_edit_mode);
	ClassDB::bind_method(D_METHOD("toggle_edit_mode"),         &Z1D_WorldEditor::toggle_edit_mode);

	// --- Camera ---
	ClassDB::bind_method(D_METHOD("set_editor_camera", "camera"), &Z1D_WorldEditor::set_editor_camera);
	ClassDB::bind_method(D_METHOD("get_editor_camera"),           &Z1D_WorldEditor::get_editor_camera);

	ClassDB::bind_method(D_METHOD("set_orbit_target", "target"), &Z1D_WorldEditor::set_orbit_target);
	ClassDB::bind_method(D_METHOD("get_orbit_target"),           &Z1D_WorldEditor::get_orbit_target);

	ClassDB::bind_method(D_METHOD("set_orbit_distance", "distance"), &Z1D_WorldEditor::set_orbit_distance);
	ClassDB::bind_method(D_METHOD("get_orbit_distance"),             &Z1D_WorldEditor::get_orbit_distance);

	ClassDB::bind_method(D_METHOD("focus_on_selection"),         &Z1D_WorldEditor::focus_on_selection);
	ClassDB::bind_method(D_METHOD("focus_on_node", "node"),      &Z1D_WorldEditor::focus_on_node);

	// --- Gizmo ---
	ClassDB::bind_method(D_METHOD("set_gizmo", "gizmo"), &Z1D_WorldEditor::set_gizmo);
	ClassDB::bind_method(D_METHOD("get_gizmo"),          &Z1D_WorldEditor::get_gizmo);

	// --- Viewport ---
	ClassDB::bind_method(D_METHOD("set_linked_viewport", "viewport"), &Z1D_WorldEditor::set_linked_viewport);
	ClassDB::bind_method(D_METHOD("get_linked_viewport"),             &Z1D_WorldEditor::get_linked_viewport);

	// --- Selection Settings ---
	ClassDB::bind_method(D_METHOD("set_drag_select", "enabled"), &Z1D_WorldEditor::set_drag_select);
	ClassDB::bind_method(D_METHOD("get_drag_select"),            &Z1D_WorldEditor::get_drag_select);

	ClassDB::bind_method(D_METHOD("set_selection_mode", "mode"), &Z1D_WorldEditor::set_selection_mode);
	ClassDB::bind_method(D_METHOD("get_selection_mode"),         &Z1D_WorldEditor::get_selection_mode);

	ClassDB::bind_method(D_METHOD("set_transform_mode", "mode"), &Z1D_WorldEditor::set_transform_mode);
	ClassDB::bind_method(D_METHOD("get_transform_mode"),         &Z1D_WorldEditor::get_transform_mode);

	// --- Selection API ---
	ClassDB::bind_method(D_METHOD("select_node", "node"),      &Z1D_WorldEditor::select_node);
	ClassDB::bind_method(D_METHOD("deselect_node", "node"),    &Z1D_WorldEditor::deselect_node);
	ClassDB::bind_method(D_METHOD("clear_selection"),          &Z1D_WorldEditor::clear_selection);
	ClassDB::bind_method(D_METHOD("is_node_selected", "node"), &Z1D_WorldEditor::is_node_selected);
	ClassDB::bind_method(D_METHOD("get_selected_nodes"),       &Z1D_WorldEditor::get_selected_nodes);

	// --- Utility ---
	ClassDB::bind_method(D_METHOD("delete_selected_nodes"),    &Z1D_WorldEditor::delete_selected_nodes);
	ClassDB::bind_method(D_METHOD("duplicate_selected_nodes"), &Z1D_WorldEditor::duplicate_selected_nodes);

	// --- Properties ---
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "edit_mode"), "set_edit_mode", "get_edit_mode");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "editor_camera", PROPERTY_HINT_NODE_TYPE, "Camera3D"), "set_editor_camera", "get_editor_camera");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gizmo", PROPERTY_HINT_NODE_TYPE, "Z3D_Gizmo"), "set_gizmo", "get_gizmo");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "linked_viewport", PROPERTY_HINT_NODE_TYPE, "SubViewport"), "set_linked_viewport", "get_linked_viewport");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "drag_select"), "set_drag_select", "get_drag_select");

	ADD_GROUP("Camera", "orbit_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "orbit_target"), "set_orbit_target", "get_orbit_target");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "orbit_distance", PROPERTY_HINT_RANGE, "0.1,1000,0.1"), "set_orbit_distance", "get_orbit_distance");

	ADD_GROUP("Selection", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "selection_mode", PROPERTY_HINT_ENUM, "Single,Multi"), "set_selection_mode", "get_selection_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "transform_mode", PROPERTY_HINT_ENUM, "Local,Global"), "set_transform_mode", "get_transform_mode");

	// --- Signals ---
	// Hover events
	ADD_SIGNAL(MethodInfo("node_mouse_over",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node3D"),
		PropertyInfo(Variant::BOOL, "over")));

	// Selection request signals (emitted before selection happens — can be intercepted)
	ADD_SIGNAL(MethodInfo("request_node_select",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node3D")));

	ADD_SIGNAL(MethodInfo("request_node_deselect",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node3D")));

	// Selection changed signals (emitted after selection state changes)
	ADD_SIGNAL(MethodInfo("node_selected",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node3D")));

	ADD_SIGNAL(MethodInfo("node_deselected",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Node3D")));

	ADD_SIGNAL(MethodInfo("selection_changed"));

	// Edit mode
	ADD_SIGNAL(MethodInfo("edit_mode_changed",
		PropertyInfo(Variant::BOOL, "active")));

	// Drag selection
	ADD_SIGNAL(MethodInfo("drag_select_started",
		PropertyInfo(Variant::VECTOR2, "start_pos")));

	ADD_SIGNAL(MethodInfo("drag_select_ended",
		PropertyInfo(Variant::RECT2, "rect")));

	// --- Enums ---
	BIND_ENUM_CONSTANT(SELECT_SINGLE);
	BIND_ENUM_CONSTANT(SELECT_MULTI);
	BIND_ENUM_CONSTANT(TRANSFORM_LOCAL);
	BIND_ENUM_CONSTANT(TRANSFORM_GLOBAL);
}


// ---------------------------------------------------------------------------
// Notification
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (!gizmo) {
				_create_default_gizmo();
			}
			set_process_input(true);
		} break;

		case NOTIFICATION_PROCESS: {
			if (edit_mode_active) {
				// Update gizmo position to match selection, etc.
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			_destroy_default_gizmo();
		} break;
	}
}


// ---------------------------------------------------------------------------
// Edit Mode
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::set_edit_mode(bool p_enabled) {
	if (edit_mode_active == p_enabled) {
		return;
	}
	edit_mode_active = p_enabled;

	if (editor_camera) {
		editor_camera->set_current(edit_mode_active);
	}

	if (gizmo) {
		// gizmo->set_visible(edit_mode_active);
	}

	set_process(edit_mode_active);
	emit_signal("edit_mode_changed", edit_mode_active);
}

bool Z1D_WorldEditor::get_edit_mode() const {
	return edit_mode_active;
}

void Z1D_WorldEditor::toggle_edit_mode() {
	set_edit_mode(!edit_mode_active);
}


// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::set_editor_camera(Camera3D *p_camera) {
	editor_camera = p_camera;
	if (editor_camera && edit_mode_active) {
		_update_camera_transform();
	}
}

Camera3D *Z1D_WorldEditor::get_editor_camera() const {
	return editor_camera;
}

void Z1D_WorldEditor::set_orbit_target(const Vector3 &p_target) {
	orbit_target = p_target;
	_update_camera_transform();
}

Vector3 Z1D_WorldEditor::get_orbit_target() const {
	return orbit_target;
}

void Z1D_WorldEditor::set_orbit_distance(float p_distance) {
	orbit_distance = MAX(0.1f, p_distance);
	_update_camera_transform();
}

float Z1D_WorldEditor::get_orbit_distance() const {
	return orbit_distance;
}

void Z1D_WorldEditor::_update_camera_transform() {
	if (!editor_camera) {
		return;
	}

	// Spherical to Cartesian
	Vector3 offset;
	offset.x = orbit_distance * cos(orbit_pitch) * sin(orbit_yaw);
	offset.y = orbit_distance * sin(orbit_pitch);
	offset.z = orbit_distance * cos(orbit_pitch) * cos(orbit_yaw);

	Vector3 cam_pos = orbit_target + offset;
	editor_camera->set_global_position(cam_pos);
	editor_camera->look_at(orbit_target, Vector3(0, 1, 0));
}

void Z1D_WorldEditor::focus_on_selection() {
	if (selected_nodes.is_empty()) {
		return;
	}

	// Calculate bounding center of selection
	Vector3 center;
	for (Node3D *node : selected_nodes) {
		center += node->get_global_position();
	}
	center /= selected_nodes.size();

	set_orbit_target(center);
}

void Z1D_WorldEditor::focus_on_node(Node3D *p_node) {
	if (p_node) {
		set_orbit_target(p_node->get_global_position());
	}
}


// ---------------------------------------------------------------------------
// Gizmo
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::set_gizmo(Z3D_Gizmo *p_gizmo) {
	if (p_gizmo) {
		_destroy_default_gizmo();
	}
	gizmo = p_gizmo;
}

Z3D_Gizmo *Z1D_WorldEditor::get_gizmo() const {
	return gizmo;
}

void Z1D_WorldEditor::_create_default_gizmo() {
	// TODO: Instantiate default Z3D_Gizmo and add as child
	// default_gizmo = memnew(Z3D_Gizmo);
	// add_child(default_gizmo);
	// gizmo = default_gizmo;
}

void Z1D_WorldEditor::_destroy_default_gizmo() {
	if (default_gizmo) {
		// default_gizmo->queue_free();
		default_gizmo = nullptr;
		if (gizmo == default_gizmo) {
			gizmo = nullptr;
		}
	}
}


// ---------------------------------------------------------------------------
// Viewport
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::set_linked_viewport(SubViewport *p_viewport) {
	linked_viewport = p_viewport;
}

SubViewport *Z1D_WorldEditor::get_linked_viewport() const {
	return linked_viewport;
}

Viewport *Z1D_WorldEditor::_get_active_viewport() const {
	if (linked_viewport) {
		return linked_viewport;
	}
	return get_viewport();
}


// ---------------------------------------------------------------------------
// Selection Settings
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::set_drag_select(bool p_enabled) {
	drag_select = p_enabled;
}

bool Z1D_WorldEditor::get_drag_select() const {
	return drag_select;
}

void Z1D_WorldEditor::set_selection_mode(SelectionMode p_mode) {
	selection_mode = p_mode;
}

Z1D_WorldEditor::SelectionMode Z1D_WorldEditor::get_selection_mode() const {
	return selection_mode;
}

void Z1D_WorldEditor::set_transform_mode(TransformMode p_mode) {
	transform_mode = p_mode;
}

Z1D_WorldEditor::TransformMode Z1D_WorldEditor::get_transform_mode() const {
	return transform_mode;
}


// ---------------------------------------------------------------------------
// Selection API
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::select_node(Node3D *p_node) {
	if (!p_node || is_node_selected(p_node)) {
		return;
	}

	emit_signal("request_node_select", p_node);

	if (selection_mode == SELECT_SINGLE) {
		clear_selection();
	}

	selected_nodes.push_back(p_node);
	emit_signal("node_selected", p_node);
	emit_signal("selection_changed");
}

void Z1D_WorldEditor::deselect_node(Node3D *p_node) {
	int idx = selected_nodes.find(p_node);
	if (idx == -1) {
		return;
	}

	emit_signal("request_node_deselect", p_node);

	selected_nodes.remove_at(idx);
	emit_signal("node_deselected", p_node);
	emit_signal("selection_changed");
}

void Z1D_WorldEditor::clear_selection() {
	while (!selected_nodes.is_empty()) {
		Node3D *node = selected_nodes[selected_nodes.size() - 1];
		selected_nodes.remove_at(selected_nodes.size() - 1);
		emit_signal("node_deselected", node);
	}
	emit_signal("selection_changed");
}

bool Z1D_WorldEditor::is_node_selected(Node3D *p_node) const {
	return selected_nodes.find(p_node) != -1;
}

TypedArray<Node3D> Z1D_WorldEditor::get_selected_nodes() const {
	TypedArray<Node3D> result;
	for (Node3D *node : selected_nodes) {
		result.push_back(node);
	}
	return result;
}


// ---------------------------------------------------------------------------
// Input Processing
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::_process_input(const Ref<InputEvent> &p_event) {
	if (!edit_mode_active) {
		return;
	}

	_process_camera_input(p_event);

	// Mouse motion — hover detection
	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		Node3D *hit = _raycast_node_at(mm->get_position());
		if (hit != hovered_node) {
			if (hovered_node) {
				emit_signal("node_mouse_over", hovered_node, false);
			}
			hovered_node = hit;
			if (hovered_node) {
				emit_signal("node_mouse_over", hovered_node, true);
			}
		}

		// Drag selection update
		if (is_drag_selecting) {
			_update_drag_selection(mm->get_position());
		}
	}

	// Mouse button — selection & drag select
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid() && mb->get_button_index() == MouseButton::LEFT) {
		if (mb->is_pressed()) {
			if (drag_select && !hovered_node) {
				// Start drag selection
				is_drag_selecting = true;
				drag_start_pos = mb->get_position();
				emit_signal("drag_select_started", drag_start_pos);
			} else if (hovered_node) {
				// Click selection
				if (mb->is_shift_pressed() && selection_mode == SELECT_MULTI) {
					if (is_node_selected(hovered_node)) {
						deselect_node(hovered_node);
					} else {
						select_node(hovered_node);
					}
				} else {
					select_node(hovered_node);
				}
			} else {
				// Click on nothing — clear selection
				clear_selection();
			}
		} else {
			// Mouse released
			if (is_drag_selecting) {
				_finalize_drag_selection();
				is_drag_selecting = false;
			}
		}
	}
}

void Z1D_WorldEditor::_process_camera_input(const Ref<InputEvent> &p_event) {
	if (!editor_camera) {
		return;
	}

	// Middle mouse drag — orbit
	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		if (mm->get_button_mask().has_flag(MouseButtonMask::MIDDLE)) {
			if (mm->is_shift_pressed()) {
				// Pan
				Vector3 right = editor_camera->get_global_transform().basis.get_column(0);
				Vector3 up = editor_camera->get_global_transform().basis.get_column(1);
				orbit_target -= right * mm->get_relative().x * pan_speed * orbit_distance;
				orbit_target += up * mm->get_relative().y * pan_speed * orbit_distance;
			} else {
				// Orbit
				orbit_yaw -= mm->get_relative().x * orbit_speed;
				orbit_pitch -= mm->get_relative().y * orbit_speed;
				orbit_pitch = CLAMP(orbit_pitch, -Math::PI * 0.49f, Math::PI * 0.49f);
			}
			_update_camera_transform();
		}
	}

	// Scroll — zoom
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->get_button_index() == MouseButton::WHEEL_UP) {
			orbit_distance /= zoom_speed;
			_update_camera_transform();
		} else if (mb->get_button_index() == MouseButton::WHEEL_DOWN) {
			orbit_distance *= zoom_speed;
			_update_camera_transform();
		}
	}
}

Node3D *Z1D_WorldEditor::_raycast_node_at(const Vector2 &p_screen_pos) {
	// TODO: Implement physics raycast or visual picking
	// Use PhysicsDirectSpaceState3D::intersect_ray() or similar
	// Return the first Node3D hit, or nullptr
	return nullptr;
}

void Z1D_WorldEditor::_update_drag_selection(const Vector2 &p_current_pos) {
	// TODO: Draw selection rectangle overlay
	// Could emit a signal with the current rect for UI to draw
}

void Z1D_WorldEditor::_finalize_drag_selection() {
	// TODO: Raycast all nodes within the drag rect and select them
	Rect2 rect = Rect2(drag_start_pos, Vector2()).expand(drag_start_pos);
	// ... expand to current mouse pos, select intersecting nodes
	emit_signal("drag_select_ended", rect);
}


// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

void Z1D_WorldEditor::delete_selected_nodes() {
	for (Node3D *node : selected_nodes) {
		node->queue_free();
	}
	selected_nodes.clear();
	emit_signal("selection_changed");
}

void Z1D_WorldEditor::duplicate_selected_nodes() {
	Vector<Node3D*> new_selection;
	for (Node3D *node : selected_nodes) {
		Node *dupe = node->duplicate();
		if (node->get_parent()) {
			node->get_parent()->add_child(dupe);
		}
		Node3D *dupe_3d = Object::cast_to<Node3D>(dupe);
		if (dupe_3d) {
			new_selection.push_back(dupe_3d);
		}
	}

	clear_selection();
	for (Node3D *node : new_selection) {
		select_node(node);
	}
}
