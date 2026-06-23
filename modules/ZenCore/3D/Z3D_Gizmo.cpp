#include "Z3D_Gizmo.h"
#include "scene/main/viewport.h"
#include "core/input/input.h"

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

Z3D_Gizmo::Z3D_Gizmo() {
	set_notify_transform(true);
	_create_materials();

	// Create mesh instance for gizmo visuals
	axis_mesh_instance = memnew(MeshInstance3D);
	axis_mesh_instance->set_name("_GizmoMesh");
	axis_mesh_instance->set_cast_shadows_setting(GeometryInstance3D::SHADOW_CASTING_SETTING_OFF);
	add_child(axis_mesh_instance, false, INTERNAL_MODE_FRONT);

	axis_mesh.instantiate();
	axis_mesh_instance->set_mesh(axis_mesh);
}

Z3D_Gizmo::~Z3D_Gizmo() {
	// Children are freed automatically
}

// ---------------------------------------------------------------------------
// Binding
// ---------------------------------------------------------------------------

void Z3D_Gizmo::_bind_methods() {
	// Selection
	ClassDB::bind_method(D_METHOD("set_selected_nodes", "nodes"), &Z3D_Gizmo::set_selected_nodes);
	ClassDB::bind_method(D_METHOD("get_selected_nodes"), &Z3D_Gizmo::get_selected_nodes);
	ClassDB::bind_method(D_METHOD("add_selected_node", "node"), &Z3D_Gizmo::add_selected_node);
	ClassDB::bind_method(D_METHOD("remove_selected_node", "node"), &Z3D_Gizmo::remove_selected_node);
	ClassDB::bind_method(D_METHOD("clear_selection"), &Z3D_Gizmo::clear_selection);
	ClassDB::bind_method(D_METHOD("has_selection"), &Z3D_Gizmo::has_selection);
	ClassDB::bind_method(D_METHOD("get_selection_count"), &Z3D_Gizmo::get_selection_count);

	// Multiselect
	ClassDB::bind_method(D_METHOD("set_multiselect_enabled", "enabled"), &Z3D_Gizmo::set_multiselect_enabled);
	ClassDB::bind_method(D_METHOD("get_multiselect_enabled"), &Z3D_Gizmo::get_multiselect_enabled);

	// Mode
	ClassDB::bind_method(D_METHOD("set_gizmo_mode", "mode"), &Z3D_Gizmo::set_gizmo_mode);
	ClassDB::bind_method(D_METHOD("get_gizmo_mode"), &Z3D_Gizmo::get_gizmo_mode);

	// Space
	ClassDB::bind_method(D_METHOD("set_gizmo_space", "space"), &Z3D_Gizmo::set_gizmo_space);
	ClassDB::bind_method(D_METHOD("get_gizmo_space"), &Z3D_Gizmo::get_gizmo_space);

	// Scale
	ClassDB::bind_method(D_METHOD("set_gizmo_scale", "scale"), &Z3D_Gizmo::set_gizmo_scale);
	ClassDB::bind_method(D_METHOD("get_gizmo_scale"), &Z3D_Gizmo::get_gizmo_scale);
	ClassDB::bind_method(D_METHOD("set_auto_scale_with_distance", "auto_scale"), &Z3D_Gizmo::set_auto_scale_with_distance);
	ClassDB::bind_method(D_METHOD("get_auto_scale_with_distance"), &Z3D_Gizmo::get_auto_scale_with_distance);

	// Camera
	ClassDB::bind_method(D_METHOD("set_active_camera", "camera"), &Z3D_Gizmo::set_active_camera);
	ClassDB::bind_method(D_METHOD("get_active_camera"), &Z3D_Gizmo::get_active_camera);

	// SubViewport support
	ClassDB::bind_method(D_METHOD("set_viewport_offset", "offset"), &Z3D_Gizmo::set_viewport_offset);
	ClassDB::bind_method(D_METHOD("get_viewport_offset"), &Z3D_Gizmo::get_viewport_offset);
	ClassDB::bind_method(D_METHOD("set_viewport_scale", "scale"), &Z3D_Gizmo::set_viewport_scale);
	ClassDB::bind_method(D_METHOD("get_viewport_scale"), &Z3D_Gizmo::get_viewport_scale);
	ClassDB::bind_method(D_METHOD("set_use_local_coords", "use_local"), &Z3D_Gizmo::set_use_local_coords);
	ClassDB::bind_method(D_METHOD("get_use_local_coords"), &Z3D_Gizmo::get_use_local_coords);

	// Input
	ClassDB::bind_method(D_METHOD("handle_input", "event"), &Z3D_Gizmo::handle_input);
	ClassDB::bind_method(D_METHOD("refresh_gizmo"), &Z3D_Gizmo::refresh_gizmo);

	// Properties
	ADD_GROUP("Selection", "");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "selected_nodes", PROPERTY_HINT_ARRAY_TYPE, "Node3D"), "set_selected_nodes", "get_selected_nodes");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "multiselect_enabled"), "set_multiselect_enabled", "get_multiselect_enabled");

	ADD_GROUP("Gizmo", "gizmo_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "gizmo_mode", PROPERTY_HINT_ENUM, "Translate,Rotate,Scale"), "set_gizmo_mode", "get_gizmo_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "gizmo_space", PROPERTY_HINT_ENUM, "Local,Global"), "set_gizmo_space", "get_gizmo_space");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gizmo_scale", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_gizmo_scale", "get_gizmo_scale");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_scale_with_distance"), "set_auto_scale_with_distance", "get_auto_scale_with_distance");

	ADD_GROUP("Viewport", "");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "viewport_offset"), "set_viewport_offset", "get_viewport_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "viewport_scale"), "set_viewport_scale", "get_viewport_scale");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_local_coords"), "set_use_local_coords", "get_use_local_coords");

	// Enums
	BIND_ENUM_CONSTANT(MODE_TRANSLATE);
	BIND_ENUM_CONSTANT(MODE_ROTATE);
	BIND_ENUM_CONSTANT(MODE_SCALE);
	BIND_ENUM_CONSTANT(SPACE_LOCAL);
	BIND_ENUM_CONSTANT(SPACE_GLOBAL);

	// Signals
	ADD_SIGNAL(MethodInfo("selection_changed"));
	ADD_SIGNAL(MethodInfo("transform_started"));
	ADD_SIGNAL(MethodInfo("transform_changed", PropertyInfo(Variant::TRANSFORM3D, "delta")));
	ADD_SIGNAL(MethodInfo("transform_ended"));
}

