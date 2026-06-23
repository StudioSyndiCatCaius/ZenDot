#include "Z1D_CharacterMovement.h"
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/math/math_funcs.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

Z1D_CharacterMovement::Z1D_CharacterMovement() {
    // Physics processing enabled in NOTIFICATION_READY after editor check
}

// ---------------------------------------------------------------------------
// Binding
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_bind_methods() {
    // --- Input Methods ---
    ClassDB::bind_method(D_METHOD("add_movement_input", "direction", "scale", "control_relative"), &Z1D_CharacterMovement::add_movement_input, DEFVAL(1.0f), DEFVAL(false));
    ClassDB::bind_method(D_METHOD("add_movement_input_3d", "world_direction", "scale", "control_relative"), &Z1D_CharacterMovement::add_movement_input_3d, DEFVAL(1.0f), DEFVAL(false));
    ClassDB::bind_method(D_METHOD("add_movement_input_2d", "world_direction", "scale"), &Z1D_CharacterMovement::add_movement_input_2d, DEFVAL(1.0f));
    ClassDB::bind_method(D_METHOD("add_control_input", "delta"), &Z1D_CharacterMovement::add_control_input);
    ClassDB::bind_method(D_METHOD("add_control_input_3d", "delta"), &Z1D_CharacterMovement::add_control_input_3d);
    ClassDB::bind_method(D_METHOD("add_control_input_2d", "delta"), &Z1D_CharacterMovement::add_control_input_2d);

    // --- Launch ---
    ClassDB::bind_method(D_METHOD("launch", "velocity", "override_horizontal", "override_vertical"), &Z1D_CharacterMovement::launch, DEFVAL(false), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("launch_3d", "velocity", "override_horizontal", "override_vertical"), &Z1D_CharacterMovement::launch_3d, DEFVAL(false), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("launch_2d", "velocity", "override_horizontal", "override_vertical"), &Z1D_CharacterMovement::launch_2d, DEFVAL(false), DEFVAL(true));

    // --- Velocity ---
    ClassDB::bind_method(D_METHOD("get_velocity"), &Z1D_CharacterMovement::get_velocity);
    ClassDB::bind_method(D_METHOD("set_velocity", "velocity"), &Z1D_CharacterMovement::set_velocity);
    ClassDB::bind_method(D_METHOD("get_velocity_3d"), &Z1D_CharacterMovement::get_velocity_3d);
    ClassDB::bind_method(D_METHOD("set_velocity_3d", "velocity"), &Z1D_CharacterMovement::set_velocity_3d);
    ClassDB::bind_method(D_METHOD("get_velocity_2d"), &Z1D_CharacterMovement::get_velocity_2d);
    ClassDB::bind_method(D_METHOD("set_velocity_2d", "velocity"), &Z1D_CharacterMovement::set_velocity_2d);

    // --- State Queries ---
    ClassDB::bind_method(D_METHOD("is_on_floor"), &Z1D_CharacterMovement::is_on_floor);
    ClassDB::bind_method(D_METHOD("is_falling"), &Z1D_CharacterMovement::is_falling);
    ClassDB::bind_method(D_METHOD("is_moving"), &Z1D_CharacterMovement::is_moving);
    ClassDB::bind_method(D_METHOD("is_3d_mode"), &Z1D_CharacterMovement::is_3d_mode);
    ClassDB::bind_method(D_METHOD("get_movement_mode"), &Z1D_CharacterMovement::get_movement_mode);
    ClassDB::bind_method(D_METHOD("set_movement_mode", "mode"), &Z1D_CharacterMovement::set_movement_mode);

    // --- Properties ---
    ClassDB::bind_method(D_METHOD("set_max_speed", "speed"), &Z1D_CharacterMovement::set_max_speed);
    ClassDB::bind_method(D_METHOD("get_max_speed"), &Z1D_CharacterMovement::get_max_speed);
    ClassDB::bind_method(D_METHOD("set_acceleration", "accel"), &Z1D_CharacterMovement::set_acceleration);
    ClassDB::bind_method(D_METHOD("get_acceleration"), &Z1D_CharacterMovement::get_acceleration);
    ClassDB::bind_method(D_METHOD("set_deceleration", "decel"), &Z1D_CharacterMovement::set_deceleration);
    ClassDB::bind_method(D_METHOD("get_deceleration"), &Z1D_CharacterMovement::get_deceleration);
    ClassDB::bind_method(D_METHOD("set_ground_friction", "friction"), &Z1D_CharacterMovement::set_ground_friction);
    ClassDB::bind_method(D_METHOD("get_ground_friction"), &Z1D_CharacterMovement::get_ground_friction);
    ClassDB::bind_method(D_METHOD("set_air_control", "control"), &Z1D_CharacterMovement::set_air_control);
    ClassDB::bind_method(D_METHOD("get_air_control"), &Z1D_CharacterMovement::get_air_control);
    ClassDB::bind_method(D_METHOD("set_air_friction", "friction"), &Z1D_CharacterMovement::set_air_friction);
    ClassDB::bind_method(D_METHOD("get_air_friction"), &Z1D_CharacterMovement::get_air_friction);
    ClassDB::bind_method(D_METHOD("set_gravity_scale", "scale"), &Z1D_CharacterMovement::set_gravity_scale);
    ClassDB::bind_method(D_METHOD("get_gravity_scale"), &Z1D_CharacterMovement::get_gravity_scale);
    ClassDB::bind_method(D_METHOD("set_terminal_velocity", "velocity"), &Z1D_CharacterMovement::set_terminal_velocity);
    ClassDB::bind_method(D_METHOD("get_terminal_velocity"), &Z1D_CharacterMovement::get_terminal_velocity);
    ClassDB::bind_method(D_METHOD("set_fall_acceleration", "accel"), &Z1D_CharacterMovement::set_fall_acceleration);
    ClassDB::bind_method(D_METHOD("get_fall_acceleration"), &Z1D_CharacterMovement::get_fall_acceleration);
    ClassDB::bind_method(D_METHOD("set_rotation_rate", "rate"), &Z1D_CharacterMovement::set_rotation_rate);
    ClassDB::bind_method(D_METHOD("get_rotation_rate"), &Z1D_CharacterMovement::get_rotation_rate);
    ClassDB::bind_method(D_METHOD("set_orient_to_movement", "orient"), &Z1D_CharacterMovement::set_orient_to_movement);
    ClassDB::bind_method(D_METHOD("get_orient_to_movement"), &Z1D_CharacterMovement::get_orient_to_movement);
    ClassDB::bind_method(D_METHOD("set_control_rotation_path", "path"), &Z1D_CharacterMovement::set_control_rotation_path);
    ClassDB::bind_method(D_METHOD("get_control_rotation_path"), &Z1D_CharacterMovement::get_control_rotation_path);
    ClassDB::bind_method(D_METHOD("set_control_pitch_min", "min"), &Z1D_CharacterMovement::set_control_pitch_min);
    ClassDB::bind_method(D_METHOD("get_control_pitch_min"), &Z1D_CharacterMovement::get_control_pitch_min);
    ClassDB::bind_method(D_METHOD("set_control_pitch_max", "max"), &Z1D_CharacterMovement::set_control_pitch_max);
    ClassDB::bind_method(D_METHOD("get_control_pitch_max"), &Z1D_CharacterMovement::get_control_pitch_max);
    ClassDB::bind_method(D_METHOD("set_control_sensitivity", "sens"), &Z1D_CharacterMovement::set_control_sensitivity);
    ClassDB::bind_method(D_METHOD("get_control_sensitivity"), &Z1D_CharacterMovement::get_control_sensitivity);

    // --- Property Registration ---
    ADD_GROUP("Movement", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_speed",       PROPERTY_HINT_RANGE, "0,50,0.1,or_greater"),    "set_max_speed",       "get_max_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "acceleration",    PROPERTY_HINT_RANGE, "0,100,0.1,or_greater"),   "set_acceleration",    "get_acceleration");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "deceleration",    PROPERTY_HINT_RANGE, "0,100,0.1,or_greater"),   "set_deceleration",    "get_deceleration");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ground_friction", PROPERTY_HINT_RANGE, "0,20,0.1,or_greater"),    "set_ground_friction", "get_ground_friction");

    ADD_GROUP("Air Control", "air_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "air_control",     PROPERTY_HINT_RANGE, "0,1,0.01"),               "set_air_control",     "get_air_control");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "air_friction",    PROPERTY_HINT_RANGE, "0,20,0.1,or_greater"),    "set_air_friction",    "get_air_friction");

    ADD_GROUP("Gravity", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity_scale",     PROPERTY_HINT_RANGE, "-10,10,0.01"),          "set_gravity_scale",     "get_gravity_scale");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "terminal_velocity", PROPERTY_HINT_RANGE, "0,100,0.1,or_greater"), "set_terminal_velocity", "get_terminal_velocity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fall_acceleration", PROPERTY_HINT_RANGE, "1,5,0.01"),             "set_fall_acceleration", "get_fall_acceleration");

    ADD_GROUP("Rotation", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation_rate",      PROPERTY_HINT_RANGE, "0,720,1,or_greater"), "set_rotation_rate",      "get_rotation_rate");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL,  "orient_to_movement"),                                             "set_orient_to_movement", "get_orient_to_movement");

    ADD_GROUP("Control Rotation", "control_");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "control_rotation_path"),                                          "set_control_rotation_path", "get_control_rotation_path");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "control_pitch_min",   PROPERTY_HINT_RANGE, "-90,0,0.1"),              "set_control_pitch_min",     "get_control_pitch_min");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "control_pitch_max",   PROPERTY_HINT_RANGE, "0,90,0.1"),               "set_control_pitch_max",     "get_control_pitch_max");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "control_sensitivity", PROPERTY_HINT_RANGE, "0.01,1.0,0.01,or_greater"),"set_control_sensitivity",   "get_control_sensitivity");

    // --- Enums ---
    BIND_ENUM_CONSTANT(MOVEMENT_MODE_WALKING);
    BIND_ENUM_CONSTANT(MOVEMENT_MODE_FALLING);
    BIND_ENUM_CONSTANT(MOVEMENT_MODE_FLYING);
    BIND_ENUM_CONSTANT(MOVEMENT_MODE_CUSTOM);

    // --- Signals ---
    ADD_SIGNAL(MethodInfo("landed"));
    ADD_SIGNAL(MethodInfo("started_falling"));
    ADD_SIGNAL(MethodInfo("launched", PropertyInfo(Variant::VECTOR3, "velocity")));
    ADD_SIGNAL(MethodInfo("movement_mode_changed", PropertyInfo(Variant::INT, "new_mode"), PropertyInfo(Variant::INT, "old_mode")));
}

