#include "Zen_InputAction.h"

void Zen_InputAction::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_bindings", "bindings"), &Zen_InputAction::set_bindings);
	ClassDB::bind_method(D_METHOD("get_bindings"),             &Zen_InputAction::get_bindings);

	ClassDB::bind_method(D_METHOD("set_binding", "action", "value"), &Zen_InputAction::set_binding);
	ClassDB::bind_method(D_METHOD("remove_binding", "action"),       &Zen_InputAction::remove_binding);
	ClassDB::bind_method(D_METHOD("get_binding", "action"),          &Zen_InputAction::get_binding);
	ClassDB::bind_method(D_METHOD("has_binding", "action"),          &Zen_InputAction::has_binding);
	ClassDB::bind_method(D_METHOD("get_actions"),                    &Zen_InputAction::get_actions);
	ClassDB::bind_method(D_METHOD("clear"),                          &Zen_InputAction::clear);

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "bindings"), "set_bindings", "get_bindings");
}

void Zen_InputAction::set_bindings(const Dictionary &p_bindings) {
	bindings = p_bindings;
	emit_changed();
}

Dictionary Zen_InputAction::get_bindings() const {
	return bindings;
}

void Zen_InputAction::set_binding(const StringName &p_action, const Vector3 &p_value) {
	bindings[p_action] = p_value;
	emit_changed();
}

void Zen_InputAction::remove_binding(const StringName &p_action) {
	bindings.erase(p_action);
	emit_changed();
}

Vector3 Zen_InputAction::get_binding(const StringName &p_action) const {
	if (bindings.has(p_action)) {
		return bindings[p_action];
	}
	return Vector3();
}

bool Zen_InputAction::has_binding(const StringName &p_action) const {
	return bindings.has(p_action);
}

Array Zen_InputAction::get_actions() const {
	return bindings.keys();
}

void Zen_InputAction::clear() {
	bindings.clear();
	emit_changed();
}
