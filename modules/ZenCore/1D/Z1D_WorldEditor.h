#pragma once

#include "scene/3d/node_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/main/viewport.h"

class Z3D_Gizmo; // Forward declaration — assumes you have this class elsewhere

// ---------------------------------------------------------------------------
// Z1D_WorldEditor
// Base class for a 3D level editor, similar to Godot's built-in scene editor.
// Provides orbital camera controls, gizmo manipulation, node selection, and
// optional SubViewport-relative input handling.
// ---------------------------------------------------------------------------
class Z1D_WorldEditor : public Node3D {
	GDCLASS(Z1D_WorldEditor, Node3D);

public:
	enum SelectionMode {
		SELECT_SINGLE,
		SELECT_MULTI,
	};

	enum TransformMode {
		TRANSFORM_LOCAL,
		TRANSFORM_GLOBAL,
	};

private:
	// --- Core References ---
	Camera3D       *editor_camera     = nullptr;   // Orbital editor camera
	Z3D_Gizmo      *gizmo             = nullptr;   // Transform gizmo (creates default if null)
	SubViewport    *linked_viewport   = nullptr;   // Optional: constrain input to this viewport
	Z3D_Gizmo      *default_gizmo     = nullptr;   // Internally created gizmo if none assigned

	// --- State ---
	bool            edit_mode_active  = false;
	bool            drag_select       = true;
	bool            is_drag_selecting = false;
	Vector2         drag_start_pos;

	// --- Selection ---
	SelectionMode   selection_mode    = SELECT_SINGLE;
	TransformMode   transform_mode    = TRANSFORM_LOCAL;
	Vector<Node3D*> selected_nodes;
	Node3D         *hovered_node      = nullptr;

	// --- Camera Orbit State ---
	float           orbit_distance    = 10.0f;
	float           orbit_yaw         = 0.0f;
	float           orbit_pitch       = -0.4f;  // Slight downward angle
	Vector3         orbit_target      = Vector3();
	float           zoom_speed        = 1.1f;
	float           pan_speed         = 0.01f;
	float           orbit_speed       = 0.005f;

	// --- Internal Methods ---
	void _create_default_gizmo();
	void _destroy_default_gizmo();
	void _update_camera_transform();
	void _process_input(const Ref<InputEvent> &p_event);
	void _process_camera_input(const Ref<InputEvent> &p_event);
	Node3D *_raycast_node_at(const Vector2 &p_screen_pos);
	void _update_drag_selection(const Vector2 &p_current_pos);
	void _finalize_drag_selection();
	Viewport *_get_active_viewport() const;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Z1D_WorldEditor();
	~Z1D_WorldEditor();

	// --- Edit Mode ---
	void set_edit_mode(bool p_enabled);
	bool get_edit_mode() const;
	void toggle_edit_mode();

	// --- Camera ---
	void set_editor_camera(Camera3D *p_camera);
	Camera3D *get_editor_camera() const;

	void set_orbit_target(const Vector3 &p_target);
	Vector3 get_orbit_target() const;

	void set_orbit_distance(float p_distance);
	float get_orbit_distance() const;

	void focus_on_selection();          // Zoom camera to fit selected nodes
	void focus_on_node(Node3D *p_node); // Zoom camera to specific node

	// --- Gizmo ---
	void set_gizmo(Z3D_Gizmo *p_gizmo);
	Z3D_Gizmo *get_gizmo() const;

	// --- Viewport ---
	void set_linked_viewport(SubViewport *p_viewport);
	SubViewport *get_linked_viewport() const;

	// --- Selection ---
	void set_drag_select(bool p_enabled);
	bool get_drag_select() const;

	void set_selection_mode(SelectionMode p_mode);
	SelectionMode get_selection_mode() const;

	void set_transform_mode(TransformMode p_mode);
	TransformMode get_transform_mode() const;

	// Selection API (mimics GraphEdit pattern)
	void select_node(Node3D *p_node);
	void deselect_node(Node3D *p_node);
	void clear_selection();
	bool is_node_selected(Node3D *p_node) const;
	TypedArray<Node3D> get_selected_nodes() const;

	// --- Utility ---
	void delete_selected_nodes();
	void duplicate_selected_nodes();
};

VARIANT_ENUM_CAST(Z1D_WorldEditor::SelectionMode);
VARIANT_ENUM_CAST(Z1D_WorldEditor::TransformMode);