// ---------------------------------------------------------------------------
// Notification
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY:
            _resolve_body();
            _resolve_control_rotation_node();
            set_physics_process(!Engine::get_singleton()->is_editor_hint());
            break;

        case NOTIFICATION_PHYSICS_PROCESS: {
            float delta = get_physics_process_delta_time();
            if (is_3d && body_3d) {
                _physics_process_3d(delta);
            } else if (!is_3d && body_2d) {
                _physics_process_2d(delta);
            }
        } break;
    }
}

// ---------------------------------------------------------------------------
// Body Resolution
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_resolve_body() {
    Node *parent = get_parent();
    body_3d = Object::cast_to<CharacterBody3D>(parent);
    body_2d = Object::cast_to<CharacterBody2D>(parent);

    if (body_3d) {
        is_3d = true;
    } else if (body_2d) {
        is_3d = false;
    } else {
        WARN_PRINT("Z1D_CharacterMovement: Parent is not a CharacterBody2D or CharacterBody3D!");
    }
}

void Z1D_CharacterMovement::_resolve_control_rotation_node() {
    control_rotation_node_3d = nullptr;
    control_rotation_node_2d = nullptr;

    if (control_rotation_path.is_empty()) {
        return;
    }

    Node *node = get_node_or_null(control_rotation_path);
    if (!node) {
        return;
    }

    control_rotation_node_3d = Object::cast_to<Node3D>(node);
    control_rotation_node_2d = Object::cast_to<Node2D>(node);
}

