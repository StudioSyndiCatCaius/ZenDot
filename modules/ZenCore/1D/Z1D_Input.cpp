#include "Z1D_Input.h"
#include "core/input/input.h"
#include "core/input/input_event.h"
#include "core/os/keyboard.h"
#include "scene/main/canvas_item.h"
#include "scene/3d/node_3d.h"

Z1D_Input::Z1D_Input() {
	set_process_input(true);
	set_process(false); // Only process while input is active
}

void Z1D_Input::_bind_methods() {
	// Input action resource
	ClassDB::bind_method(D_METHOD("set_input_action", "action"), &Z1D_Input::set_input_action);
	ClassDB::bind_method(D_METHOD("get_input_action"),           &Z1D_Input::get_input_action);

	// Modifier requirements
	ClassDB::bind_method(D_METHOD("set_require_ctrl",  "require"), &Z1D_Input::set_require_ctrl);
	ClassDB::bind_method(D_METHOD("get_require_ctrl"),             &Z1D_Input::get_require_ctrl);

	ClassDB::bind_method(D_METHOD("set_require_alt",   "require"), &Z1D_Input::set_require_alt);
	ClassDB::bind_method(D_METHOD("get_require_alt"),              &Z1D_Input::get_require_alt);

	ClassDB::bind_method(D_METHOD("set_require_shift", "require"), &Z1D_Input::set_require_shift);
	ClassDB::bind_method(D_METHOD("get_require_shift"),            &Z1D_Input::get_require_shift);

	// Visibility link
	ClassDB::bind_method(D_METHOD("set_visibility_link", "path"), &Z1D_Input::set_visibility_link);
	ClassDB::bind_method(D_METHOD("get_visibility_link"),         &Z1D_Input::get_visibility_link);

	// Runtime state
	ClassDB::bind_method(D_METHOD("get_current_value"), &Z1D_Input::get_current_value);
	ClassDB::bind_method(D_METHOD("get_is_active"),     &Z1D_Input::get_is_active);

	// Manual control
	ClassDB::bind_method(D_METHOD("reset"), &Z1D_Input::reset);

	// Properties
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "input_action", PROPERTY_HINT_RESOURCE_TYPE, "Zen_InputAction"), "set_input_action", "get_input_action");

	ADD_GROUP("Modifier Requirements", "require_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "require_ctrl"),  "set_require_ctrl",  "get_require_ctrl");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "require_alt"),   "set_require_alt",   "get_require_alt");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "require_shift"), "set_require_shift", "get_require_shift");

	ADD_GROUP("Visibility", "");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "visibility_link"), "set_visibility_link", "get_visibility_link");

	// Signals
	ADD_SIGNAL(MethodInfo("input_started",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Z1D_Input"),
		PropertyInfo(Variant::VECTOR3, "value")));

	ADD_SIGNAL(MethodInfo("input_updated",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Z1D_Input"),
		PropertyInfo(Variant::VECTOR3, "value")));

	ADD_SIGNAL(MethodInfo("input_ended",
		PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_NODE_TYPE, "Z1D_Input"),
		PropertyInfo(Variant::VECTOR3, "value")));
}

void Z1D_Input::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PROCESS: {
			if (is_active) {
				emit_signal(SNAME("input_updated"), this, current_value);
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			if (is_active) {
				is_active = false;
				set_process(false);
				emit_signal(SNAME("input_ended"), this, current_value);
			}
			active_inputs.clear();
			current_value = Vector3();
		} break;
	}
}

void Z1D_Input::input(const Ref<InputEvent> &p_event) {
	_process_input(p_event);
}

void Z1D_Input::_process_input(const Ref<InputEvent> &p_event) {
	if (p_event.is_null() || input_action.is_null()) {
		return;
	}

	// Check visibility constraint first
	if (!_check_visibility()) {
		return;
	}

	// Get the key name from this event
	StringName input_name = _get_input_name(p_event);
	if (input_name == StringName()) {
		return; // Not a supported input type
	}

	// Check if this key is in our bindings
	Dictionary bindings = input_action->get_bindings();
	if (!bindings.has(input_name)) {
		return;
	}

	bool state_changed = false;
	Vector3 old_value = current_value;
	bool was_active = is_active;

	bool pressed = p_event->is_pressed() && !p_event->is_echo();
	bool released = !p_event->is_pressed();

	if (pressed && !active_inputs.has(input_name)) {
		// Input is being pressed
		if (active_inputs.is_empty()) {
			if (!_check_modifiers()) {
				return;
			}
			modifiers_valid_at_start = true;
		}

		active_inputs.insert(input_name);
		state_changed = true;

	} else if (released && active_inputs.has(input_name)) {
		// Input is being released
		active_inputs.erase(input_name);
		state_changed = true;
	}

	if (state_changed) {
		_recalculate_value();

		bool now_active = !active_inputs.is_empty();

		if (!was_active && now_active) {
			is_active = true;
			set_process(true);
			emit_signal(SNAME("input_started"), this, current_value);

		} else if (was_active && !now_active) {
			is_active = false;
			set_process(false);
			modifiers_valid_at_start = false;
			emit_signal(SNAME("input_ended"), this, old_value);
		}
	}
}

