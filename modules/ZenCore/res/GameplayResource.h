#ifndef GAMEPLAY_RESOURCE_H
#define GAMEPLAY_RESOURCE_H

#include "core/io/resource.h"
#include "scene/resources/texture.h"

class GameplayResource : public Resource {
	GDCLASS(GameplayResource, Resource);

private:

protected:
	static void _bind_methods();

public:
	GameplayResource();
	~GameplayResource();

	// Getters
	String title;
	String get_title() const { return title; }
	void set_title(const String &p_title) { title=p_title; }

	String description;
	String get_description() const { return description; }
	void set_description(const String &p_description) { description=p_description; }

	Ref<Texture2D> icon;
	void set_icon(const Ref<Texture2D> &p_icon) { icon=p_icon; };
	Ref<Texture2D> get_icon() const { return icon; };
};

#endif
