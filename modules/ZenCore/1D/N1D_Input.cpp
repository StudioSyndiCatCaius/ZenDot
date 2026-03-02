#include "N1D_Input.h"
#include "core/core_bind.h"

void N1D_Input::_bind_methods() {
    // Bind signals
    ADD_SIGNAL(MethodInfo("OnInputBegin"));
    ADD_SIGNAL(MethodInfo("OnInputEnd"));
    ADD_SIGNAL(MethodInfo("OnInputUpdate", PropertyInfo(Variant::VECTOR3, "delta")));

    // Bind virtual methods
    GDVIRTUAL_BIND(_ready);
    GDVIRTUAL_BIND(_process, "delta");

    // Bind properties with type hints
    ClassDB::bind_method(D_METHOD("set_key_inputs", "key_inputs"), &N1D_Input::set_key_inputs);
    ClassDB::bind_method(D_METHOD("get_key_inputs"), &N1D_Input::get_key_inputs);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "KeyInputs", PROPERTY_HINT_DICTIONARY_TYPE, "Key;Vector3", PROPERTY_USAGE_DEFAULT, "Dictionary[Key, Vector3]"), "set_key_inputs", "get_key_inputs");

    ClassDB::bind_method(D_METHOD("set_mouse_inputs", "mouse_inputs"), &N1D_Input::set_mouse_inputs);
    ClassDB::bind_method(D_METHOD("get_mouse_inputs"), &N1D_Input::get_mouse_inputs);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "MouseInputs", PROPERTY_HINT_DICTIONARY_TYPE, "MouseButton;Vector3", PROPERTY_USAGE_DEFAULT, "Dictionary[MouseButton, Vector3]"), "set_mouse_inputs", "get_mouse_inputs");

    ClassDB::bind_method(D_METHOD("set_joy_button_inputs", "joy_button_inputs"), &N1D_Input::set_joy_button_inputs);
    ClassDB::bind_method(D_METHOD("get_joy_button_inputs"), &N1D_Input::get_joy_button_inputs);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "JoyButtonInputs", PROPERTY_HINT_DICTIONARY_TYPE, "JoyButton;Vector3", PROPERTY_USAGE_DEFAULT, "Dictionary[JoyButton, Vector3]"), "set_joy_button_inputs", "get_joy_button_inputs");

    ClassDB::bind_method(D_METHOD("set_joy_axis_inputs", "joy_axis_inputs"), &N1D_Input::set_joy_axis_inputs);
    ClassDB::bind_method(D_METHOD("get_joy_axis_inputs"), &N1D_Input::get_joy_axis_inputs);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "JoyAxisInputs", PROPERTY_HINT_DICTIONARY_TYPE, "JoyAxis;Vector3", PROPERTY_USAGE_DEFAULT, "Dictionary[JoyAxis, Vector3]"), "set_joy_axis_inputs", "get_joy_axis_inputs");

    ClassDB::bind_method(D_METHOD("set_max_value", "max_value"), &N1D_Input::set_max_value);
    ClassDB::bind_method(D_METHOD("get_max_value"), &N1D_Input::get_max_value);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "MaxValue"), "set_max_value", "get_max_value");

    // Bind public methods
    ClassDB::bind_method(D_METHOD("is_input_down"), &N1D_Input::is_input_down);
    ClassDB::bind_method(D_METHOD("get_input_axis"), &N1D_Input::get_input_axis);
}

N1D_Input::N1D_Input() {
    max_value = Vector3(1, 1, 1);
    previous_input = Vector3();
    current_input = Vector3();
    is_active = false;
}

N1D_Input::~N1D_Input() {
}

void N1D_Input::_ready() {
    set_process(true);
}

void N1D_Input::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
        	_ready();
        	break;
        }
        case NOTIFICATION_PROCESS: {
        	_process(get_process_delta_time());
        	break;
        }
    }
}