// ---------------------------------------------------------------------------
// Notification
// ---------------------------------------------------------------------------

void Z3D_Gizmo::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			_rebuild_gizmo_mesh();
			_update_visibility();
		} break;

		case NOTIFICATION_PROCESS: {
			if (has_selection()) {
				_update_gizmo_transform();
				if (auto_scale_with_distance) {
					_rebuild_gizmo_mesh();
				}
			}
		} break;

		case NOTIFICATION_ENTER_TREE: {
			set_process(true);
		} break;
	}
}

// ---------------------------------------------------------------------------
// Materials
// ---------------------------------------------------------------------------

void Z3D_Gizmo::_create_materials() {
	auto create_mat = [](const Color &col, bool transparent = false) -> Ref<StandardMaterial3D> {
		Ref<StandardMaterial3D> mat;
		mat.instantiate();
		mat->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
		mat->set_flag(BaseMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
		mat->set_cull_mode(BaseMaterial3D::CULL_DISABLED); // Double-sided
		mat->set_albedo(col);
		if (transparent) {
			mat->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
		}
		mat->set_render_priority(100); // Render on top
		return mat;
	};

	mat_x = create_mat(color_x);
	mat_y = create_mat(color_y);
	mat_z = create_mat(color_z);
	mat_x_highlight = create_mat(color_highlight);
	mat_y_highlight = create_mat(color_highlight);
	mat_z_highlight = create_mat(color_highlight);

	Color plane_x = color_x;
	plane_x.a = 0.3f;
	Color plane_y = color_y;
	plane_y.a = 0.3f;
	Color plane_z = color_z;
	plane_z.a = 0.3f;

	mat_plane_yz = create_mat(plane_x, true);
	mat_plane_xz = create_mat(plane_y, true);
	mat_plane_xy = create_mat(plane_z, true);

	Color highlight_alpha = color_highlight;
	highlight_alpha.a = 0.5f;
	mat_plane_xy_highlight = create_mat(highlight_alpha, true);
	mat_plane_xz_highlight = create_mat(highlight_alpha, true);
	mat_plane_yz_highlight = create_mat(highlight_alpha, true);

	mat_center = create_mat(Color(1, 1, 1));
}

// ---------------------------------------------------------------------------
// Mesh building
// ---------------------------------------------------------------------------

void Z3D_Gizmo::_rebuild_gizmo_mesh() {
	if (!axis_mesh.is_valid()) {
		return;
	}

	axis_mesh->clear_surfaces();

	if (!has_selection()) {
		return;
	}

	switch (current_mode) {
		case MODE_TRANSLATE:
			_draw_translate_handles();
			break;
		case MODE_ROTATE:
			_draw_rotate_handles();
			break;
		case MODE_SCALE:
			_draw_scale_handles();
			break;
	}
}

float Z3D_Gizmo::_get_gizmo_scale_factor() const {
	if (!auto_scale_with_distance || !active_camera) {
		return 1.0f;
	}

	Vector3 gizmo_pos = get_global_position();
	Vector3 cam_pos = active_camera->get_global_position();
	float distance = gizmo_pos.distance_to(cam_pos);

	float fov = active_camera->get_fov();
	float factor = distance * Math::tan(Math::deg_to_rad(fov * 0.5f)) * base_gizmo_size;

	return CLAMP(factor, 0.1f, 10.0f);
}

void Z3D_Gizmo::_draw_translate_handles() {
	float scale = _get_gizmo_scale_factor() * gizmo_scale;
	float axis_length = 1.0f * scale;
	float arrow_size = 0.1f * scale;
	float plane_size = 0.3f * scale;
	float plane_offset = 0.3f * scale;

	// X axis
	axis_mesh->surface_begin(Mesh::PRIMITIVE_LINES, hovered_axis == AXIS_X ? mat_x_highlight : mat_x);
	axis_mesh->surface_add_vertex(Vector3(0, 0, 0));
	axis_mesh->surface_add_vertex(Vector3(axis_length, 0, 0));
	axis_mesh->surface_end();

	// X arrow head
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, hovered_axis == AXIS_X ? mat_x_highlight : mat_x);
	Vector3 x_tip = Vector3(axis_length + arrow_size * 2, 0, 0);
	axis_mesh->surface_add_vertex(x_tip);
	axis_mesh->surface_add_vertex(Vector3(axis_length, arrow_size, 0));
	axis_mesh->surface_add_vertex(Vector3(axis_length, -arrow_size, 0));
	axis_mesh->surface_add_vertex(x_tip);
	axis_mesh->surface_add_vertex(Vector3(axis_length, 0, arrow_size));
	axis_mesh->surface_add_vertex(Vector3(axis_length, 0, -arrow_size));
	axis_mesh->surface_end();

	// Y axis
	axis_mesh->surface_begin(Mesh::PRIMITIVE_LINES, hovered_axis == AXIS_Y ? mat_y_highlight : mat_y);
	axis_mesh->surface_add_vertex(Vector3(0, 0, 0));
	axis_mesh->surface_add_vertex(Vector3(0, axis_length, 0));
	axis_mesh->surface_end();

	// Y arrow head
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, hovered_axis == AXIS_Y ? mat_y_highlight : mat_y);
	Vector3 y_tip = Vector3(0, axis_length + arrow_size * 2, 0);
	axis_mesh->surface_add_vertex(y_tip);
	axis_mesh->surface_add_vertex(Vector3(arrow_size, axis_length, 0));
	axis_mesh->surface_add_vertex(Vector3(-arrow_size, axis_length, 0));
	axis_mesh->surface_add_vertex(y_tip);
	axis_mesh->surface_add_vertex(Vector3(0, axis_length, arrow_size));
	axis_mesh->surface_add_vertex(Vector3(0, axis_length, -arrow_size));
	axis_mesh->surface_end();

	// Z axis
	axis_mesh->surface_begin(Mesh::PRIMITIVE_LINES, hovered_axis == AXIS_Z ? mat_z_highlight : mat_z);
	axis_mesh->surface_add_vertex(Vector3(0, 0, 0));
	axis_mesh->surface_add_vertex(Vector3(0, 0, axis_length));
	axis_mesh->surface_end();

	// Z arrow head
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, hovered_axis == AXIS_Z ? mat_z_highlight : mat_z);
	Vector3 z_tip = Vector3(0, 0, axis_length + arrow_size * 2);
	axis_mesh->surface_add_vertex(z_tip);
	axis_mesh->surface_add_vertex(Vector3(arrow_size, 0, axis_length));
	axis_mesh->surface_add_vertex(Vector3(-arrow_size, 0, axis_length));
	axis_mesh->surface_add_vertex(z_tip);
	axis_mesh->surface_add_vertex(Vector3(0, arrow_size, axis_length));
	axis_mesh->surface_add_vertex(Vector3(0, -arrow_size, axis_length));
	axis_mesh->surface_end();

	// XY plane handle (Z color, double-sided via material)
	Ref<StandardMaterial3D> xy_mat = hovered_axis == AXIS_XY ? mat_plane_xy_highlight : mat_plane_xy;
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, xy_mat);
	axis_mesh->surface_add_vertex(Vector3(plane_offset, plane_offset, 0));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, plane_offset, 0));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, plane_offset + plane_size, 0));
	axis_mesh->surface_add_vertex(Vector3(plane_offset, plane_offset, 0));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, plane_offset + plane_size, 0));
	axis_mesh->surface_add_vertex(Vector3(plane_offset, plane_offset + plane_size, 0));
	axis_mesh->surface_end();

	// XZ plane handle (Y color)
	Ref<StandardMaterial3D> xz_mat = hovered_axis == AXIS_XZ ? mat_plane_xz_highlight : mat_plane_xz;
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, xz_mat);
	axis_mesh->surface_add_vertex(Vector3(plane_offset, 0, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, 0, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, 0, plane_offset + plane_size));
	axis_mesh->surface_add_vertex(Vector3(plane_offset, 0, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(plane_offset + plane_size, 0, plane_offset + plane_size));
	axis_mesh->surface_add_vertex(Vector3(plane_offset, 0, plane_offset + plane_size));
	axis_mesh->surface_end();

	// YZ plane handle (X color)
	Ref<StandardMaterial3D> yz_mat = hovered_axis == AXIS_YZ ? mat_plane_yz_highlight : mat_plane_yz;
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, yz_mat);
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset + plane_size, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset + plane_size, plane_offset + plane_size));
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset, plane_offset));
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset + plane_size, plane_offset + plane_size));
	axis_mesh->surface_add_vertex(Vector3(0, plane_offset, plane_offset + plane_size));
	axis_mesh->surface_end();
}

