#pragma once

#include "scene/main/node.h"
#include "core/input/input_event.h"
#include "res/Zen_InputAction.h"

class Zen_InputAction;
// ---------------------------------------------------------------------------
// Z1D_Input — A 1D input listener node that uses a Zen_InputAction resource
// to aggregate multiple input actions into a single Vector3 value.
// ---------------------------------------------------------------------------
class Z1D_Input : public Node {
	GDCLASS(Z1D_Input, Node);

	// Configuration
	Ref<Zen_InputAction> input_action;
	bool require_ctrl  = false;
	bool require_alt   = false;
	bool require_shift = false;
	NodePath visibility_link;

	// Internal state
	HashSet<StringName> active_inputs;
	Vector3 current_value;
	bool is_active = false;
	bool modifiers_valid_at_start = false;

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void input(const Ref<InputEvent> &p_event) override;

	void _process_input(const Ref<InputEvent> &p_event);
	bool _check_modifiers() const;
	bool _check_visibility() const;
	void _recalculate_value();

	// Helper to extract key name from input event (e.g. "KEY_W", "JOY_BUTTON_0")
	StringName _get_input_name(const Ref<InputEvent> &p_event) const;

public:
	Z1D_Input();

	// Input action resource
	void set_input_action(const Ref<Zen_InputAction> &p_action);
	Ref<Zen_InputAction> get_input_action() const;

	// Modifier requirements
	void set_require_ctrl(bool p_require);
	bool get_require_ctrl() const;

	void set_require_alt(bool p_require);
	bool get_require_alt() const;

	void set_require_shift(bool p_require);
	bool get_require_shift() const;

	// Visibility link
	void set_visibility_link(const NodePath &p_path);
	NodePath get_visibility_link() const;

	// Runtime state accessors
	Vector3 get_current_value() const;
	bool get_is_active() const;

	// Manual control
	void reset();
};