void Z1D_CharacterMovement::set_control_rotation_path(const NodePath &p_path) {
    control_rotation_path = p_path;
    if (is_inside_tree()) {
        _resolve_control_rotation_node();
    }
}

// ---------------------------------------------------------------------------
// Project Gravity
// ---------------------------------------------------------------------------

float Z1D_CharacterMovement::_get_project_gravity() const {
    // Godot 4 stores gravity in physics/3d/default_gravity or physics/2d/default_gravity
    if (is_3d) {
        return ProjectSettings::get_singleton()->get_setting("physics/3d/default_gravity", 9.8f);
    } else {
        return ProjectSettings::get_singleton()->get_setting("physics/2d/default_gravity", 980.0f);
    }
}

// ---------------------------------------------------------------------------
// Input Accumulation
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::add_movement_input_3d(const Vector3 &p_world_direction, float p_scale, bool p_control_relative) {
    if (p_world_direction.length_squared() < 0.0001f) {
        return;
    }

    Vector3 direction = p_world_direction.normalized();

    // Transform to control rotation space (yaw only)
    if (p_control_relative && control_rotation_node_3d) {
        float yaw = control_rotation_node_3d->get_rotation().y;
        Basis rotation_basis = Basis(Vector3(0, 1, 0), yaw);
        direction = rotation_basis.xform(direction);
    }

    pending_movement_input_3d += direction * p_scale;
}