void Z3D_Gizmo::_draw_rotate_handles() {
	float scale = _get_gizmo_scale_factor() * gizmo_scale;
	float radius = 1.0f * scale;
	int segments = 64;

	auto draw_circle = [&](const Vector3 &axis, const Ref<StandardMaterial3D> &mat) {
		axis_mesh->surface_begin(Mesh::PRIMITIVE_LINE_STRIP, mat);
		for (int i = 0; i <= segments; i++) {
			float angle = (float)i / segments * Math::TAU;
			Vector3 point;
			if (axis == Vector3(1, 0, 0)) {
				point = Vector3(0, Math::cos(angle), Math::sin(angle)) * radius;
			} else if (axis == Vector3(0, 1, 0)) {
				point = Vector3(Math::cos(angle), 0, Math::sin(angle)) * radius;
			} else {
				point = Vector3(Math::cos(angle), Math::sin(angle), 0) * radius;
			}
			axis_mesh->surface_add_vertex(point);
		}
		axis_mesh->surface_end();
	};

	draw_circle(Vector3(1, 0, 0), hovered_axis == AXIS_X ? mat_x_highlight : mat_x);
	draw_circle(Vector3(0, 1, 0), hovered_axis == AXIS_Y ? mat_y_highlight : mat_y);
	draw_circle(Vector3(0, 0, 1), hovered_axis == AXIS_Z ? mat_z_highlight : mat_z);
}

