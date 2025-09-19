#ifndef GAMEPLAY_RESOURCE_H
#define GAMEPLAY_RESOURCE_H

#include "core/io/resource.h"

class GameplayResource : public Resource {
	GDCLASS(GameplayResource, Resource);

private:
	String player_name;
	int level;
	float health;
	int score;
	Array inventory;
	Dictionary stats;

protected:
	static void _bind_methods();

public:
	GameplayResource();
	~GameplayResource();

	// Getters
	String get_player_name() const;
	int get_level() const;
	float get_health() const;
	int get_score() const;
	Array get_inventory() const;
	Dictionary get_stats() const;

	// Setters
	void set_player_name(const String &p_name);
	void set_level(int p_level);
	void set_health(float p_health);
	void set_score(int p_score);
	void set_inventory(const Array &p_inventory);
	void set_stats(const Dictionary &p_stats);

	// Custom methods
	void add_inventory_item(const String &item);
	void remove_inventory_item(const String &item);
	void increase_score(int amount);
	void level_up();
};

#endif