void Z1D_CharacterMovement::add_movement_input_2d(const Vector2 &p_world_direction, float p_scale) {
    if (p_world_direction.length_squared() > 0.0001f) {
        pending_movement_input_2d += p_world_direction.normalized() * p_scale;
    }
}

void Z1D_CharacterMovement::add_control_input_3d(const Vector3 &p_delta) {
    pending_control_input_3d += p_delta * control_sensitivity;
}

void Z1D_CharacterMovement::add_control_input_2d(float p_delta) {
    pending_control_input_2d.x += p_delta * control_sensitivity;
}

void Z1D_CharacterMovement::add_movement_input(const Variant &p_direction, float p_scale, bool p_control_relative) {
    if (is_3d) {
        add_movement_input_3d(p_direction, p_scale, p_control_relative);
    } else {
        add_movement_input_2d(p_direction, p_scale);
    }
}

void Z1D_CharacterMovement::add_control_input(const Variant &p_delta) {
    if (is_3d) {
        add_control_input_3d(p_delta);
    } else {
        add_control_input_2d(float(p_delta));
    }
}

// ---------------------------------------------------------------------------
// Launch
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::launch_3d(const Vector3 &p_velocity, bool p_override_horizontal, bool p_override_vertical) {
    has_pending_launch = true;
    pending_launch_velocity_3d = p_velocity;
    launch_override_h = p_override_horizontal;
    launch_override_v = p_override_vertical;
}

void Z1D_CharacterMovement::launch_2d(const Vector2 &p_velocity, bool p_override_horizontal, bool p_override_vertical) {
    has_pending_launch = true;
    pending_launch_velocity_2d = p_velocity;
    launch_override_h = p_override_horizontal;
    launch_override_v = p_override_vertical;
}

void Z1D_CharacterMovement::launch(const Variant &p_velocity, bool p_override_horizontal, bool p_override_vertical) {
    if (is_3d) {
        launch_3d(p_velocity, p_override_horizontal, p_override_vertical);
    } else {
        launch_2d(p_velocity, p_override_horizontal, p_override_vertical);
    }
}

// ---------------------------------------------------------------------------
// Velocity Access
// ---------------------------------------------------------------------------

Vector3 Z1D_CharacterMovement::get_velocity_3d() const { return velocity_3d; }
Vector2 Z1D_CharacterMovement::get_velocity_2d() const { return velocity_2d; }

Variant Z1D_CharacterMovement::get_velocity() const {
    return is_3d ? Variant(velocity_3d) : Variant(velocity_2d);
}

void Z1D_CharacterMovement::set_velocity_3d(const Vector3 &p_velocity) { velocity_3d = p_velocity; }
void Z1D_CharacterMovement::set_velocity_2d(const Vector2 &p_velocity) { velocity_2d = p_velocity; }

void Z1D_CharacterMovement::set_velocity(const Variant &p_velocity) {
    if (is_3d) {
        velocity_3d = p_velocity;
    } else {
        velocity_2d = p_velocity;
    }
}

// ---------------------------------------------------------------------------
// State Queries
// ---------------------------------------------------------------------------

bool Z1D_CharacterMovement::is_on_floor() const {
    if (is_3d && body_3d) {
        return body_3d->is_on_floor();
    } else if (!is_3d && body_2d) {
        return body_2d->is_on_floor();
    }
    return false;
}

