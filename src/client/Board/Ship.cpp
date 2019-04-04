#include "Ship.h"
#include "client/Core/ServiceLocator.h"
#include <fstream>

Ship::Ship()
{
  renderer = Locator::getRenderer();

  // Configure ship
  sprite.setSprite("data/UI/BOARD/ship.png");
  ship_config = file_handler.openAsJSON("CONFIGS/ship_config.json");
  sprite.setPosition(
    Vector2(ship_config["default_pos"][0], ship_config["default_pos"][1]));
  sprite.setDimensions(
    Vector2(ship_config["render_size"][0], ship_config["render_size"][1]));

  // Build up the rooms in the ship
  rooms.emplace_back("COCKPIT", sprite.getPosition(), ship_rooms::COCKPIT);
  rooms.emplace_back(
    "SUPPLY_BAY", sprite.getPosition(), ship_rooms::SUPPLY_BAY);
  rooms.emplace_back("MEDBAY", sprite.getPosition(), ship_rooms::MEDIC_WING);
  rooms.emplace_back(
    "COMMUNICATIONS", sprite.getPosition(), ship_rooms::COMMUNICATIONS_ROOM);
  rooms.emplace_back("ENGINE", sprite.getPosition(), ship_rooms::ENGINE_ROOM);
}

/* Render the ship */
void Ship::render()
{
  sprite.render();
}