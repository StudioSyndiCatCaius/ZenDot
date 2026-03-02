#ifndef N1D_INPUT_H
#define N1D_INPUT_H

#include "scene/main/node.h"
#include "core/input/input.h"
#include "core/variant/dictionary.h"
#include "core/math/vector3.h"

class N1D_Input : public Node {
	GDCLASS(N1D_Input, Node);

private:
	Dictionary key_inputs;
	Dictionary mouse_inputs;
	Dictionary joy_button_inputs;
	Dictionary joy_axis_inputs;
	Vector3 max_value;
	Vector3 previous_input;
	Vector3 current_input;
	bool is_active;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	N1D_Input();
	~N1D_Input();

	void _ready();
	void _process(double delta);

	// Getters and Setters for exported properties
	void set_key_inputs(const Dictionary &p_key_inputs);
	Dictionary get_key_inputs() const;

	void set_mouse_inputs(const Dictionary &p_mouse_inputs);
	Dictionary get_mouse_inputs() const;

	void set_joy_button_inputs(const Dictionary &p_joy_button_inputs);
	Dictionary get_joy_button_inputs() const;

	void set_joy_axis_inputs(const Dictionary &p_joy_axis_inputs);
	Dictionary get_joy_axis_inputs() const;

	void set_max_value(const Vector3 &p_max_value);
	Vector3 get_max_value() const;

	// Public methods
	bool is_input_down() const;
	Vector3 get_input_axis() const;
};

#endif // N1D_INPUT_H
