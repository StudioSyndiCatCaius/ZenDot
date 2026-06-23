#pragma once

#include "core/io/resource.h"

// ---------------------------------------------------------------------------
// Zen_InputAction — A reusable resource that defines input key bindings.
// Maps raw input key/button names (e.g. "KEY_W", "KEY_A", "JOY_BUTTON_0")
// to Vector3 contribution values. Can be shared across multiple Z1D_Input nodes.
//
// Supported key formats:
//   Keyboard:  "KEY_A", "KEY_W", "KEY_SPACE", "KEY_SHIFT", etc.
//   Joypad:    "JOY_BUTTON_0", "JOY_BUTTON_1", etc.
//   Mouse:     "MOUSE_BUTTON_LEFT", "MOUSE_BUTTON_RIGHT", etc.
// ---------------------------------------------------------------------------
class Zen_InputAction : public Resource {
	GDCLASS(Zen_InputAction, Resource);

	Dictionary bindings; // StringName -> Vector3

protected:
	static void _bind_methods();

public:
	Zen_InputAction();
	void set_bindings(const Dictionary &p_bindings);
	Dictionary get_bindings() const;

	// Convenience methods for GDScript
	void set_binding(const StringName &p_action, const Vector3 &p_value);
	void remove_binding(const StringName &p_action);
	Vector3 get_binding(const StringName &p_action) const;
	bool has_binding(const StringName &p_action) const;
	Array get_actions() const;
	void clear();
};