void Z3D_Gizmo::_draw_scale_handles() {
	float scale = _get_gizmo_scale_factor() * gizmo_scale;
	float axis_length = 1.0f * scale;
	float box_size = 0.08f * scale;

	auto draw_axis_with_box = [&](const Vector3 &dir, const Ref<StandardMaterial3D> &mat) {
		Vector3 end = dir * axis_length;

		// Line
		axis_mesh->surface_begin(Mesh::PRIMITIVE_LINES, mat);
		axis_mesh->surface_add_vertex(Vector3(0, 0, 0));
		axis_mesh->surface_add_vertex(end);
		axis_mesh->surface_end();

		// Box at end
		Vector3 up = dir.cross(Vector3(1, 0, 0)).normalized();
		if (up.length_squared() < 0.01f) {
			up = dir.cross(Vector3(0, 1, 0)).normalized();
		}
		Vector3 right = dir.cross(up).normalized();

		axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, mat);
		Vector3 corners[8];
		for (int i = 0; i < 8; i++) {
			Vector3 offset;
			offset += ((i & 1) ? 1 : -1) * right * box_size;
			offset += ((i & 2) ? 1 : -1) * up * box_size;
			offset += ((i & 4) ? 1 : -1) * dir * box_size;
			corners[i] = end + offset;
		}
		// Front face
		axis_mesh->surface_add_vertex(corners[4]);
		axis_mesh->surface_add_vertex(corners[5]);
		axis_mesh->surface_add_vertex(corners[7]);
		axis_mesh->surface_add_vertex(corners[4]);
		axis_mesh->surface_add_vertex(corners[7]);
		axis_mesh->surface_add_vertex(corners[6]);
		axis_mesh->surface_end();
	};

	draw_axis_with_box(Vector3(1, 0, 0), hovered_axis == AXIS_X ? mat_x_highlight : mat_x);
	draw_axis_with_box(Vector3(0, 1, 0), hovered_axis == AXIS_Y ? mat_y_highlight : mat_y);
	draw_axis_with_box(Vector3(0, 0, 1), hovered_axis == AXIS_Z ? mat_z_highlight : mat_z);

	// Center box for uniform scale
	float center_size = 0.1f * scale;
	axis_mesh->surface_begin(Mesh::PRIMITIVE_TRIANGLES, hovered_axis == AXIS_CENTER ? mat_center : mat_center);
	Vector3 cs = Vector3(center_size, center_size, center_size);
	axis_mesh->surface_add_vertex(Vector3(-cs.x, -cs.y, cs.z));
	axis_mesh->surface_add_vertex(Vector3(cs.x, -cs.y, cs.z));
	axis_mesh->surface_add_vertex(Vector3(cs.x, cs.y, cs.z));
	axis_mesh->surface_add_vertex(Vector3(-cs.x, -cs.y, cs.z));
	axis_mesh->surface_add_vertex(Vector3(cs.x, cs.y, cs.z));
	axis_mesh->surface_add_vertex(Vector3(-cs.x, cs.y, cs.z));
	axis_mesh->surface_end();
}