bool Z1D_CharacterMovement::is_falling() const {
    return movement_mode == MOVEMENT_MODE_FALLING;
}

bool Z1D_CharacterMovement::is_moving() const {
    if (is_3d) {
        return velocity_3d.length_squared() > 0.01f;
    } else {
        return velocity_2d.length_squared() > 0.01f;
    }
}

// ---------------------------------------------------------------------------
// 3D Physics Processing
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_physics_process_3d(float delta) {
    // --- Apply Control Rotation ---
    _apply_control_rotation_3d(delta);

    // --- Handle Launch ---
    if (has_pending_launch) {
        has_pending_launch = false;

        if (launch_override_h) {
            velocity_3d.x = pending_launch_velocity_3d.x;
            velocity_3d.z = pending_launch_velocity_3d.z;
        } else {
            velocity_3d.x += pending_launch_velocity_3d.x;
            velocity_3d.z += pending_launch_velocity_3d.z;
        }

        if (launch_override_v) {
            velocity_3d.y = pending_launch_velocity_3d.y;
        } else {
            velocity_3d.y += pending_launch_velocity_3d.y;
        }

        // If launching upward, switch to falling mode immediately
        if (velocity_3d.y > 0) {
            movement_mode = MOVEMENT_MODE_FALLING;
        }

        emit_signal("launched", pending_launch_velocity_3d);
        pending_launch_velocity_3d = Vector3();
    }

    // --- Determine Grounded State ---
    bool on_floor = body_3d->is_on_floor();
    MovementMode old_mode = movement_mode;

    if (movement_mode != MOVEMENT_MODE_FLYING && movement_mode != MOVEMENT_MODE_CUSTOM) {
        // Don't consider "landed" if we're moving upward (just launched)
        if (on_floor && velocity_3d.y <= 0) {
            if (movement_mode == MOVEMENT_MODE_FALLING) {
                movement_mode = MOVEMENT_MODE_WALKING;
                emit_signal("landed");
            }
        } else {
            if (movement_mode == MOVEMENT_MODE_WALKING) {
                movement_mode = MOVEMENT_MODE_FALLING;
                emit_signal("started_falling");
            }
        }
    }

    if (old_mode != movement_mode) {
        emit_signal("movement_mode_changed", (int)movement_mode, (int)old_mode);
    }

    // --- Calculate Movement ---
    Vector3 input = pending_movement_input_3d;
    pending_movement_input_3d = Vector3(); // Clear for next frame

    // Flatten input to horizontal plane
    input.y = 0;
    float input_magnitude = MIN(input.length(), 1.0f);
    Vector3 input_dir = input_magnitude > 0.0001f ? input.normalized() : Vector3();

    // Current horizontal velocity
    Vector3 horizontal_vel(velocity_3d.x, 0, velocity_3d.z);
    float current_speed = horizontal_vel.length();

    // Determine accel/friction based on state
    float effective_accel = acceleration;
    float effective_friction = ground_friction;

    if (!on_floor) {
        effective_accel *= air_control;
        effective_friction = air_friction;
    }

    // Apply acceleration or deceleration
    if (input_magnitude > 0.0001f) {
        // Accelerate towards desired direction
        Vector3 desired_vel = input_dir * max_speed * input_magnitude;
        horizontal_vel = horizontal_vel.move_toward(desired_vel, effective_accel * delta);
    } else {
        // Decelerate to stop
        horizontal_vel = horizontal_vel.move_toward(Vector3(), deceleration * delta);
    }

    // Apply friction
    if (on_floor && input_magnitude < 0.0001f) {
        horizontal_vel *= MAX(0.0f, 1.0f - effective_friction * delta);
    }

    // Clamp to max speed
    if (horizontal_vel.length() > max_speed) {
        horizontal_vel = horizontal_vel.normalized() * max_speed;
    }

    velocity_3d.x = horizontal_vel.x;
    velocity_3d.z = horizontal_vel.z;

    // --- Apply Gravity ---
    if (movement_mode == MOVEMENT_MODE_FALLING || (movement_mode == MOVEMENT_MODE_WALKING && !on_floor)) {
        float gravity = _get_project_gravity() * gravity_scale;

        // Apply extra fall acceleration when moving downward
        if (velocity_3d.y < 0) {
            gravity *= fall_acceleration;
        }

        velocity_3d.y -= gravity * delta;

        // Clamp to terminal velocity
        if (velocity_3d.y < -terminal_velocity) {
            velocity_3d.y = -terminal_velocity;
        }
    } else if (movement_mode == MOVEMENT_MODE_WALKING && on_floor) {
        // Small downward force to keep grounded
        velocity_3d.y = -1.0f;
    }

    // --- Orient to Movement ---
    if (orient_to_movement && input_magnitude > 0.0001f) {
        _orient_to_movement_3d(delta);
    }

    // --- Apply Movement ---
    body_3d->set_velocity(velocity_3d);
    body_3d->move_and_slide();
    velocity_3d = body_3d->get_velocity();

    was_on_floor = on_floor;
}

