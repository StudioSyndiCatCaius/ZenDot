#include "GameplayResource.h"

GameplayResource::GameplayResource() {
    player_name = "";
    level = 1;
    health = 100.0f;
    score = 0;
    inventory = Array();
    stats = Dictionary();
}

GameplayResource::~GameplayResource() {
}

void GameplayResource::_bind_methods() {
    // Bind properties
    ClassDB::bind_method(D_METHOD("set_player_name", "name"), &GameplayResource::set_player_name);
    ClassDB::bind_method(D_METHOD("get_player_name"), &GameplayResource::get_player_name);

    ClassDB::bind_method(D_METHOD("set_level", "level"), &GameplayResource::set_level);
    ClassDB::bind_method(D_METHOD("get_level"), &GameplayResource::get_level);

    ClassDB::bind_method(D_METHOD("set_health", "health"), &GameplayResource::set_health);
    ClassDB::bind_method(D_METHOD("get_health"), &GameplayResource::get_health);

    ClassDB::bind_method(D_METHOD("set_score", "score"), &GameplayResource::set_score);
    ClassDB::bind_method(D_METHOD("get_score"), &GameplayResource::get_score);

    ClassDB::bind_method(D_METHOD("set_inventory", "inventory"), &GameplayResource::set_inventory);
    ClassDB::bind_method(D_METHOD("get_inventory"), &GameplayResource::get_inventory);

    ClassDB::bind_method(D_METHOD("set_stats", "stats"), &GameplayResource::set_stats);
    ClassDB::bind_method(D_METHOD("get_stats"), &GameplayResource::get_stats);

    // Bind custom methods
    ClassDB::bind_method(D_METHOD("add_inventory_item", "item"), &GameplayResource::add_inventory_item);
    ClassDB::bind_method(D_METHOD("remove_inventory_item", "item"), &GameplayResource::remove_inventory_item);
    ClassDB::bind_method(D_METHOD("increase_score", "amount"), &GameplayResource::increase_score);
    ClassDB::bind_method(D_METHOD("level_up"), &GameplayResource::level_up);

    // Add properties to the editor
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "player_name"), "set_player_name", "get_player_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "level"), "set_level", "get_level");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "health"), "set_health", "get_health");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "score"), "set_score", "get_score");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "inventory"), "set_inventory", "get_inventory");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "stats"), "set_stats", "get_stats");
}

// Getters
String GameplayResource::get_player_name() const {
    return player_name;
}

int GameplayResource::get_level() const {
    return level;
}

float GameplayResource::get_health() const {
    return health;
}

int GameplayResource::get_score() const {
    return score;
}

Array GameplayResource::get_inventory() const {
    return inventory;
}

Dictionary GameplayResource::get_stats() const {
    return stats;
}

// Setters
void GameplayResource::set_player_name(const String &p_name) {
    player_name = p_name;
    emit_changed();
}

void GameplayResource::set_level(int p_level) {
    level = p_level;
    emit_changed();
}

void GameplayResource::set_health(float p_health) {
    health = p_health;
    emit_changed();
}

void GameplayResource::set_score(int p_score) {
    score = p_score;
    emit_changed();
}

void GameplayResource::set_inventory(const Array &p_inventory) {
    inventory = p_inventory;
    emit_changed();
}

void GameplayResource::set_stats(const Dictionary &p_stats) {
    stats = p_stats;
    emit_changed();
}

// Custom methods
void GameplayResource::add_inventory_item(const String &item) {
    inventory.append(item);
    emit_changed();
}

void GameplayResource::remove_inventory_item(const String &item) {
    int index = inventory.find(item);
    if (index != -1) {
        inventory.remove_at(index);
        emit_changed();
    }
}

void GameplayResource::increase_score(int amount) {
    score += amount;
    emit_changed();
}

void GameplayResource::level_up() {
    level++;
    health = 100.0f; // Reset health on level up
    emit_changed();
}
