#pragma once

#include "scene/main/node.h"
#include "scene/3d/physics/character_body_3d.h"
#include "scene/2d/physics/character_body_2d.h"
#include "scene/3d/node_3d.h"
#include "scene/2d/node_2d.h"

// ---------------------------------------------------------------------------
// Z1D_CharacterMovement
// ---------------------------------------------------------------------------
// A UE-inspired character movement component for Godot 4.
// Attach as a child of CharacterBody2D or CharacterBody3D.
// Call AddMovementInput() and AddControlInput() from your input handling code.
// ---------------------------------------------------------------------------

class Z1D_CharacterMovement : public Node {
    GDCLASS(Z1D_CharacterMovement, Node);

public:
    enum MovementMode {
        MOVEMENT_MODE_WALKING,
        MOVEMENT_MODE_FALLING,
        MOVEMENT_MODE_FLYING,
        MOVEMENT_MODE_CUSTOM,
    };

private:
    // --- Linked Bodies (one will be valid, the other null) ---
    CharacterBody3D *body_3d = nullptr;
    CharacterBody2D *body_2d = nullptr;
    bool is_3d = false;

    // --- Optional Control Rotation Node ---
    NodePath control_rotation_path;
    Node3D *control_rotation_node_3d = nullptr;
    Node2D *control_rotation_node_2d = nullptr;

    // --- Movement Settings ---
    float max_speed          = 6.0f;      // Max horizontal speed (m/s)
    float acceleration       = 20.0f;     // Ground acceleration (m/s²)
    float deceleration       = 20.0f;     // Ground deceleration (m/s²)
    float ground_friction    = 8.0f;      // Friction multiplier on ground
    float air_control        = 0.3f;      // Multiplier for acceleration while airborne (0-1)
    float air_friction       = 0.5f;      // Friction while airborne

    // --- Gravity / Falling ---
    float gravity_scale      = 1.0f;      // Multiplier for project gravity
    float terminal_velocity  = 20.0f;     // Max fall speed (m/s)
    float fall_acceleration  = 1.5f;      // Extra gravity multiplier when falling (makes falls snappier)

    // --- Rotation Settings ---
    float rotation_rate      = 360.0f;    // Degrees per second for character rotation
    bool  orient_to_movement = true;      // Auto-rotate character to face movement direction
    float control_pitch_min  = -89.0f;    // Min pitch for control rotation (degrees)
    float control_pitch_max  = 89.0f;     // Max pitch for control rotation (degrees)
    float control_sensitivity = 0.1f;     // Multiplier for control input (mouse sensitivity)

    // --- State ---
    MovementMode movement_mode = MOVEMENT_MODE_FALLING;
    Vector3 velocity_3d;
    Vector2 velocity_2d;
    bool was_on_floor = false;

    // --- Per-Frame Input Accumulation ---
    Vector3 pending_movement_input_3d;
    Vector2 pending_movement_input_2d;
    Vector3 pending_control_input_3d;  // x = yaw, y = pitch, z = roll (typically)
    Vector2 pending_control_input_2d;  // x = rotation delta

    // --- Pending Launch ---
    bool    has_pending_launch = false;
    Vector3 pending_launch_velocity_3d;
    Vector2 pending_launch_velocity_2d;
    bool    launch_override_h = false;
    bool    launch_override_v = false;

protected:
    static void _bind_methods();
    void _notification(int p_what);

    // --- Internal Processing ---
    void _resolve_body();
    void _resolve_control_rotation_node();
    void _physics_process_3d(float delta);
    void _physics_process_2d(float delta);
    void _apply_control_rotation_3d(float delta);
    void _apply_control_rotation_2d(float delta);
    void _orient_to_movement_3d(float delta);
    void _orient_to_movement_2d(float delta);
    float _get_project_gravity() const;

public:
    Z1D_CharacterMovement();