// ---------------------------------------------------------------------------
// 2D Physics Processing
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_physics_process_2d(float delta) {
    // --- Apply Control Rotation ---
    _apply_control_rotation_2d(delta);

    // --- Handle Launch ---
    if (has_pending_launch) {
        has_pending_launch = false;

        if (launch_override_h) {
            velocity_2d.x = pending_launch_velocity_2d.x;
        } else {
            velocity_2d.x += pending_launch_velocity_2d.x;
        }

        if (launch_override_v) {
            velocity_2d.y = pending_launch_velocity_2d.y;
        } else {
            velocity_2d.y += pending_launch_velocity_2d.y;
        }

        // If launching upward (negative Y in 2D), switch to falling mode immediately
        if (velocity_2d.y < 0) {
            movement_mode = MOVEMENT_MODE_FALLING;
        }

        emit_signal("launched", Vector3(pending_launch_velocity_2d.x, pending_launch_velocity_2d.y, 0));
        pending_launch_velocity_2d = Vector2();
    }

    // --- Determine Grounded State ---
    bool on_floor = body_2d->is_on_floor();
    MovementMode old_mode = movement_mode;

    if (movement_mode != MOVEMENT_MODE_FLYING && movement_mode != MOVEMENT_MODE_CUSTOM) {
        // Don't consider "landed" if we're moving upward (just launched) - in 2D, up is negative Y
        if (on_floor && velocity_2d.y >= 0) {
            if (movement_mode == MOVEMENT_MODE_FALLING) {
                movement_mode = MOVEMENT_MODE_WALKING;
                emit_signal("landed");
            }
        } else {
            if (movement_mode == MOVEMENT_MODE_WALKING) {
                movement_mode = MOVEMENT_MODE_FALLING;
                emit_signal("started_falling");
            }
        }
    }

    if (old_mode != movement_mode) {
        emit_signal("movement_mode_changed", (int)movement_mode, (int)old_mode);
    }

    // --- Calculate Horizontal Movement ---
    Vector2 input = pending_movement_input_2d;
    pending_movement_input_2d = Vector2(); // Clear for next frame

    float input_x = CLAMP(input.x, -1.0f, 1.0f);
    float current_speed_x = velocity_2d.x;

    float effective_accel = acceleration;
    float effective_friction = ground_friction;

    if (!on_floor) {
        effective_accel *= air_control;
        effective_friction = air_friction;
    }

    if (Math::abs(input_x) > 0.0001f) {
        // Accelerate
        float desired_speed = input_x * max_speed;
        velocity_2d.x = Math::move_toward(velocity_2d.x, desired_speed, effective_accel * delta);
    } else {
        // Decelerate
        velocity_2d.x = Math::move_toward(velocity_2d.x, 0.0f, deceleration * delta);
    }

    // Apply friction
    if (on_floor && Math::abs(input_x) < 0.0001f) {
        velocity_2d.x *= MAX(0.0f, 1.0f - effective_friction * delta);
    }

    // Clamp to max speed
    velocity_2d.x = CLAMP(velocity_2d.x, -max_speed, max_speed);

    // --- Apply Gravity (Y is down in 2D) ---
    if (movement_mode == MOVEMENT_MODE_FALLING || (movement_mode == MOVEMENT_MODE_WALKING && !on_floor)) {
        float gravity = _get_project_gravity() * gravity_scale;

        // Extra fall acceleration when moving downward
        if (velocity_2d.y > 0) {
            gravity *= fall_acceleration;
        }

        velocity_2d.y += gravity * delta;

        // Clamp to terminal velocity
        if (velocity_2d.y > terminal_velocity) {
            velocity_2d.y = terminal_velocity;
        }
    } else if (movement_mode == MOVEMENT_MODE_WALKING && on_floor) {
        velocity_2d.y = 1.0f;
    }

    // --- Orient to Movement (2D: flip sprite direction) ---
    if (orient_to_movement && Math::abs(input_x) > 0.0001f) {
        _orient_to_movement_2d(delta);
    }

    // --- Apply Movement ---
    body_2d->set_velocity(velocity_2d);
    body_2d->move_and_slide();
    velocity_2d = body_2d->get_velocity();

    was_on_floor = on_floor;
}