void N1D_Input::_process(double delta) {
    current_input = Vector3();
    Input *input = Input::get_singleton();

    // Process keyboard inputs - validate types
    Array key_array = key_inputs.keys();
    for (int i = 0; i < key_array.size(); i++) {
        Variant key_var = key_array[i];
        if (key_var.get_type() != Variant::INT) continue;

        Key key = (Key)(int64_t)key_var;
        Variant value_var = key_inputs[key_var];
        if (value_var.get_type() != Variant::VECTOR3) continue;

        Vector3 input_value = value_var;
        real_t strength = input->is_key_pressed(key) ? 1.0 : 0.0;
        current_input += input_value * strength;
    }

    // Process mouse inputs - validate types
    Array mouse_array = mouse_inputs.keys();
    for (int i = 0; i < mouse_array.size(); i++) {
        Variant mouse_var = mouse_array[i];
        if (mouse_var.get_type() != Variant::INT) continue;

        MouseButton mouse_button = (MouseButton)(int64_t)mouse_var;
        Variant value_var = mouse_inputs[mouse_var];
        if (value_var.get_type() != Variant::VECTOR3) continue;

        Vector3 input_value = value_var;
        real_t strength = input->is_mouse_button_pressed(mouse_button) ? 1.0 : 0.0;
        current_input += input_value * strength;
    }

    // Process gamepad button inputs - validate types
    Array joy_button_array = joy_button_inputs.keys();
    for (int i = 0; i < joy_button_array.size(); i++) {
        Variant joy_button_var = joy_button_array[i];
        if (joy_button_var.get_type() != Variant::INT) continue;

        JoyButton joy_button = (JoyButton)(int64_t)joy_button_var;
        Variant value_var = joy_button_inputs[joy_button_var];
        if (value_var.get_type() != Variant::VECTOR3) continue;

        Vector3 input_value = value_var;
        real_t strength = input->is_joy_button_pressed(0, joy_button) ? 1.0 : 0.0;
        current_input += input_value * strength;
    }

    // Process gamepad axis inputs - validate types
    Array joy_axis_array = joy_axis_inputs.keys();
    for (int i = 0; i < joy_axis_array.size(); i++) {
        Variant joy_axis_var = joy_axis_array[i];
        if (joy_axis_var.get_type() != Variant::INT) continue;

        JoyAxis joy_axis = (JoyAxis)(int64_t)joy_axis_var;
        Variant value_var = joy_axis_inputs[joy_axis_var];
        if (value_var.get_type() != Variant::VECTOR3) continue;

        Vector3 input_value = value_var;
        real_t strength = input->get_joy_axis(0, joy_axis);
        current_input += input_value * strength;
    }

    // Clamp to MaxValue
    current_input.x = CLAMP(current_input.x, -max_value.x, max_value.x);
    current_input.y = CLAMP(current_input.y, -max_value.y, max_value.y);
    current_input.z = CLAMP(current_input.z, -max_value.z, max_value.z);

    // Check for input begin/end
    bool was_active = is_active;
    is_active = current_input.length() > 0.01;

    if (is_active && !was_active) {
        emit_signal(SNAME("OnInputBegin"));
    } else if (!is_active && was_active) {
        emit_signal(SNAME("OnInputEnd"));
    }

    emit_signal(SNAME("OnInputUpdate"), current_input);
    previous_input = current_input;
}

// Getters and Setters
void N1D_Input::set_key_inputs(const Dictionary &p_key_inputs) {
    key_inputs = p_key_inputs;
}

Dictionary N1D_Input::get_key_inputs() const {
    return key_inputs;
}

void N1D_Input::set_mouse_inputs(const Dictionary &p_mouse_inputs) {
    mouse_inputs = p_mouse_inputs;
}

Dictionary N1D_Input::get_mouse_inputs() const {
    return mouse_inputs;
}

void N1D_Input::set_joy_button_inputs(const Dictionary &p_joy_button_inputs) {
    joy_button_inputs = p_joy_button_inputs;
}

Dictionary N1D_Input::get_joy_button_inputs() const {
    return joy_button_inputs;
}

void N1D_Input::set_joy_axis_inputs(const Dictionary &p_joy_axis_inputs) {
    joy_axis_inputs = p_joy_axis_inputs;
}

Dictionary N1D_Input::get_joy_axis_inputs() const {
    return joy_axis_inputs;
}

void N1D_Input::set_max_value(const Vector3 &p_max_value) {
    max_value = p_max_value;
}

Vector3 N1D_Input::get_max_value() const {
    return max_value;
}

// Public methods
bool N1D_Input::is_input_down() const {
    return is_active;
}

Vector3 N1D_Input::get_input_axis() const {
    return current_input;
}