StringName Z1D_Input::_get_input_name(const Ref<InputEvent> &p_event) const {
	// Keyboard key
	const InputEventKey *key_event = Object::cast_to<InputEventKey>(*p_event);
	if (key_event) {
		Key keycode = key_event->get_keycode();
		if (keycode == Key::NONE) {
			keycode = key_event->get_physical_keycode();
		}
		if (keycode != Key::NONE) {
			String key_str = keycode_get_string(keycode);
			return StringName("KEY_" + key_str.to_upper().replace(" ", "_"));
		}
		return StringName();
	}

	// Joypad button
	const InputEventJoypadButton *joy_button = Object::cast_to<InputEventJoypadButton>(*p_event);
	if (joy_button) {
		return StringName("JOY_BUTTON_" + itos((int)joy_button->get_button_index()));
	}

	// Mouse button
	const InputEventMouseButton *mouse_button = Object::cast_to<InputEventMouseButton>(*p_event);
	if (mouse_button) {
		switch (mouse_button->get_button_index()) {
			case MouseButton::LEFT:   return SNAME("MOUSE_BUTTON_LEFT");
			case MouseButton::RIGHT:  return SNAME("MOUSE_BUTTON_RIGHT");
			case MouseButton::MIDDLE: return SNAME("MOUSE_BUTTON_MIDDLE");
			default: return StringName("MOUSE_BUTTON_" + itos((int)mouse_button->get_button_index()));
		}
	}

	return StringName();
}

bool Z1D_Input::_check_modifiers() const {
	Input *input = Input::get_singleton();
	if (!input) {
		return true;
	}

	if (require_ctrl && !input->is_key_pressed(Key::CTRL)) {
		return false;
	}
	if (require_alt && !input->is_key_pressed(Key::ALT)) {
		return false;
	}
	if (require_shift && !input->is_key_pressed(Key::SHIFT)) {
		return false;
	}

	return true;
}

bool Z1D_Input::_check_visibility() const {
	if (visibility_link.is_empty()) {
		return true;
	}

	Node *linked_node = get_node_or_null(visibility_link);
	if (!linked_node) {
		return true;
	}

	CanvasItem *canvas_item = Object::cast_to<CanvasItem>(linked_node);
	if (canvas_item) {
		return canvas_item->is_visible_in_tree();
	}

	Node3D *node_3d = Object::cast_to<Node3D>(linked_node);
	if (node_3d) {
		return node_3d->is_visible_in_tree();
	}

	return true;
}

void Z1D_Input::_recalculate_value() {
	current_value = Vector3();

	if (input_action.is_null()) {
		return;
	}

	Dictionary bindings = input_action->get_bindings();

	for (const StringName &action : active_inputs) {
		if (bindings.has(action)) {
			Vector3 contribution = bindings[action];
			current_value += contribution;
		}
	}
}

// ---------------------------------------------------------------------------
// Property setters/getters
// ---------------------------------------------------------------------------

void Z1D_Input::set_input_action(const Ref<Zen_InputAction> &p_action) {
	input_action = p_action;
}

Ref<Zen_InputAction> Z1D_Input::get_input_action() const {
	return input_action;
}

void Z1D_Input::set_require_ctrl(bool p_require) {
	require_ctrl = p_require;
}

bool Z1D_Input::get_require_ctrl() const {
	return require_ctrl;
}

void Z1D_Input::set_require_alt(bool p_require) {
	require_alt = p_require;
}

bool Z1D_Input::get_require_alt() const {
	return require_alt;
}

void Z1D_Input::set_require_shift(bool p_require) {
	require_shift = p_require;
}

bool Z1D_Input::get_require_shift() const {
	return require_shift;
}

void Z1D_Input::set_visibility_link(const NodePath &p_path) {
	visibility_link = p_path;
}

NodePath Z1D_Input::get_visibility_link() const {
	return visibility_link;
}

Vector3 Z1D_Input::get_current_value() const {
	return current_value;
}

bool Z1D_Input::get_is_active() const {
	return is_active;
}

void Z1D_Input::reset() {
	if (is_active) {
		emit_signal(SNAME("input_ended"), this, current_value);
	}
	active_inputs.clear();
	current_value = Vector3();
	is_active = false;
	set_process(false);
	modifiers_valid_at_start = false;
}