// ---------------------------------------------------------------------------
// Control Rotation Application
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_apply_control_rotation_3d(float delta) {
    if (!control_rotation_node_3d || pending_control_input_3d.is_zero_approx()) {
        pending_control_input_3d = Vector3();
        return;
    }

    // Get current rotation
    Vector3 rotation = control_rotation_node_3d->get_rotation_degrees();

    // Apply yaw (x input = yaw)
    rotation.y -= pending_control_input_3d.x;

    // Apply pitch (y input = pitch)
    rotation.x -= pending_control_input_3d.y;
    rotation.x = CLAMP(rotation.x, control_pitch_min, control_pitch_max);

    // Apply roll if needed (z input = roll) - typically not used for cameras
    rotation.z += pending_control_input_3d.z;

    control_rotation_node_3d->set_rotation_degrees(rotation);
    pending_control_input_3d = Vector3();
}

void Z1D_CharacterMovement::_apply_control_rotation_2d(float delta) {
    if (!control_rotation_node_2d || Math::abs(pending_control_input_2d.x) < 0.0001f) {
        pending_control_input_2d = Vector2();
        return;
    }

    float rotation = control_rotation_node_2d->get_rotation_degrees();
    rotation += pending_control_input_2d.x;
    control_rotation_node_2d->set_rotation_degrees(rotation);
    pending_control_input_2d = Vector2();
}

// ---------------------------------------------------------------------------
// Orient to Movement
// ---------------------------------------------------------------------------

void Z1D_CharacterMovement::_orient_to_movement_3d(float delta) {
    Vector3 horizontal_vel(velocity_3d.x, 0, velocity_3d.z);
    if (horizontal_vel.length_squared() < 0.01f) {
        return;
    }

    // Calculate target yaw from velocity direction
    float target_yaw = Math::rad_to_deg(Math::atan2(-horizontal_vel.x, -horizontal_vel.z));
    Vector3 current_rot = body_3d->get_rotation_degrees();

    // Smoothly interpolate towards target
    float yaw_diff = Math::angle_difference(Math::deg_to_rad(current_rot.y), Math::deg_to_rad(target_yaw));
    float max_rotation = rotation_rate * delta;

    if (Math::abs(yaw_diff) > Math::deg_to_rad(max_rotation)) {
        yaw_diff = SIGN(yaw_diff) * Math::deg_to_rad(max_rotation);
    }

    current_rot.y += Math::rad_to_deg(yaw_diff);
    body_3d->set_rotation_degrees(current_rot);
}

void Z1D_CharacterMovement::_orient_to_movement_2d(float delta) {
    // For 2D, "orient to movement" typically means flipping the sprite
    // We'll rotate the body to face movement direction
    // You might want to handle this differently (e.g., flip Node2D.scale.x)

    if (Math::abs(velocity_2d.x) > 0.01f) {
        float target_angle = velocity_2d.x > 0 ? 0.0f : 180.0f;
        float current_angle = body_2d->get_rotation_degrees();

        float angle_diff = Math::angle_difference(Math::deg_to_rad(current_angle), Math::deg_to_rad(target_angle));
        float max_rotation = rotation_rate * delta;

        if (Math::abs(angle_diff) > Math::deg_to_rad(max_rotation)) {
            angle_diff = SIGN(angle_diff) * Math::deg_to_rad(max_rotation);
        }

        current_angle += Math::rad_to_deg(angle_diff);
        body_2d->set_rotation_degrees(current_angle);
    }
}