// ---------------------------------------------------------------------------
// Transform update
// ---------------------------------------------------------------------------

void Z3D_Gizmo::_update_gizmo_transform() {
	if (!has_selection()) {
		return;
	}

	Vector3 center = Vector3();
	int count = 0;
	for (int i = 0; i < selected_nodes.size(); i++) {
		Node3D *node = Object::cast_to<Node3D>(selected_nodes[i]);
		if (node && node->is_inside_tree()) {
			center += node->get_global_position();
			count++;
		}
	}
	if (count > 0) {
		center /= count;
	}
	set_global_position(center);

	if (current_space == SPACE_LOCAL && count == 1) {
		Node3D *node = Object::cast_to<Node3D>(selected_nodes[0]);
		if (node) {
			set_global_basis(node->get_global_basis());
		}
	} else {
		set_global_basis(Basis());
	}
}

void Z3D_Gizmo::_update_visibility() {
	bool vis = has_selection();
	if (axis_mesh_instance) {
		axis_mesh_instance->set_visible(vis);
	}
	set_visible(vis);
}

// ---------------------------------------------------------------------------
// Coordinate transformation
// ---------------------------------------------------------------------------

Vector2 Z3D_Gizmo::_transform_mouse_position(const Vector2 &p_global_pos) const {
	if (use_local_coords) {
		return p_global_pos;
	}
	return (p_global_pos - viewport_offset) / viewport_scale;
}

// ---------------------------------------------------------------------------
// Selection management
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_selected_nodes(const TypedArray<Node3D> &p_nodes) {
	selected_nodes = p_nodes;
	_update_gizmo_transform();
	_update_visibility();
	_rebuild_gizmo_mesh();
	emit_signal("selection_changed");
}

TypedArray<Node3D> Z3D_Gizmo::get_selected_nodes() const {
	return selected_nodes;
}

void Z3D_Gizmo::add_selected_node(Node3D *p_node) {
	if (!p_node) return;

	if (!multiselect_enabled) {
		selected_nodes.clear();
	}

	for (int i = 0; i < selected_nodes.size(); i++) {
		if (Object::cast_to<Node3D>(selected_nodes[i]) == p_node) {
			return;
		}
	}

	selected_nodes.push_back(p_node);
	_update_gizmo_transform();
	_update_visibility();
	_rebuild_gizmo_mesh();
	emit_signal("selection_changed");
}

void Z3D_Gizmo::remove_selected_node(Node3D *p_node) {
	for (int i = 0; i < selected_nodes.size(); i++) {
		if (Object::cast_to<Node3D>(selected_nodes[i]) == p_node) {
			selected_nodes.remove_at(i);
			_update_gizmo_transform();
			_update_visibility();
			_rebuild_gizmo_mesh();
			emit_signal("selection_changed");
			return;
		}
	}
}

void Z3D_Gizmo::clear_selection() {
	selected_nodes.clear();
	_update_visibility();
	_rebuild_gizmo_mesh();
	emit_signal("selection_changed");
}

bool Z3D_Gizmo::has_selection() const {
	return selected_nodes.size() > 0;
}

int Z3D_Gizmo::get_selection_count() const {
	return selected_nodes.size();
}

// ---------------------------------------------------------------------------
// Multiselect
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_multiselect_enabled(bool p_enabled) {
	multiselect_enabled = p_enabled;
	if (!multiselect_enabled && selected_nodes.size() > 1) {
		Node3D *first = Object::cast_to<Node3D>(selected_nodes[0]);
		selected_nodes.clear();
		if (first) {
			selected_nodes.push_back(first);
		}
		_update_gizmo_transform();
		_rebuild_gizmo_mesh();
		emit_signal("selection_changed");
	}
}

bool Z3D_Gizmo::get_multiselect_enabled() const {
	return multiselect_enabled;
}

// ---------------------------------------------------------------------------
// Mode / Space
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_gizmo_mode(GizmoMode p_mode) {
	if (current_mode != p_mode) {
		current_mode = p_mode;
		_rebuild_gizmo_mesh();
	}
}

Z3D_Gizmo::GizmoMode Z3D_Gizmo::get_gizmo_mode() const {
	return current_mode;
}

void Z3D_Gizmo::set_gizmo_space(GizmoSpace p_space) {
	if (current_space != p_space) {
		current_space = p_space;
		_update_gizmo_transform();
		_rebuild_gizmo_mesh();
	}
}

