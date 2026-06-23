#pragma once

#include "scene/3d/node_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/main/viewport.h"
#include "scene/resources/immediate_mesh.h"
#include "scene/resources/material.h"
#include "core/input/input_event.h"

// Runtime 3D Transform Gizmo — works like the editor gizmo but at runtime.
// Supports translate, rotate, scale handles with multi-select.
// Works in SubViewports — set viewport_offset if the SubViewport is embedded.
class Z3D_Gizmo : public Node3D {
	GDCLASS(Z3D_Gizmo, Node3D);

public:
	enum GizmoMode {
		MODE_TRANSLATE,
		MODE_ROTATE,
		MODE_SCALE,
	};

	enum GizmoSpace {
		SPACE_LOCAL,
		SPACE_GLOBAL,
	};

private:
	// Selection
	TypedArray<Node3D> selected_nodes;
	bool multiselect_enabled = true;

	// Gizmo state
	GizmoMode current_mode = MODE_TRANSLATE;
	GizmoSpace current_space = SPACE_LOCAL;
	float gizmo_scale = 1.0f;
	bool auto_scale_with_distance = true;
	float base_gizmo_size = 0.15f; // Fraction of screen height

	// Visual components
	MeshInstance3D *axis_mesh_instance = nullptr;
	Ref<ImmediateMesh> axis_mesh;
	Ref<StandardMaterial3D> mat_x;
	Ref<StandardMaterial3D> mat_y;
	Ref<StandardMaterial3D> mat_z;
	Ref<StandardMaterial3D> mat_x_highlight;
	Ref<StandardMaterial3D> mat_y_highlight;
	Ref<StandardMaterial3D> mat_z_highlight;
	Ref<StandardMaterial3D> mat_plane_xy;
	Ref<StandardMaterial3D> mat_plane_xz;
	Ref<StandardMaterial3D> mat_plane_yz;
	Ref<StandardMaterial3D> mat_plane_xy_highlight;
	Ref<StandardMaterial3D> mat_plane_xz_highlight;
	Ref<StandardMaterial3D> mat_plane_yz_highlight;
	Ref<StandardMaterial3D> mat_center;

	// Interaction state
	enum ActiveAxis {
		AXIS_NONE = 0,
		AXIS_X = 1,
		AXIS_Y = 2,
		AXIS_Z = 3,
		AXIS_XY = 4,
		AXIS_XZ = 5,
		AXIS_YZ = 6,
		AXIS_CENTER = 7,
	};
	ActiveAxis hovered_axis = AXIS_NONE;
	ActiveAxis active_axis = AXIS_NONE;
	bool is_dragging = false;
	Vector2 drag_start_mouse;
	Vector3 drag_start_position;
	Vector3 drag_start_scale;
	Basis drag_start_basis;
	TypedArray<Transform3D> drag_start_transforms; // Per-node start transforms
	Vector3 drag_plane_normal;
	Vector3 drag_plane_origin;

	// Camera reference
	Camera3D *active_camera = nullptr;

	// SubViewport support
	Vector2 viewport_offset = Vector2(0, 0);
	Vector2 viewport_scale = Vector2(1, 1);
	bool use_local_coords = false;

	// Colors
	Color color_x = Color(1.0f, 0.2f, 0.2f);
	Color color_y = Color(0.2f, 1.0f, 0.2f);
	Color color_z = Color(0.2f, 0.4f, 1.0f);
	Color color_highlight = Color(1.0f, 1.0f, 0.4f);
	Color color_plane_alpha = Color(1, 1, 1, 0.3f);

	// Internal methods
	void _create_materials();
	void _rebuild_gizmo_mesh();
	void _draw_translate_handles();
	void _draw_rotate_handles();
	void _draw_scale_handles();
	void _update_gizmo_transform();
	void _update_visibility();
	float _get_gizmo_scale_factor() const;

	// Coordinate transformation
	Vector2 _transform_mouse_position(const Vector2 &p_global_pos) const;

	// Interaction
	ActiveAxis _raycast_axis(const Vector2 &p_screen_pos) const;
	bool _point_in_triangle(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c) const;
	Vector3 _get_axis_vector(ActiveAxis p_axis) const;
	Vector3 _project_mouse_to_plane(const Vector2 &p_mouse, const Vector3 &p_plane_normal, const Vector3 &p_plane_origin) const;
	float _project_mouse_to_axis(const Vector2 &p_mouse, const Vector3 &p_axis_dir, const Vector3 &p_origin) const;
	void _apply_translation(const Vector3 &p_delta);
	void _apply_rotation(const Vector3 &p_axis, float p_angle);
	void _apply_scale(const Vector3 &p_scale_delta);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Z3D_Gizmo();
	~Z3D_Gizmo();

	// Selection management
	void set_selected_nodes(const TypedArray<Node3D> &p_nodes);
	TypedArray<Node3D> get_selected_nodes() const;
	void add_selected_node(Node3D *p_node);
	void remove_selected_node(Node3D *p_node);
	void clear_selection();
	bool has_selection() const;
	int get_selection_count() const;

	// Multi-select toggle
	void set_multiselect_enabled(bool p_enabled);
	bool get_multiselect_enabled() const;

	// Gizmo mode
	void set_gizmo_mode(GizmoMode p_mode);
	GizmoMode get_gizmo_mode() const;

	// Transform space
	void set_gizmo_space(GizmoSpace p_space);
	GizmoSpace get_gizmo_space() const;

	// Visual settings
	void set_gizmo_scale(float p_scale);
	float get_gizmo_scale() const;
	void set_auto_scale_with_distance(bool p_auto);
	bool get_auto_scale_with_distance() const;

	// Camera
	void set_active_camera(Camera3D *p_camera);
	Camera3D *get_active_camera() const;

	// SubViewport support
	void set_viewport_offset(const Vector2 &p_offset);
	Vector2 get_viewport_offset() const;
	void set_viewport_scale(const Vector2 &p_scale);
	Vector2 get_viewport_scale() const;
	void set_use_local_coords(bool p_use_local);
	bool get_use_local_coords() const;

	// Input handling
	bool handle_input(const Ref<InputEvent> &p_event);

	// Manual update (usually automatic)
	void refresh_gizmo();
};

VARIANT_ENUM_CAST(Z3D_Gizmo::GizmoMode);
VARIANT_ENUM_CAST(Z3D_Gizmo::GizmoSpace);