    // --- Input Methods (call these from your input handling) ---
    void add_movement_input_3d(const Vector3 &p_world_direction, float p_scale = 1.0f, bool p_control_relative = false);
    void add_movement_input_2d(const Vector2 &p_world_direction, float p_scale = 1.0f);
    void add_control_input_3d(const Vector3 &p_delta);  // x=yaw, y=pitch, z=roll
    void add_control_input_2d(float p_delta);           // rotation delta

    // Convenience: auto-detects 2D/3D
    void add_movement_input(const Variant &p_direction, float p_scale = 1.0f, bool p_control_relative = false);
    void add_control_input(const Variant &p_delta);

    // --- Launch (impulse) ---
    void launch_3d(const Vector3 &p_velocity, bool p_override_horizontal = false, bool p_override_vertical = true);
    void launch_2d(const Vector2 &p_velocity, bool p_override_horizontal = false, bool p_override_vertical = true);
    void launch(const Variant &p_velocity, bool p_override_horizontal = false, bool p_override_vertical = true);

    // --- Velocity Access ---
    Vector3 get_velocity_3d() const;
    Vector2 get_velocity_2d() const;
    Variant get_velocity() const;
    void set_velocity_3d(const Vector3 &p_velocity);
    void set_velocity_2d(const Vector2 &p_velocity);
    void set_velocity(const Variant &p_velocity);

    // --- State Queries ---
    bool is_on_floor() const;
    bool is_falling() const;
    bool is_moving() const;
    bool is_3d_mode() const { return is_3d; }
    MovementMode get_movement_mode() const { return movement_mode; }
    void set_movement_mode(MovementMode p_mode) { movement_mode = p_mode; }

    // --- Getters / Setters ---
    void set_max_speed(float p_speed)             { max_speed = p_speed; }
    float get_max_speed() const                   { return max_speed; }

    void set_acceleration(float p_accel)          { acceleration = p_accel; }
    float get_acceleration() const                { return acceleration; }

    void set_deceleration(float p_decel)          { deceleration = p_decel; }
    float get_deceleration() const                { return deceleration; }

    void set_ground_friction(float p_friction)    { ground_friction = p_friction; }
    float get_ground_friction() const             { return ground_friction; }

    void set_air_control(float p_control)         { air_control = CLAMP(p_control, 0.0f, 1.0f); }
    float get_air_control() const                 { return air_control; }

    void set_air_friction(float p_friction)       { air_friction = p_friction; }
    float get_air_friction() const                { return air_friction; }

    void set_gravity_scale(float p_scale)         { gravity_scale = p_scale; }
    float get_gravity_scale() const               { return gravity_scale; }

    void set_terminal_velocity(float p_velocity)  { terminal_velocity = p_velocity; }
    float get_terminal_velocity() const           { return terminal_velocity; }

    void set_fall_acceleration(float p_accel)     { fall_acceleration = p_accel; }
    float get_fall_acceleration() const           { return fall_acceleration; }

    void set_rotation_rate(float p_rate)          { rotation_rate = p_rate; }
    float get_rotation_rate() const               { return rotation_rate; }

    void set_orient_to_movement(bool p_orient)    { orient_to_movement = p_orient; }
    bool get_orient_to_movement() const           { return orient_to_movement; }

    void set_control_rotation_path(const NodePath &p_path);
    NodePath get_control_rotation_path() const    { return control_rotation_path; }

    void set_control_pitch_min(float p_min)       { control_pitch_min = p_min; }
    float get_control_pitch_min() const           { return control_pitch_min; }

    void set_control_pitch_max(float p_max)       { control_pitch_max = p_max; }
    float get_control_pitch_max() const           { return control_pitch_max; }

    void set_control_sensitivity(float p_sens)    { control_sensitivity = p_sens; }
    float get_control_sensitivity() const         { return control_sensitivity; }
};

VARIANT_ENUM_CAST(Z1D_CharacterMovement::MovementMode);