Z3D_Gizmo::GizmoSpace Z3D_Gizmo::get_gizmo_space() const {
	return current_space;
}

// ---------------------------------------------------------------------------
// Visual settings
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_gizmo_scale(float p_scale) {
	gizmo_scale = CLAMP(p_scale, 0.1f, 5.0f);
	_rebuild_gizmo_mesh();
}

float Z3D_Gizmo::get_gizmo_scale() const {
	return gizmo_scale;
}

void Z3D_Gizmo::set_auto_scale_with_distance(bool p_auto) {
	auto_scale_with_distance = p_auto;
	_rebuild_gizmo_mesh();
}

bool Z3D_Gizmo::get_auto_scale_with_distance() const {
	return auto_scale_with_distance;
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_active_camera(Camera3D *p_camera) {
	active_camera = p_camera;
}

Camera3D *Z3D_Gizmo::get_active_camera() const {
	return active_camera;
}

// ---------------------------------------------------------------------------
// SubViewport support
// ---------------------------------------------------------------------------

void Z3D_Gizmo::set_viewport_offset(const Vector2 &p_offset) {
	viewport_offset = p_offset;
}

Vector2 Z3D_Gizmo::get_viewport_offset() const {
	return viewport_offset;
}

void Z3D_Gizmo::set_viewport_scale(const Vector2 &p_scale) {
	viewport_scale = p_scale;
}

Vector2 Z3D_Gizmo::get_viewport_scale() const {
	return viewport_scale;
}

void Z3D_Gizmo::set_use_local_coords(bool p_use_local) {
	use_local_coords = p_use_local;
}

bool Z3D_Gizmo::get_use_local_coords() const {
	return use_local_coords;
}

// ---------------------------------------------------------------------------
// Input handling
// ---------------------------------------------------------------------------

bool Z3D_Gizmo::handle_input(const Ref<InputEvent> &p_event) {
	if (!has_selection() || !active_camera) {
		return false;
	}

	Ref<InputEventMouseMotion> motion = p_event;
	Ref<InputEventMouseButton> button = p_event;

	if (motion.is_valid()) {
		Vector2 mouse_pos = _transform_mouse_position(motion->get_position());

		if (is_dragging) {
			switch (current_mode) {
				case MODE_TRANSLATE: {
					Vector3 delta;
					if (active_axis == AXIS_X || active_axis == AXIS_Y || active_axis == AXIS_Z) {
						Vector3 axis_dir = get_global_basis().xform(_get_axis_vector(active_axis));
						float proj = _project_mouse_to_axis(mouse_pos, axis_dir, drag_plane_origin);
						float start_proj = _project_mouse_to_axis(drag_start_mouse, axis_dir, drag_plane_origin);
						delta = axis_dir * (proj - start_proj);
					} else {
						Vector3 new_pos = _project_mouse_to_plane(mouse_pos, drag_plane_normal, drag_plane_origin);
						Vector3 start_pos = _project_mouse_to_plane(drag_start_mouse, drag_plane_normal, drag_plane_origin);
						delta = new_pos - start_pos;
					}
					_apply_translation(delta);
				} break;

				case MODE_ROTATE: {
					Vector3 axis = get_global_basis().xform(_get_axis_vector(active_axis));
					Vector2 center_screen = active_camera->unproject_position(get_global_position());
					Vector2 start_dir = (drag_start_mouse - center_screen).normalized();
					Vector2 current_dir = (mouse_pos - center_screen).normalized();
					float angle = start_dir.angle_to(current_dir);
					_apply_rotation(axis, angle);
				} break;

				case MODE_SCALE: {
					Vector2 center_screen = active_camera->unproject_position(get_global_position());
					float start_dist = drag_start_mouse.distance_to(center_screen);
					float current_dist = mouse_pos.distance_to(center_screen);
					float scale_factor = current_dist / MAX(start_dist, 0.001f);

					Vector3 scale_delta;
					if (active_axis == AXIS_CENTER) {
						scale_delta = Vector3(scale_factor, scale_factor, scale_factor);
					} else {
						scale_delta = Vector3(1, 1, 1);
						Vector3 axis = _get_axis_vector(active_axis);
						if (axis.x > 0.5f) scale_delta.x = scale_factor;
						if (axis.y > 0.5f) scale_delta.y = scale_factor;
						if (axis.z > 0.5f) scale_delta.z = scale_factor;
					}
					_apply_scale(scale_delta);
				} break;
			}

			emit_signal("transform_changed", Transform3D());
			return true;
		} else {
			ActiveAxis new_hover = _raycast_axis(mouse_pos);
			if (new_hover != hovered_axis) {
				hovered_axis = new_hover;
				_rebuild_gizmo_mesh();
			}
		}
	}

	if (button.is_valid() && button->get_button_index() == MouseButton::LEFT) {
		Vector2 mouse_pos = _transform_mouse_position(button->get_position());

		if (button->is_pressed()) {
			ActiveAxis hit = _raycast_axis(mouse_pos);
			if (hit != AXIS_NONE) {
				is_dragging = true;
				active_axis = hit;
				drag_start_mouse = mouse_pos;
				drag_plane_origin = get_global_position();

				drag_start_transforms.clear();
				for (int i = 0; i < selected_nodes.size(); i++) {
					Node3D *node = Object::cast_to<Node3D>(selected_nodes[i]);
					if (node) {
						drag_start_transforms.push_back(node->get_global_transform());
					}
				}

				Vector3 cam_dir = (active_camera->get_global_position() - drag_plane_origin).normalized();
				if (active_axis == AXIS_XY) {
					drag_plane_normal = get_global_basis().xform(Vector3(0, 0, 1));
				} else if (active_axis == AXIS_XZ) {
					drag_plane_normal = get_global_basis().xform(Vector3(0, 1, 0));
				} else if (active_axis == AXIS_YZ) {
					drag_plane_normal = get_global_basis().xform(Vector3(1, 0, 0));
				} else {
					Vector3 axis = get_global_basis().xform(_get_axis_vector(active_axis));
					drag_plane_normal = cam_dir.cross(axis).cross(axis).normalized();
				}

				emit_signal("transform_started");
				return true;
			}
		} else {
			if (is_dragging) {
				is_dragging = false;
				active_axis = AXIS_NONE;
				emit_signal("transform_ended");
				return true;
			}
		}
	}

	return false;
}

// ---------------------------------------------------------------------------
// Hit testing
// ---------------------------------------------------------------------------

bool Z3D_Gizmo::_point_in_triangle(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c) const {
	Vector2 v0 = c - a;
	Vector2 v1 = b - a;
	Vector2 v2 = p - a;

	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
	float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

	return (u >= 0) && (v >= 0) && (u + v <= 1);
}

Z3D_Gizmo::ActiveAxis Z3D_Gizmo::_raycast_axis(const Vector2 &p_screen_pos) const {
	if (!active_camera) {
		return AXIS_NONE;
	}

	float scale = _get_gizmo_scale_factor() * gizmo_scale;
	float hit_threshold = 15.0f;

	Vector3 gizmo_pos = get_global_position();
	Basis gizmo_basis = get_global_basis();

	// Check plane handles first (translate mode only) — they take priority
	if (current_mode == MODE_TRANSLATE) {
		float plane_offset = 0.3f * scale;
		float plane_size = 0.3f * scale;

		auto check_plane = [&](const Vector3 &u_axis, const Vector3 &v_axis) -> bool {
			Vector3 corners[4] = {
				gizmo_pos + gizmo_basis.xform(u_axis * plane_offset + v_axis * plane_offset),
				gizmo_pos + gizmo_basis.xform(u_axis * (plane_offset + plane_size) + v_axis * plane_offset),
				gizmo_pos + gizmo_basis.xform(u_axis * (plane_offset + plane_size) + v_axis * (plane_offset + plane_size)),
				gizmo_pos + gizmo_basis.xform(u_axis * plane_offset + v_axis * (plane_offset + plane_size)),
			};

			Vector2 screen_corners[4];
			for (int i = 0; i < 4; i++) {
				screen_corners[i] = active_camera->unproject_position(corners[i]);
			}

			return _point_in_triangle(p_screen_pos, screen_corners[0], screen_corners[1], screen_corners[2]) ||
				   _point_in_triangle(p_screen_pos, screen_corners[0], screen_corners[2], screen_corners[3]);
		};

		// XY plane (moves in X and Y)
		if (check_plane(Vector3(1, 0, 0), Vector3(0, 1, 0))) {
			return AXIS_XY;
		}
		// XZ plane (moves in X and Z)
		if (check_plane(Vector3(1, 0, 0), Vector3(0, 0, 1))) {
			return AXIS_XZ;
		}
		// YZ plane (moves in Y and Z)
		if (check_plane(Vector3(0, 1, 0), Vector3(0, 0, 1))) {
			return AXIS_YZ;
		}
	}

	// Check axis lines
	auto project_and_check_axis = [&](const Vector3 &axis_local) -> float {
		Vector3 axis_world = gizmo_basis.xform(axis_local * scale);
		Vector3 end_world = gizmo_pos + axis_world;

		Vector2 start_screen = active_camera->unproject_position(gizmo_pos);
		Vector2 end_screen = active_camera->unproject_position(end_world);

		Vector2 line = end_screen - start_screen;
		float len = line.length();
		if (len < 1.0f) return 99999.0f;

		Vector2 dir = line / len;
		Vector2 to_point = p_screen_pos - start_screen;
		float proj = to_point.dot(dir);
		proj = CLAMP(proj, 0.0f, len);

		Vector2 closest = start_screen + dir * proj;
		return p_screen_pos.distance_to(closest);
	};

	float dist_x = project_and_check_axis(Vector3(1, 0, 0));
	float dist_y = project_and_check_axis(Vector3(0, 1, 0));
	float dist_z = project_and_check_axis(Vector3(0, 0, 1));

	float min_dist = hit_threshold;
	ActiveAxis result = AXIS_NONE;

	if (dist_x < min_dist) { min_dist = dist_x; result = AXIS_X; }
	if (dist_y < min_dist) { min_dist = dist_y; result = AXIS_Y; }
	if (dist_z < min_dist) { min_dist = dist_z; result = AXIS_Z; }

	// Check center for scale mode
	if (current_mode == MODE_SCALE) {
		Vector2 center_screen = active_camera->unproject_position(gizmo_pos);
		float center_dist = p_screen_pos.distance_to(center_screen);
		if (center_dist < hit_threshold * 1.5f && center_dist < min_dist) {
			result = AXIS_CENTER;
		}
	}

	return result;
}

Vector3 Z3D_Gizmo::_get_axis_vector(ActiveAxis p_axis) const {
	switch (p_axis) {
		case AXIS_X: return Vector3(1, 0, 0);
		case AXIS_Y: return Vector3(0, 1, 0);
		case AXIS_Z: return Vector3(0, 0, 1);
		default: return Vector3();
	}
}

Vector3 Z3D_Gizmo::_project_mouse_to_plane(const Vector2 &p_mouse, const Vector3 &p_plane_normal, const Vector3 &p_plane_origin) const {
	if (!active_camera) return p_plane_origin;

	Vector3 ray_origin = active_camera->project_ray_origin(p_mouse);
	Vector3 ray_dir = active_camera->project_ray_normal(p_mouse);

	float denom = p_plane_normal.dot(ray_dir);
	if (Math::abs(denom) < 0.0001f) {
		return p_plane_origin;
	}

	float t = p_plane_normal.dot(p_plane_origin - ray_origin) / denom;
	return ray_origin + ray_dir * t;
}

float Z3D_Gizmo::_project_mouse_to_axis(const Vector2 &p_mouse, const Vector3 &p_axis_dir, const Vector3 &p_origin) const {
	if (!active_camera) return 0.0f;

	Vector3 ray_origin = active_camera->project_ray_origin(p_mouse);
	Vector3 ray_dir = active_camera->project_ray_normal(p_mouse);

	Vector3 w0 = p_origin - ray_origin;
	float a = p_axis_dir.dot(p_axis_dir);
	float b = p_axis_dir.dot(ray_dir);
	float c = ray_dir.dot(ray_dir);
	float d = p_axis_dir.dot(w0);
	float e = ray_dir.dot(w0);

	float denom = a * c - b * b;
	if (Math::abs(denom) < 0.0001f) {
		return 0.0f;
	}

	float t = (b * e - c * d) / denom;
	return t;
}

void Z3D_Gizmo::_apply_translation(const Vector3 &p_delta) {
	for (int i = 0; i < selected_nodes.size() && i < drag_start_transforms.size(); i++) {
		Node3D *node = Object::cast_to<Node3D>(selected_nodes[i]);
		if (node) {
			Transform3D start = drag_start_transforms[i];
			node->set_global_position(start.origin + p_delta);
		}
	}
}

void Z3D_Gizmo::_apply_rotation(const Vector3 &p_axis, float p_angle) {
	Vector3 pivot = get_global_position();

	for (int i = 0; i < selected_nodes.size() && i < drag_start_transforms.size(); i++) {
		Node3D *node = Object::cast_to<Node3D>(selected_nodes[i]);
		if (node) {
			Transform3D start = drag_start_transforms[i];

			Vector3 offset = start.origin - pivot;
			Basis rotation = Basis(p_axis, p_angle);
			Vector3 new_offset = rotation.xform(offset);
			Basis new_basis = rotation * start.basis;

			node->set_global_position(pivot + new_offset);
			node->set_global_basis(new_basis);
		}
	}
}

void Z3D_Gizmo::_apply_scale(const Vector3 &p_scale_delta) {
	Vector3 pivot = get_global_position();

	for (int i = 0; i < selected_nodes.size() && i < drag_start_transforms.size(); i++) {
		Node3D *node = Object::cast_to<Node3D>(selected_nodes[i]);
		if (node) {
			Transform3D start = drag_start_transforms[i];

			Vector3 offset = start.origin - pivot;
			Vector3 new_offset = offset * p_scale_delta;

			node->set_global_position(pivot + new_offset);
			node->set_scale(start.basis.get_scale() * p_scale_delta);
		}
	}
}

void Z3D_Gizmo::refresh_gizmo() {
	_update_gizmo_transform();
	_update_visibility();
	_rebuild_gizmo_mesh();
}
