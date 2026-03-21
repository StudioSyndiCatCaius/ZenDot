#ifndef ZEN_CHARACTER_H
#define ZEN_CHARACTER_H

#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/physics/character_body_3d.h"
#include "scene/3d/physics/collision_shape_3d.h"
#include "scene/resources/3d/capsule_shape_3d.h"

class ZenCharacter : public CharacterBody3D {
	GDCLASS(ZenCharacter, CharacterBody3D);

private:
	MeshInstance3D* mesh_instance;
	CollisionShape3D* collision_shape;
	Ref<CapsuleShape3D> capsule_shape;

protected:
	static void _bind_methods();

public:
	ZenCharacter();
	~ZenCharacter();

	virtual void _ready();
	virtual void _enter_tree() ;

	// Getters and Setters for the components
	MeshInstance3D* get_mesh_instance() const;
	CollisionShape3D* get_collision_shape() const;

	// Utility methods for configuring the capsule
	void set_capsule_height(float height);
	void set_capsule_radius(float radius);
	float get_capsule_height() const;
	float get_capsule_radius() const;

	// Mesh configuration
	void set_mesh(const Ref<Mesh>& p_mesh);
	Ref<Mesh> get_mesh() const;
};


#endif // ZEN_CHARACTER_H
