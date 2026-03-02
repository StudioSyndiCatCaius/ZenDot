#include "N3D_ZenCharacter.h"
#include "core/object/class_db.h"


void ZenCharacter::_bind_methods() {
    // Bind methods for GDScript access
    ClassDB::bind_method(D_METHOD("get_mesh_instance"), &ZenCharacter::get_mesh_instance);
    ClassDB::bind_method(D_METHOD("get_collision_shape"), &ZenCharacter::get_collision_shape);

    ClassDB::bind_method(D_METHOD("set_capsule_height", "height"), &ZenCharacter::set_capsule_height);
    ClassDB::bind_method(D_METHOD("set_capsule_radius", "radius"), &ZenCharacter::set_capsule_radius);
    ClassDB::bind_method(D_METHOD("get_capsule_height"), &ZenCharacter::get_capsule_height);
    ClassDB::bind_method(D_METHOD("get_capsule_radius"), &ZenCharacter::get_capsule_radius);

    ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &ZenCharacter::set_mesh);
    ClassDB::bind_method(D_METHOD("get_mesh"), &ZenCharacter::get_mesh);

    // Add properties for the editor
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "capsule_height", PROPERTY_HINT_RANGE, "0.1,10.0,0.1"), "set_capsule_height", "get_capsule_height");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "capsule_radius", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_capsule_radius", "get_capsule_radius");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh", "get_mesh");
}

ZenCharacter::ZenCharacter() {
    mesh_instance = nullptr;
    collision_shape = nullptr;
}

ZenCharacter::~ZenCharacter() {
    // Cleanup is handled by Godot's memory management
}

void ZenCharacter::_enter_tree() {
    //CharacterBody3D::_enter_tree();

    // Create the capsule shape
    capsule_shape.instantiate();
    capsule_shape->set_height(2.0f);
    capsule_shape->set_radius(0.5f);

    // Create collision shape node
    collision_shape = memnew(CollisionShape3D);
    collision_shape->set_shape(capsule_shape);
    collision_shape->set_name("CollisionShape3D");
    add_child(collision_shape);

    // Create mesh instance node
    mesh_instance = memnew(MeshInstance3D);
    mesh_instance->set_name("MeshInstance3D");
    add_child(mesh_instance);
}

void ZenCharacter::_ready() {
   // CharacterBody3D::_ready();
}

MeshInstance3D* ZenCharacter::get_mesh_instance() const {
    return mesh_instance;
}

CollisionShape3D* ZenCharacter::get_collision_shape() const {
    return collision_shape;
}

void ZenCharacter::set_capsule_height(float height) {
    if (capsule_shape.is_valid()) {
        capsule_shape->set_height(height);
    }
}

void ZenCharacter::set_capsule_radius(float radius) {
    if (capsule_shape.is_valid()) {
        capsule_shape->set_radius(radius);
    }
}

float ZenCharacter::get_capsule_height() const {
    if (capsule_shape.is_valid()) {
        return capsule_shape->get_height();
    }
    return 0.0f;
}

float ZenCharacter::get_capsule_radius() const {
    if (capsule_shape.is_valid()) {
        return capsule_shape->get_radius();
    }
    return 0.0f;
}

void ZenCharacter::set_mesh(const Ref<Mesh>& p_mesh) {
    if (mesh_instance) {
        mesh_instance->set_mesh(p_mesh);
    }
}

Ref<Mesh> ZenCharacter::get_mesh() const {
    if (mesh_instance) {
        return mesh_instance->get_mesh();
    }
    return Ref<Mesh>();
}
